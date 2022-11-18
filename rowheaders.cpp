#include "rowheaders.h"

namespace xls {

WorkBookHeaders::WorkBookHeaders(std::set<QString> &&sheets, std::vector<Header> &&headers)
{
    for(auto &&s : sheets)
    {
        auto &sh = _sheet_headers.emplace_back(std::move(s));
        sh.set(headers);
    }
    headers.clear();
}

bool WorkBookHeaders::select_sheet(const QString &sheet)
{
    _selected_sheet_headers =
            std::find_if(_sheet_headers.begin(), _sheet_headers.end(),
                         [&](const SheetHeaders &sh){ return sh.sheet() == sheet; });

    return _selected_sheet_headers != _sheet_headers.end();
}

void WorkBookHeaders::add_sheet(const QString &sheet)
{
    for(auto const &h : _sheet_headers)
    {
        if(h.sheet() == sheet)
            return;
    }

    _sheet_headers.push_back(sheet);
    _selected_sheet_headers = _sheet_headers.end()-1;
}

void WorkBookHeaders::add(const QString &sheet, const Header &header)
{
    if(select_sheet(sheet))
        _selected_sheet_headers->add(header);
}

void WorkBookHeaders::set(SheetHeaders &&shheaders)
{
    _sheet_headers.emplace_back(std::move(shheaders));
    _selected_sheet_headers = _sheet_headers.end()-1;
}

void WorkBookHeaders::set_row(int row, const QString &sheet)
{
    if(!sheet.isEmpty())
    {
        if(!select_sheet(sheet))
            return ;

        _selected_sheet_headers->set_row(row);
    }
    for(auto &sh : _sheet_headers)
        sh.set_row(row);
}

void WorkBookHeaders::add(const Header &header)
{
    _selected_sheet_headers->add(header);
}

void WorkBookHeaders::add(Header &&header)
{
    _selected_sheet_headers->add(std::move(header));
}

const std::vector<Header> &WorkBookHeaders::get_headers(const QString &sheet)
{
    Q_ASSERT(select_sheet(sheet));

    for(auto const &h : _sheet_headers)
    {
        if(h.sheet() == _selected_sheet_headers->sheet())
            return h.headers();
    }
    return {};  // never evaluated
}

const std::vector<Header> &WorkBookHeaders::get_headers()
{
    check_sel_sh();
    return _selected_sheet_headers->headers();
}

void WorkBookHeaders::clear()
{
    check_sel_sh();
    _selected_sheet_headers->clear();
}

void WorkBookHeaders::clear(const QString &sheet)
{
    Q_ASSERT(select_sheet(sheet));
    _selected_sheet_headers->clear();
}

std::vector<Header>::const_iterator WorkBookHeaders::find(const QString &header, const QString &sheet)
{
    Q_ASSERT(!_sheet_headers.empty());

    if(!sheet.isEmpty())
    {
        if(!select_sheet(sheet))
            return _sheet_headers.begin()->headers().end();
        else
            return _selected_sheet_headers->find(header);
    }
    return _sheet_headers.begin()->find(header);
}

std::vector<Header>::const_iterator WorkBookHeaders::find(const QXlsx::CellReference &cf) const
{
    check_sel_sh();
    auto col = cf.column();
    auto row = cf.row();
    if(col < 0 && row < 0)
        return _selected_sheet_headers->headers().end();

    auto &hs = _selected_sheet_headers->headers();
    for(auto hIt = hs.begin(); hIt != hs.end(); ++hIt)
    {
        // if Referenced Cell's row is set to -1 -> find first matching header for given col
        // if Referenced Cell's col is set to -1 -> find first matching header for given row
        if((col < 0 && hIt->row() == row) || (row < 0 && hIt->col() == col)
            || (hIt->col() == col && hIt->row() == row))
        {
            return hIt;
        }
    }
    return _selected_sheet_headers->headers().end();
}

void WorkBookHeaders::check_sel_sh() const
{
    Q_ASSERT(_selected_sheet_headers != _sheet_headers.end());
}

void SheetHeaders::add(Header &&header)
{
    if(!check_bf_add(header))
        return;

    _headers.emplace_back(std::move(header));
}

void SheetHeaders::add(const Header &header)
{
    if(!check_bf_add(header))
        return ;

    _headers.push_back(header);
}

void SheetHeaders::set(const std::vector<Header> &hds)
{
    for(auto const &h : hds)
    {
        if(!h.is_valid())
            return ;
    }
    _headers = hds;
}

void SheetHeaders::set(SheetHeaders &&shheaders)
{
    if(shheaders.is_valid())
        *this = std::move(shheaders);
}

void SheetHeaders::set_sheet(const QString &sheet)
{
    _sheet_name = sheet;
}

void SheetHeaders::set_row(int row)
{
    Q_ASSERT(row > 0);

    for(auto &h : _headers)
        h.set_row(row);
}

std::vector<Header>::const_iterator SheetHeaders::find(const Header &header) const
{
    for(auto hIt = _headers.begin(); hIt != _headers.end(); ++hIt)
    {
        if(*hIt == header)
            return hIt;
    }
    return _headers.end();
}

std::vector<Header>::const_iterator SheetHeaders::find(const QString &header_text) const
{
    for(auto hIt = _headers.begin(); hIt != _headers.end(); ++hIt)
    {
        if(hIt->text == header_text)
            return hIt;
    }
    return _headers.end();
}

bool SheetHeaders::is_valid() const
{
    for(auto const &h : _headers)
    {
        if(!h.is_valid())
            return false;
    }
    return !_sheet_name.isEmpty();
}

bool SheetHeaders::check_bf_add(const Header &h) const
{
    if(!h.is_valid())
        return false;

    if(find(h) != _headers.end())
        return false;

    return true;
}

}   // namespace xls
