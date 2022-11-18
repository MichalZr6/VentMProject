#include "summarycreator.h"

SummaryCreator::SummaryCreator(const std::unique_ptr<xls::XlsxFileManager> &ofile,
                               const std::vector<std::unique_ptr<VentElem> > *elems)
{
    if(ofile && !elems->empty())
    {
        _fittings_area = 0;
        _fittings_length = 0;
        _circ_fittings_area = 0;
        _circ_fittings_length = 0;
        _ducts_area = 0;
        _ducts_length = 0;
        _circ_ducts_area = 0;
        _circ_ducts_length = 0;
        _elements_count = 0;
        _ofile = ofile.get();
        _elems = elems;

        count_elements();
    }
}

void SummaryCreator::set_insulation(std::vector<QString> &&systems, const VentInsulation &insulation)
{
    for(auto const &s : systems)
    {
        sdfa
    }
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
            count_ducts(area, len, e);
        }
        else if(e->is_fitting())
        {
            count_fittings(area, len, e);
        }
        else if(e->is_device())
        {
            check_for_air_handling_unit(e);
        }
    }
}

void SummaryCreator::count_ducts(double area, int length, const std::unique_ptr<VentElem> &e)
{
    _ducts_area += area;
    _ducts_length += length;
    if(e->check_type(VentElem::IS_CIRCULAR))
    {
        _circ_ducts_area += area;
        _circ_ducts_length += length;
    }
}

void SummaryCreator::count_fittings(double area, int length, const std::unique_ptr<VentElem> &e)
{
    _fittings_area += area;
    _fittings_length += length;
    if(e->check_type(VentElem::IS_CIRCULAR))
    {
        _circ_fittings_area += area;
        _circ_fittings_length += length;
    }
}

void SummaryCreator::check_for_air_handling_unit(const std::unique_ptr<VentElem> &elem)
{
    auto name = elem->get_name().toLower();
    auto type = elem->get_system_type();
    if(name.indexOf(_suppExhUnit) >= 0)
    {
        if(type == VentElem::AirflowSystemType::SUPP_EXHAUST)
            _devices[_suppExhUnit +" nawiewno-wywiewna"]++;
        else if(type == VentElem::AirflowSystemType::SUPPLY)
            _devices[_suppExhUnit +" nawiewna"]++;
        else
            _devices[_suppExhUnit]++;
    }
    else if(name.indexOf(_ventilator) >= 0)
    {
        if(type == VentElem::AirflowSystemType::SUPPLY)
            _devices[_ventilator +" nawiewny"]++;
        else if(type == VentElem::AirflowSystemType::EXHAUST)
            _devices[_ventilator +" wywiewny"]++;
        else
            _devices[_ventilator]++;
    }
}
