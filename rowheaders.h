#ifndef ROWHEADERS_H
#define ROWHEADERS_H

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

    operator QString() const
    {
        return text;
    }

    bool operator <(const Header &other) const
    {
        return _row < other._row ?
                    true : _col < other._col;
    }

    bool operator ==(const Header &other) const
    {
        return _col == other._col && _row == other._row;
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

class SheetHeaders
{
public:
    SheetHeaders() = default;
    SheetHeaders(const QString &sheet)
        : _sheet_name(sheet) {}
    SheetHeaders(const QString &sheet, const std::vector<Header> &hds)
        : _sheet_name(sheet), _headers(hds) {}
    SheetHeaders(const QString &sheet, std::vector<Header> &&hds)
        : _sheet_name(sheet), _headers(std::move(hds)) {}

    void add(Header &&header);
    void add(const Header &header);

    void set(const std::vector<Header> &hds);
    void set(SheetHeaders &&shheaders);
    void set_sheet(const QString &sheet);
    void set_row(int row);

    const std::vector<Header> &headers() const
    { return _headers; }

    QString sheet() const
    { return _sheet_name; }

    std::vector<Header>::const_iterator find(const Header &header) const;
    std::vector<Header>::const_iterator find(const QString &header_text) const;

    auto begin() const
    { return _headers.begin(); }

    auto end() const
    { return _headers.end(); }

    void clear()
    { _headers.clear(); }

    auto size() const
    { return _headers.size(); }

    bool is_valid() const;

    bool operator ==(const SheetHeaders &other) const
    { return _sheet_name == other._sheet_name
                && _headers == other._headers; }

private:
    QString _sheet_name;
    std::vector<Header> _headers;

    bool check_bf_add(const Header &h) const;
};

class WorkBookHeaders
{
public:    
    WorkBookHeaders() = default;
    WorkBookHeaders(std::set<QString> &&sheets, std::vector<Header> &&headers);

    bool select_sheet(const QString &sheet);
    void add_sheet(const QString &sheet);

    void add(const QString &sheet, const Header &header);
    void add(const Header &header);
    void add(Header &&header);

    void set(SheetHeaders &&shheaders);
    void set_row(int row, const QString &sheet = "");

    const std::vector<Header> &get_headers(const QString &sheet);
    const std::vector<Header> &get_headers();

    auto begin() const
    { return _sheet_headers.begin(); }

    auto end() const
    { return _sheet_headers.end(); }

    auto size() const
    {
        return _selected_sheet_headers == _sheet_headers.end() ?
                    0 : _selected_sheet_headers->size();
    }

    auto size(const QString &sheet)
    {
        select_sheet(sheet);
        return size();
    }

    void clear();
    void clear(const QString &sheet);

    auto empty() const
    { return _sheet_headers.empty(); }

    std::vector<Header>::const_iterator find(const QXlsx::CellReference &cf) const;
    std::vector<Header>::const_iterator find(const QString &header, const QString &sheet = "");

private:
    std::vector<SheetHeaders> _sheet_headers;
    std::vector<SheetHeaders>::iterator _selected_sheet_headers;

    void check_sel_sh() const;
};

}   // namespace xls

#endif // ROWHEADERS_H
