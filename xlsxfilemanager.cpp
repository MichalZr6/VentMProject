#include "xlsxfilemanager.h"

namespace xls
{

XlsxFileManager::XlsxFileManager()
{
    _output_file = std::make_unique<QXlsx::Document>();
    init_formats();
}

XlsxFileManager::XlsxFileManager(QString fname)
    : XlsxFileManager()
{
    set_filename(fname);
}

void XlsxFileManager::set_headers(const std::vector<Header> &headers, const QString &sheet)
{
    set_curr_sheet(sheet);
    _headers.set({sheet, headers});
}

void XlsxFileManager::set_headers(const std::vector<Header> &headers, const std::set<QString> &sheets)
{
    Q_ASSERT(sheets.size() <= getSheetNames().size());

    if(!sheets.empty())
    {
        for(auto const &shname : getSheetNames())
        {
            for(auto const &s : sheets)
            {
                if(shname == s)
                {
                    set_headers(headers, s);
                }
            }
        }
    }
}

void XlsxFileManager::set_headers(WorkBookHeaders &&wb_headers)
{
    _headers = std::move(wb_headers);
}

void XlsxFileManager::set_headers(const WorkBookHeaders &wb_headers)
{
    _headers = wb_headers;
}

void XlsxFileManager::set_filename(QString name)
{
    if(name.isEmpty())
        return ;

    if(name.right(5) != ".xlsx")
    {
        log_error("[OutputXlsxGenerator::set_filename]: Nieprawidłowe rozszerzenie pliku.");
        return ;
    }

    _fname = name;
}

void XlsxFileManager::add_sheets(const std::vector<QString> &names)
{
    if(names.empty() && getSheetNames().empty())
    {
       _output_file->workbook()->addSheet(DEF_SHEET_NAME);
        return;
    }

    for(auto const &n : names)
    {
        if(n.isEmpty())
            continue;
        if(_output_file->workbook()->addSheet(n) == 0)
        {
            log_error("[XlsxFileManager::add_sheets]: Błąd dodawania arkusza o nazwie: "+ n);
            return ;
        }
    }
}

bool XlsxFileManager::save()
{
    Q_ASSERT(_output_file);

    autosize_columns();

    return _output_file->saveAs(_fname);
}

void XlsxFileManager::close()
{
    _no_autosize_cols.clear();
    _output_file.reset();
    _headers.clear();
    _formats.clear();
    _fname = "ProgramOutput.xlsx";
}

bool XlsxFileManager::hasErrors() const
{
    return !_errors.empty();
}

std::vector<QString> XlsxFileManager::getErrors() const
{
    return _errors;
}

void XlsxFileManager::init_formats()
{
    auto fmt = QXlsx::Format();

    fmt.setTextWrap(true);
    fmt.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    fmt.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    _formats[fmtType::STANDARD] = fmt;

    fmt.setNumberFormatIndex(1);
    _formats[fmtType::INTEGER] = fmt;

    fmt.setNumberFormatIndex(2);
    _formats[fmtType::DBL_PREC2PL] = fmt;
}

const QXlsx::Format &XlsxFileManager::get_fmt(fmtType type) const
{
    return _formats.at(type);
}

QXlsx::Format XlsxFileManager::get_col_fmt(int col) const
{
    return _output_file->columnFormat(col);
}

XlsxFileManager::~XlsxFileManager()
{
    if(!_errors.empty())
    {
        QFile file("errlog.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QTextStream out(&file);
            QLocale loc = QLocale(QLocale::Polish, QLocale::Europe);
            for(auto const &e : _errors)
            {
                out << "[" <<
                       loc.toString(QDateTime::currentDateTime(), "dd / MM / yyyy / hh:mm:ss")
                    << "]: " << e << "\n";
            }
            file.close();
        }
    }
}

void XlsxFileManager::set_default_sheet()
{
    if(getSheetNames().empty())
    {
        set_curr_sheet(DEF_SHEET_NAME);
    }
}

// returns {row, col}
QXlsx::CellReference XlsxFileManager::find_text(const QString &text) const
{
    auto sh = _output_file->sheetNames();
    int maxrow = -1;
    int maxcol = -1;
    for(auto const & s : sh)
    {
        auto currsheet = (QXlsx::Worksheet*) _output_file->sheet(s);
        if(currsheet == NULL)
            continue;

        currsheet->getFullCells(&maxrow, &maxcol);

        for(int r = 0; r < maxrow; ++r)
        {
            for(int c = 0; c < maxcol; ++c)
            {
                auto cell = _output_file->cellAt(r, c);
                if(!cell)
                    continue;

                if(cell->value().toString() == text)
                    return {r+1, c+1};
            }
        }
    }
    return {-1, -1};
}

QStringList XlsxFileManager::getSheetNames() const
{
    return _output_file->sheetNames();
}

void XlsxFileManager::write(const std::vector<QVariant> &row_values, const QXlsx::CellReference &startAddr)
{
    if(!startAddr.isValid())
        return ;

    write(row_values, startAddr.row(), startAddr.column());
}

void XlsxFileManager::write(const std::vector<QVariant> &row_values, int row, int col)
{
    QVariant val;
    bool check;

    for(int c = col; c <= row_values.size(); ++c)
    {
        val = row_values[c-1];
        if(!val.isValid())
            continue;

        check = is_number(val.toString().toStdString(), '.');
        if(check)
        {
            auto idx = val.toString().indexOf('.');
            _output_file->autosizeColumnWidth(c);
            if(idx < 0)
                _output_file->write(row, c, val.toInt(), get_fmt(fmtType::INTEGER));
            else
            {
                QXlsx::Format fmt;
                fmt.setNumberFormatIndex(2);
                _output_file->write(row, c, val.toDouble(), get_fmt(fmtType::DBL_PREC2PL));
            }

            continue;
        }

        _output_file->write(row, c, val, get_fmt(fmtType::STANDARD));
        if(val.toString().size() < 20)
            // will be autosized
            continue;
        else if(val.toString().size() > 30)
            // very long text:
            _output_file->setColumnWidth(c, 40);
        else
            //long text:
            _output_file->setColumnWidth(c, 20);

        // col width is set, autosize will break it
        _no_autosize_cols[_output_file->currentWorksheet()].insert(c);
    }
}

void XlsxFileManager::write(const WorkBookHeaders &headers)
{
    set_headers(headers);
    for(auto const &sh : headers)
    {
        _output_file->selectSheet(sh.sheet());
        int start_merge_idx = -1;
        int end_merge_idx = -1;
        auto headers = sh.headers();
        for(int i = 0; i < headers.size(); ++i)
        {
            if(i != 0)
            {
                if(headers[i].text == headers[i-1].text)
                {
                    if(start_merge_idx == -1)
                        start_merge_idx = i - 1;
                    else
                        end_merge_idx = i;
                }
            }
            _output_file->write(headers[i].row(), headers[i].col(), headers[i].text, get_fmt(fmtType::STANDARD));
        }
        if(start_merge_idx >= 0)
        {
            const auto & hs = headers[start_merge_idx];
            const auto & he = headers[end_merge_idx];
            _output_file->mergeCells( { hs.row(), hs.col(), he.row(), he.col() });
        }
    }
}

void XlsxFileManager::autosize_columns()
{
    int maxcols, maxrows;
    for(auto const &sh : getSheetNames())
    {
        _output_file->selectSheet(sh);
        _output_file->currentWorksheet()->getFullCells(&maxrows, &maxcols);
        for(int c = 0; c < maxcols; ++c)
        {
            if(_no_autosize_cols[_output_file->currentWorksheet()].find(c)
                    == _no_autosize_cols[_output_file->currentWorksheet()].end())
                _output_file->autosizeColumnWidth(c);
        }
    }
}

bool XlsxFileManager::set_curr_sheet(const QString &sheet_name)
{
    if(!_output_file->selectSheet(sheet_name))
    {
        log_error("[OutputXlsxGenerator::set_curr_sheet]: Nieprawidłowa nazwa arkusza: "+ sheet_name);
        return false;
    }
    if(!_headers.select_sheet(sheet_name))
    {
        log_error("[OutputXlsxGenerator::set_curr_sheet]: Nieprawidłowa nazwa arkusza: "+ sheet_name +" dla parametru _headers");
        return false;
    }
    return true;
}

void XlsxFileManager::set_col_fmt(int col, const QXlsx::Format &fmt)
{
    _output_file->setColumnFormat(col, fmt);
}

void XlsxFileManager::set_col_fmt(int col, fmtType type)
{
    set_col_fmt(col, get_fmt(type));
}

void XlsxFileManager::log_error(QString err)
{
    _errors.push_back(err);
}

} // xls namespace
