#include "ventsystemscontainer.h"

VentSystemsContainer::VentSystemsContainer()
{

}

void VentSystemsContainer::add(const VentSystem &vent_system)
{
    _container[vent_system.get_sys_name()] = vent_system;
}

void VentSystemsContainer::add(VentElem *ve)
{
    auto ve_sys = ve->get_system_name();
    if(_container.find(ve_sys) != _container.end())
    {
        _container[ve_sys].add(ve);
    }
}

void VentSystemsContainer::set_insulation(const std::vector<QString> &systems, VentInsulation *insulation)
{
    for(auto const &s : systems)
    {
        _container.at(s).set_insulation(insulation);
    }
}
