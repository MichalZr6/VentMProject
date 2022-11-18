#include "ventpackelemfactory.h"

namespace vph
{

    const std::map<numtype, QString> headers = {{numtype::sys, system}, {numtype::idx, index}, {numtype::quant, quantity},
                                      {numtype::type, type}, {numtype::name, name}, {numtype::dim, dimensions},
                                      {numtype::mat, material}, {numtype::area, area}, {numtype::totarea, total_area},
                                      {numtype::prod, maker}, {numtype::notes, comments}};
    bool find(const QString &text)
    {
        for(auto const &h : headers)
        {
            if(h.second == text)
                return true;
        }
        return false;
    }
}

VentPackElemFactory::VentPackElemFactory(const std::filesystem::path &path_to_file_listing_elements, VentElemFileType type)
    : VentElemFactory(path_to_file_listing_elements, type)
{
    _curr_row_values.reserve(30);

    _xlsfile = std::make_unique<QXlsx::Document>(QString::fromStdWString(path_to_file_listing_elements.wstring()));
    //QXlsx::Document... - już tutaj wczytywanie pliku, duży nawet kilka sekund, mały kilka milisekund

    if(!_xlsfile->load())
    {
        log_critical("VentPackElemFactory constructor: Nie udało się otworzyć pliku. "
                     "Możliwe że jest już otwarty w innym programie");
    }
}

bool VentPackElemFactory::load_elements_from_file()
{
    if(!is_valid())
        return false;

    int sheet_idx = 0;

    for(auto const &s : _xlsfile->sheetNames())
    {
        if(!select_sheet(s, sheet_idx))
            continue;

        process_xls_worksheet_data();

        sheet_idx++;
    }

    if(_is_valid && _elements.empty())
    {
        log_critical("Loading elements from file failed. Headers are valid, but no elements were found in the given file.");
        return false;
    }

    return true;
}

void VentPackElemFactory::process_xls_worksheet_data()
{
    int max_row = -1;
    int max_col = -1;
    _curr_wsheet->getFullCells(&max_row, &max_col);

    for(_curr_row = 0; _curr_row <= max_row; ++_curr_row)
    {
        for(int c = 0; c <= max_col; ++c)  // jak tutaj będzie c < max_col to będzie ok ale powyżej _curr_row musi być <= bo inaczej nie czyta ostatniego wiersza
        {
            QXlsx::Cell *cell = _xlsfile->cellAt(_curr_row, c);
            if(cell != NULL)
                _curr_row_values.push_back(cell->readValue());
        }
        if(_headers.size(_curr_wsheet->sheetName()) == 0)
        {
            find_headers();
            //find_curr_system_type();          // if headers were not found yet; means we also can search for system type -> system type is later read from elements
        }                                       // leave it commented out as system type doesnt need to be set here
        else                                    // update: its not needed (?)
            process_xls_row_data();

        _curr_row_values.clear();
    }
}

//void VentPackElemFactory::find_curr_system_type()
//{
//    if(_curr_row_values.empty())
//        return ;

//    QString val, prop;
//    for(int i = 0; i < _curr_row_values.size()-1; ++i)
//    {
//        if(!_curr_row_values[i].isValid())
//            continue;

//        prop = _curr_row_values[i].value<QString>();
//        if((prop == "Nazwa:" || prop == "Typ:" || prop == "Opis:")
//                && _curr_row_values[i+1].isValid())
//        {
//            val = _curr_row_values[i+1].value<QString>().toLower();
//            if(val.contains("nawiewny"))
//                _curr_airflow_sys_type = VentElem::AirflowSystemType::SUPPLY;
//            else if(val.contains("wywiewny"))
//                _curr_airflow_sys_type = VentElem::AirflowSystemType::INDOOR_EXTRACT;
//            else if(val.contains("wyrzutowy"))
//                _curr_airflow_sys_type = VentElem::AirflowSystemType::EXHAUST;
//            else if(val.contains("czerpny"))
//                _curr_airflow_sys_type = VentElem::AirflowSystemType::FRESH;
//        }
//    }
//}

