#ifndef SUMMARYCREATOR_H
#define SUMMARYCREATOR_H

#include <memory>
#include <vector>

#include "vmpoutputfileman.h"
#include "ventelem.h"
#include "fixingsfasteners.h"
#include "ventinsulation.h"
#include "qstrval.h"

class SummaryCreator
{
public:
    SummaryCreator() = delete;
    SummaryCreator(const SummaryCreator &other) = delete;
    SummaryCreator(const std::unique_ptr<VMPOutputFileMan> &ofile,
                   const std::vector<std::unique_ptr<VentElem>> *elems);

    double get(GroupType gt, ValueType vt) const;

    const std::vector<const VentElem *> &devices() const
    {
        return _devices;
    }

private:
    const QString _suppExhUnit = "centrala wentylacyjna";
    const QString _fan = "wentylator";

    std::vector<double> _values;
    
    VMPOutputFileMan *_ofile;
    const std::vector<std::unique_ptr<VentElem>> *_elems;
    FixingsFasteners _fixfastmat;
    int _elements_count = -1;
    std::vector<const VentElem *> _devices;
    std::vector<const VentElem *> _others;
    std::vector<const VentElem *> _finals;

    void count_elements();
    void add_duct(double area, double length, const std::unique_ptr<VentElem> &e);
    void add_fitting(double area, double length, const std::unique_ptr<VentElem> &e);
    void check_for_air_handling_unit(const std::unique_ptr<VentElem> &e);
    void add_final(const std::unique_ptr<VentElem> &e);
    // void count_final_elements(const );

    void add(GroupType gt, ValueType vt, double value);
};

#endif // SUMMARYCREATOR_H
