#ifndef VENTPACKELEMFACTORY_H
#define VENTPACKELEMFACTORY_H

#include <memory>
#include <filesystem>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

#include "ventelemfactory.h"

using qvarvec = std::vector<QVariant>;

/* VentPack headers */
namespace vph {
    const QString system = "Sys.";
    const QString index = "Nr";
    const QString quantity = "Szt.";
    const QString type = "Typ";
    const QString name = "Nazwa";
    const QString dimensions = "Wymiary";
    const QString material = "Materiał";
    const QString area = "Pow. [m2]";
    const QString total_area = "Pow. całk. [m2]";
    const QString maker = "Producent";
    const QString comments = "Uwagi";
    enum class numtype { sys, idx, quant, type, name, dim, mat, area, totarea, prod, notes };
} // VP_headers

class VentPackElemFactory : public VentElemFactory
{
public:
    VentPackElemFactory(const std::filesystem::path &path_to_file_listing_elements, VentElemFileType type);

    bool load_elements_from_file() override final;

private:
    std::shared_ptr<QXlsx::Document> _xlsfile;
    QXlsx::Worksheet *_curr_wsheet;
    qvarvec _curr_row_values;
    int _curr_row;

    void process_xls_worksheet_data();
    void process_xls_row_data();
    void set_vent_elem(const QString &header, int &col, std::unique_ptr<VentElem> &ve);
    void set_elem_dimensions(int &col, std::unique_ptr<VentElem> &ve);

    //void find_curr_system_type();
    void find_headers();

    bool select_sheet(const QString &sheet, int idx);
};

#endif // VENTPACKELEMFACTORY_H
