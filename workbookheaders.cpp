#include "workbookheaders.h"

namespace xls {

WorkBookHeaders::WorkBookHeaders(const std::set<QString> &sheets, const headers_cont &headers)
{
    for(auto &s : sheets)
    {
        _sheet_headers[s] = headers;
    }
}

void WorkBookHeaders::add_sheet(const QString &sheet)
{
    _sheet_headers[sheet] = {};
}

void WorkBookHeaders::add_many(const QString &sheet, headers_cont &&headers)
{
    _sheet_headers[sheet] = std::move(headers);
}

void WorkBookHeaders::add_many(const QString &sheet, const headers_cont &headers)
{
    _sheet_headers[sheet] = headers;
}

void WorkBookHeaders::add(const QString &sheet, Header &&header)
{
    _sheet_headers[sheet].emplace_back(std::move(header));
}

void WorkBookHeaders::set_row(int row, const QString &sheet)
{
    Q_ASSERT(row > 0);

    if(!sheet.isEmpty())
    {
        for(auto &h : _sheet_headers.at(sheet))
            h.set_row(row);
    }
    for(auto &sh : _sheet_headers)
    {
        for(auto &h : sh.second)
            h.set_row(row);
    }
}

void WorkBookHeaders::clear()
{
    _sheet_headers.clear();
}

void WorkBookHeaders::clear(const QString &sheet)
{
    _sheet_headers.at(sheet).clear();
}

const Header *WorkBookHeaders::find(const QString &header, const QString &sheet)
{
    Q_ASSERT(!_sheet_headers.empty());

    if(!sheet.isEmpty())
    {
        auto f = std::find(_sheet_headers.at(sheet).begin(), _sheet_headers.at(sheet).end(), header);
        return &*f;
    }

    for(auto const &sh : _sheet_headers)
    {
        auto f = std::find(sh.second.begin(), sh.second.end(), header);
        if(f != sh.second.end())
            return &*f;
    }

    return nullptr;
}

headers_cont &WorkBookHeaders::operator[](const QString &sheet)
{
    Q_ASSERT(!_sheet_headers.empty());
    return _sheet_headers[sheet];
}

const Header *WorkBookHeaders::find(const QXlsx::CellReference &cf, const QString &sheet)
{
    Q_ASSERT(!_sheet_headers.empty());
    auto col = cf.column();
    auto row = cf.row();
    if(col < 0 && row < 0)
        return nullptr;

    auto &hs = _sheet_headers.at(sheet);
    auto hssize = hs.size();
    for(size_t i = 0; i < hssize; ++i)
    {
        // if Referenced Cell's row is set to -1 -> find first matching header for given col
        // if Referenced Cell's col is set to -1 -> find first matching header for given row
        if((col < 0 && hs[i].row() == row) || (row < 0 && hs[i].col() == col)
            || (hs[i].col() == col && hs[i].row() == row))
        {
            return &hs[i];
        }
    }
    return nullptr;
}

}   // namespace xls
