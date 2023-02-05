#ifndef WORKBOOKHEADERS_H
#define WORKBOOKHEADERS_H

#include <QString>
#include <QVariant>

#include <set>
#include <map>

#include "xlsxdocument.h"
#include "xlsxworkbook.h"
#include "xlsxworksheet.h"

namespace xls
{

struct Header
{
    Header() = default;
    Header(int row, int col, QString header)
        : text(header)
    {
        if(col <= 0 || row <= 0)
        {
            invalidate();
        }
        _row = row;
        _col = col;
    }

    bool is_valid() const
    { return _col > 0 && _row > 0 && !text.isEmpty(); }

    void set_col(int col)
    {
        _col = col;
    }

    void set_row(int row)
    {
        _row = row;
    }

    int col() const { return _col; }
    int row() const { return _row; }

    QString text = "";

//    operator QString() const
//    {
//        return text;
//    }

    bool operator <(const Header &other) const
    {
        return _row < other._row ?
                    true : _col < other._col;
    }

    bool operator ==(const Header &other) const
    {
        return _col == other._col && _row == other._row;
    }

    bool operator ==(const QString &other) const
    {
        return text == other;
    }

private:
    int _row, _col = -1;

    void invalidate()
    {
        _col = -1;
        _row = -1;
        text.clear();
    }
};

//class SheetHeaders
//{
//public:
//    SheetHeaders() = default;
//    SheetHeaders(const QString &sheet)
//        : _sheet_name(sheet) {}
//    SheetHeaders(const QString &sheet, const std::vector<Header> &hds)
//        : _sheet_name(sheet), _headers(hds) {}
//    SheetHeaders(const QString &sheet, std::vector<Header> &&hds)
//        : _sheet_name(sheet), _headers(std::move(hds)) {}

//    void add(Header &&header);
//    void add(const Header &header);

//    void set(const std::vector<Header> &hds);
//    void set(SheetHeaders &&shheaders);
//    void set_sheet(const QString &sheet);
//    void set_row(int row);

//    const std::vector<Header> &headers() const
//    { return _headers; }

//    QString sheet() const
//    { return _sheet_name; }

//    std::vector<Header>::const_iterator find(const Header &header) const;
//    std::vector<Header>::const_iterator find(const QString &header_text) const;

//    auto begin() const
//    { return _headers.begin(); }

//    auto end() const
//    { return _headers.end(); }

//    void clear()
//    { _headers.clear(); }

//    auto size() const
//    { return _headers.size(); }

//    bool is_valid() const;

//    bool operator ==(const SheetHeaders &other) const
//    { return _sheet_name == other._sheet_name
//                && _headers == other._headers; }

//private:
//    QString _sheet_name;
//    std::vector<Header> _headers;

//    bool check_bf_add(const Header &h) const;
//};

using headers_cont = std::vector<Header>;

class WorkBookHeaders
{
public:    
    WorkBookHeaders() = default;

    WorkBookHeaders(const std::set<QString> &sheets, const headers_cont &headers);

    void add_sheet(const QString &sheet);

    void add_many(const QString &sheet, headers_cont &&headers);
    void add_many(const QString &sheet, const headers_cont &headers);
    void add(const QString &sheet, Header &&header);

    void set_row(int row, const QString &sheet = "");

    auto get_headers(const QString &sheet)
    {
        return _sheet_headers.at(sheet);
    }

    auto begin() const
    {
        return _sheet_headers.begin();
    }

    auto end() const
    { return _sheet_headers.end(); }

    size_t size() const
    {
        return _sheet_headers.size();
    }

    size_t size(const QString &sheet)
    {
        if(_sheet_headers.empty())
            return 0;

        try
        {
            return _sheet_headers.at(sheet).size();
        }
        catch(...)
        {
            return 0;
        }
    }

    void clear(const QString &sheet);
    void clear();

    auto empty() const
    { return _sheet_headers.empty(); }

    const Header *find(const QXlsx::CellReference &cf, const QString &sheet);
    const Header *find(const QString &header, const QString &sheet = "");

    headers_cont &operator[](const QString &sheet);

private:
    std::map<QString, headers_cont> _sheet_headers;
};

}   // namespace xls

#endif // WORKBOOKHEADERS_H
