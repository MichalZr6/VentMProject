#include "ventsystem.h"

void VentSystem::add(VentElem *ve)
{
    if(_systemName.isEmpty())
        _systemName = ve->get_system_name();

    if(ve->get_system_name() == _systemName)
    {
        _elems.push_back(ve);
    }
}

void VentSystem::set_insulation(VentInsulation *insulation)
{
    for(auto e : _elems)
    {
        e->set_insulation(insulation);
    }
}

QString VentSystem::get_sys_name() const
{
    return _systemName;
}
