#include "summarycreator.h"

SummaryCreator::SummaryCreator(const std::unique_ptr<xls::XlsxFileManager> &ofile,
                               const std::vector<std::unique_ptr<VentElem> > *elems)
{
    if(ofile && !elems->empty())
    {
        _values.resize(static_cast<int>(GroupType::OTHERS)+static_cast<int>(ValueType::NONE));
        _elements_count = 0;
        _ofile = ofile.get();
        _elems = elems;

        count_elements();
    }
}

double SummaryCreator::get(GroupType gt, ValueType vt) const
{
    Q_ASSERT(gt < GroupType::UNDEFINED && vt <= ValueType::PRICE);

    if(gt == GroupType::DEVICES)
        return _devices.size();   // you can get only devices' quantity here

    if(gt == GroupType::OTHERS)
        return _others.size();    // you can get only others' quantity here

    return _values[static_cast<int>(gt)+static_cast<int>(vt)];
}

void SummaryCreator::count_elements()
{
    for(int i = 0; i < _elems->size(); ++i)
    {
        auto &e = _elems->at(i);
        auto area = e->get_total_area();
        auto len = e->get_total_length();
        if(e->is_ductal())
        {
            sum_duct(area, len, e);
        }
        else if(e->is_fitting())
        {
            sum_fitting(area, len, e);
        }
        else if(e->is_device())
        {
            check_for_air_handling_unit(e);
        }
    }
}

void SummaryCreator::sum_duct(double area, double length, const std::unique_ptr<VentElem> &e)
{
    add(GroupType::DUCTS, ValueType::AREA, area);
    add(GroupType::DUCTS, ValueType::LENGTH, length);
    add(GroupType::DUCTS, ValueType::QUANTITY, 1);
    if(e->check_type(VentElem::IS_CIRCULAR))
    {
        add(GroupType::CIRC_DUCTS, ValueType::AREA, area);
        add(GroupType::CIRC_DUCTS, ValueType::LENGTH, length);
        add(GroupType::CIRC_DUCTS, ValueType::QUANTITY, 1);
    }
    else
    {
        add(GroupType::RECT_DUCTS, ValueType::AREA, area);
        add(GroupType::RECT_DUCTS, ValueType::LENGTH, length);
        add(GroupType::RECT_DUCTS, ValueType::QUANTITY, 1);
    }
}

void SummaryCreator::sum_fitting(double area, double length, const std::unique_ptr<VentElem> &e)
{
    add(GroupType::FITTINGS, ValueType::AREA, area);
    add(GroupType::FITTINGS, ValueType::LENGTH, length);
    add(GroupType::FITTINGS, ValueType::QUANTITY, 1);
    if(e->check_type(VentElem::IS_CIRCULAR))
    {
        add(GroupType::CIRC_FITT, ValueType::AREA, area);
        add(GroupType::CIRC_FITT, ValueType::LENGTH, length);
        add(GroupType::CIRC_FITT, ValueType::QUANTITY, 1);
    }
    else
    {
        add(GroupType::RECT_FITT, ValueType::AREA, area);
        add(GroupType::RECT_FITT, ValueType::LENGTH, length);
        add(GroupType::RECT_FITT, ValueType::QUANTITY, 1);
    }
}

void SummaryCreator::check_for_air_handling_unit(const std::unique_ptr<VentElem> &elem)
{
    auto name = elem->get_name().toLower();
    if(name.indexOf(_suppExhUnit) >= 0)
    {
        _devices.push_back(elem.get());
    }
    else if(name.indexOf(_fan) >= 0)
    {
        _devices.push_back(elem.get());
    }
}

void SummaryCreator::add(GroupType gt, ValueType vt, double value)
{
    Q_ASSERT(gt < GroupType::DEVICES && vt < ValueType::PRICE);  // DEVICES, OTHERS and PRICE cannot be set here
    Q_ASSERT(!(vt == ValueType::QUANTITY && value != 1));

    _values[static_cast<int>(gt)+static_cast<int>(vt)] += value;
}
