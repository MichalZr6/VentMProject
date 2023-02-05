#include "vmpoutputfileman.h"

const std::map<GroupType, QString> VMPOutputFileMan::_sheets = {
    { GroupType::DUCTS, "Kanały" },
    { GroupType::FITTINGS, "Kształtki" },
    { GroupType::DEVICES, "Urządzenia" },
    { GroupType::OTHERS, "Inne" },
    { GroupType::FINAL, "Elementy końcowe" },
    { GroupType::SUMMARY, "Podsumowanie" },
};

VMPOutputFileMan::VMPOutputFileMan()
{

}

VMPOutputFileMan::VMPOutputFileMan(QString fname)
    : xls::XlsxFileManager(fname)
{
    for(auto const &s : _sheets)
        output_file->addSheet(s.second);
}

void VMPOutputFileMan::set_headers(xls::WorkBookHeaders &&wb_headers)
{
    Q_ASSERT(!wb_headers.empty());
    xls::WorkBookHeaders ofile_hds;
    std::vector<xls::Header> hds = wb_headers.begin()->second;
    for(auto const &s : _sheets)
    {
        if(s.first == GroupType::SUMMARY)
            continue;

        ofile_hds.add_many(s.second, hds);
    }
    ofile_hds.set_row(headers_starting_row);
    xls::XlsxFileManager::write(std::move(ofile_hds));
}

void VMPOutputFileMan::write_elements(const std::vector<std::unique_ptr<VentElem> > *elems)
{
    GroupType gt = GroupType::UNDEFINED;
    for(auto it = elems->begin(); it != elems->end(); ++it)
    {
        if(it->get()->is_fitting())
            gt = GroupType::FITTINGS;
        else if(it->get()->is_ductal())
            gt = GroupType::DUCTS;
        else if(it->get()->is_device())
            gt = GroupType::DEVICES;
        else if(it->get()->is_final())
            gt = GroupType::FINAL;
        else
            gt = GroupType::OTHERS;

        //qDebug() << "element: " << it->get()->get_name();

        set_curr_sheet(_sheets.at(gt));
        write(it->get()->get_initial_values(),
                       {_sheet_curr_row[gt]++, _first_col});     // {start_row, start_col}
    }
}