void VentPackElemFactory::find_headers()
{
    xls::Header header;
    int fnd_ctr = 0;

    if(!_curr_wsheet)
        return;
    auto shname = _curr_wsheet->sheetName();
    _headers.add_sheet(shname);

    for(uint8_t i = 0; i < _curr_row_values.size(); ++i)
    {
        auto val = _curr_row_values[i].value<QString>();
        if(vph::find(val))
        {
            header = {_curr_row+1, i+1, val};       // QXlsx rows / cols start from 1
            if(header.is_valid())
            {
                fnd_ctr++;
                if(header.text == "Wymiary")
                {
                    for(; i < _curr_row_values.size(); ++i)
                    {
                        header = {_curr_row+1, i+1, _curr_row_values[i].isValid() ?
                                   _curr_row_values[i].value<QString>() : "Wymiary"};
                        if(header.text == "Materiał")
                        {
                            fnd_ctr++;
                            break;
                        }
                        else
                            _headers.add(std::move(header));
                    }
                }
                _headers.add(std::move(header));
            }
        }
    }

    if(fnd_ctr != vph::headers.size())     // all headers must be found
        _headers.clear();
    else
        _is_valid = true;
}

bool VentPackElemFactory::select_sheet(const QString &sheet, int idx)
{
    auto curr_sheet = _xlsfile->sheet(sheet);
    if(curr_sheet == NULL)
        return false;

    curr_sheet->workbook()->setActiveSheet(idx);
    _curr_wsheet = (QXlsx::Worksheet *) curr_sheet->workbook()->activeSheet();
    if(_curr_wsheet == NULL)
        return false;

    return true;
}

void VentPackElemFactory::process_xls_row_data()
{
    if(!is_valid() || _headers.empty() || !_curr_wsheet)
    {
        qDebug() << "[process_xls_row_data]: basic check failed";
        return ;
    }

    auto ve = std::make_unique<VentElem>();
    //ve->set_airflow_sys_type(_curr_airflow_sys_type);
    auto shname = _curr_wsheet->sheetName();
    _headers.select_sheet(shname);

    for(int col = 0; col < _curr_row_values.size(); ++col)
    {
        if(!_curr_row_values[col].isValid())
            continue;

        if(_headers.size() == 0)
        {
            qDebug() << "[process_xls_row_data]: none of defined headers was found (WorkBookHeaders) for sheet = " << shname;
            return ;
        }
        QXlsx::CellReference cref = {-1, col+1};
        auto header = _headers.find(cref);
        if(header != _headers.get_headers().end())
            set_vent_elem(*header, col, ve);
        else
        {
//            qDebug() << "[process_xls_row_data]: none of defined headers was found (WorkBookHeaders) for sheet = "
//                     << shname << " and col = " << col;
        }
    }
    ve->set_proper_length();     // when element is finally made, check/set its length for these which have no 'L' dimension

    ve->set_init_values(std::move(_curr_row_values));
    VentElemFactory::add(std::move(ve));
}

void VentPackElemFactory::set_vent_elem(const QString &header, int &col, std::unique_ptr<VentElem> &ve)
{
    if(header == vph::system)
        ve->set_system(_curr_row_values[col].value<QString>());
    else if(header == vph::index)
        ve->set_number(_curr_row_values[col].value<int>());
    else if(header == vph::quantity)
        ve->set_quantity(_curr_row_values[col].value<int>());
    else if(header == vph::name)
        ve->set_name(_curr_row_values[col].value<QString>());
    else if(header == vph::type)
        ve->set_VPelem_type(_curr_row_values[col].value<QString>());
    else if(header == vph::material)
        ve->set_material(_curr_row_values[col].value<QString>());
    else if(header == vph::area)
        ve->set_area(_curr_row_values[col].value<double>());
    else if(header == vph::total_area)
        ve->set_total_area(_curr_row_values[col].value<double>());
    else if(header == vph::maker)
        ve->set_manufacturer(_curr_row_values[col].value<QString>());
    else if(header == vph::comments)
        ve->set_comments(_curr_row_values[col].value<QString>());
    else if(header == vph::dimensions)
    {
        if(!ve->is_valid())
        {
            //means that this row is probably merged with prev one;
            //extract last element and set its dimensions
            if(_elements.empty())
            {
                log_warning("[set_vent_elem]: Reading dimensions failed in row "+ QString::number(_curr_row)
                            +". It's probably merged row in excel listing, but found no previous VP elements to append.");
            }
            else
            {
                ve = std::move(_elements.back());
                _elements.pop_back();   // erase nullptr
            }
        }
        set_elem_dimensions(col, ve);
        ve->set_properties();
    }
}

void VentPackElemFactory::set_elem_dimensions(int &col, std::unique_ptr<VentElem> &elem)
{
    QString v;

    for(; col < _curr_row_values.size(); ++col)
    {
        v = _curr_row_values[col].value<QString>();
        if(v.isEmpty())
            return ;

        if(v.endsWith("="))
        {
            v.chop(1);
            if(_curr_row_values[col+1].isValid())
                elem->set_dimension(v, _curr_row_values[col+1]);
        }
    }
}
