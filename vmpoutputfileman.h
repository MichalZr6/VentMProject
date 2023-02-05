#ifndef VMPOUTPUTFILEMAN_H
#define VMPOUTPUTFILEMAN_H

#include "xlsxfilemanager.h"

#include "ventelem.h"

enum class GroupType { DUCTS = 10, CIRC_DUCTS = 20, RECT_DUCTS = 30, FITTINGS = 40,
                       CIRC_FITT = 50, RECT_FITT = 60, DEVICES = 70, OTHERS = 80,
                       FINAL = 90, SUMMARY = 110, UNDEFINED = 120};

class VMPOutputFileMan : public xls::XlsxFileManager
{
public:
    static const int headers_starting_row = 3;
    static const int offset_rows = 1;

    VMPOutputFileMan();
    VMPOutputFileMan(QString fname);

    void set_headers(xls::WorkBookHeaders &&wb_headers) override;

    void write_elements(const std::vector<std::unique_ptr<VentElem>> *elems);


private:
    static const std::map<GroupType, QString> _sheets;

    std::map<GroupType, int> _sheet_curr_row =
        {{ GroupType::DUCTS, headers_starting_row + 1 + offset_rows},
        { GroupType::FITTINGS, headers_starting_row + 1 + offset_rows},
        { GroupType::DEVICES, headers_starting_row + 1 + offset_rows},
        { GroupType::OTHERS, headers_starting_row + 1 + offset_rows},
        { GroupType::FINAL, headers_starting_row + 1 + offset_rows},
        { GroupType::SUMMARY, 2}};

    int _first_col = 2;
};

#endif // VMPOUTPUTFILEMAN_H
