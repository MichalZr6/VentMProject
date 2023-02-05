#ifndef XLSXFILEMANAGER_H
#define XLSXFILEMANAGER_H

#include "../myIncludes/Functions.h"

#include "workbookheaders.h"
//QXlsx:
#include "xlsxdocument.h"
#include "xlsxworkbook.h"

#include <QFile>
#include <QLocale>
#include <QDateTime>

#include <vector>
#include <set>
#include <unordered_set>

namespace xls
{

class XlsxFileManager
{
public:
    enum class fmtType { STANDARD, INTEGER, DBL_PREC2PL, LONG_TEXT };
    XlsxFileManager();
    XlsxFileManager(QString fname);

    virtual void set_headers(const std::vector<Header> &headers, const QString &sheet);
    virtual void set_headers(const std::vector<Header> &headers, const std::set<QString> &sheets = {});
    virtual void set_headers(WorkBookHeaders &&wb_headers);
    virtual void set_headers(const WorkBookHeaders &wb_headers);

    void set_filename(QString name);
    void set_filepath(QString path);
    virtual void add_sheets(const std::set<QString> &names);

    virtual bool set_curr_sheet(const QString &sheet_name);
    void set_col_fmt(int col, const QXlsx::Format &fmt);
    void set_col_fmt(int col, fmtType type);
    QXlsx::CellReference find_text(const QString &text) const;

    QStringList getSheetNames() const;

    void write(const std::vector<QVariant> &row_values, const QXlsx::CellReference &startAddr);
    void write(const std::vector<QVariant> &row_values, int row, int col);
    void write(const xls::WorkBookHeaders &headers);
    void autosize_columns();
    bool save();
    void close();
    bool hasErrors() const;
    std::vector<QString> getErrors() const;

    QXlsx::Format get_col_fmt(int col) const;

    QString get_filename() const
    { return _fname; }
    ~XlsxFileManager();

    std::shared_ptr<QXlsx::Document> output_file;

private:
    std::map<fmtType, QXlsx::Format> _formats;
    std::map<QXlsx::Worksheet*, std::unordered_set<int>> _no_autosize_cols;
    WorkBookHeaders _headers;
    QString _fname = "ProgramOutput.xlsx";

    void init_formats();
    const QXlsx::Format &get_fmt(fmtType type) const;
    void set_default_sheet();

    const QString DEF_SHEET_NAME = "Sheet1";

    std::vector<QString> _errors;
    void log_error(QString err);
};

}   //xls namespace

#endif // XLSXFILEMANAGER_H
