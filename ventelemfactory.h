#ifndef VENTELEMFACTORY_H
#define VENTELEMFACTORY_H

#include <filesystem>

#include <QString>
#include <QDebug>

#include "rowheaders.h"
#include "ventelem.h"
#include "./../myIncludes/errorsandwarningslogger.h"

enum class VentElemFileType {VENTPACK, WENTYLE, USER, UNKNOWN};


class VentElemFactory
{
public:
    VentElemFactory(const std::filesystem::path &path_to_file_listing_elements,
                    VentElemFileType type = VentElemFileType::UNKNOWN);
    virtual bool load_elements_from_file() = 0;

    auto const &workbook_headers() const
    { return _headers; }

    const std::vector<std::unique_ptr<VentElem>> *elements() const;
    void add(std::unique_ptr<VentElem> &&ve);

    bool is_valid() const { return _is_valid; };
    QString get_errors() const;

protected:
    std::filesystem::path _listing_file_path;
    xls::WorkBookHeaders _headers;
    std::vector<std::unique_ptr<VentElem>> _elements;
    VentElemFileType _ftype;
    uint16_t _capacity = 100;
    ErrorsAndWarningsLogger _errors;

    bool _is_valid = true;

    bool basic_file_check();

    void log_critical(QString crit_msg);
    void log_warning(QString msg);


};

#endif // VENTELEMFACTORY_H
