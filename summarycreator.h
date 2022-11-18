#ifndef SUMMARYCREATOR_H
#define SUMMARYCREATOR_H

#include <memory>
#include <vector>

#include "xlsxfilemanager.h"
#include "ventelem.h"
#include "fixingsfasteners.h"
#include "ventinsulation.h"

class SummaryCreator
{
public:
    SummaryCreator() = default;
    SummaryCreator(const SummaryCreator &other) = delete;
    SummaryCreator(const std::unique_ptr<xls::XlsxFileManager> &ofile,
                   const std::vector<std::unique_ptr<VentElem>> *elems);

    double get_fittings_area() const
    { return _fittings_area; }

    double get_rect_fittings_area() const
    { return _fittings_area - _circ_fittings_area; }

    double get_circ_fittings_area() const
    { return _circ_fittings_area; }

    double get_ducts_area() const
    { return _ducts_area; }

    double get_rect_ducts_area() const
    { return _ducts_area - _circ_ducts_area; }

    double get_circ_ducts_area() const
    { return _circ_ducts_area; }

    double get_ducts_length() const
    { return _ducts_length; }

    double get_circ_ducts_length() const
    { return _circ_ducts_length; }

    double get_circ_fittings_length() const
    { return _circ_fittings_length; }

    double get_rect_ducts_length() const
    { return _ducts_length - _circ_ducts_length; }

    double get_rect_fittings_length() const
    { return _fittings_length - _circ_fittings_length; }

    auto get_elements_count() const
    { return _elems->size(); }

    auto get_devices_count() const
    { return _devices.size(); }

    void set_insulation(std::vector<QString> &&systems, const VentInsulation &insulation);

    const std::map<QString, int> &devices()
    {
        return _devices;
    }

private:
    const QString _suppExhUnit = "centrala wentylacyjna";
    const QString _ventilator = "wentylator";
    
    xls::XlsxFileManager *_ofile;
    const std::vector<std::unique_ptr<VentElem>> *_elems;
    FixingsFasteners _fixfastmat;
    int _elements_count = -1;
    std::map<QString, int> _devices;

    double _fittings_area = -1;
    double _fittings_length = -1;
    double _ducts_area = -1;
    double _ducts_length = -1;

    double _circ_fittings_area = -1;
    double _circ_fittings_length = -1;

    double _circ_ducts_area = -1;
    double _circ_ducts_length = -1;

    void count_elements();
    void count_ducts(double area, int length, const std::unique_ptr<VentElem> &e);
    void count_fittings(double area, int length, const std::unique_ptr<VentElem> &e);
    void check_for_air_handling_unit(const std::unique_ptr<VentElem> &elem);
    // void count_final_elements(const );
};

#endif // SUMMARYCREATOR_H
