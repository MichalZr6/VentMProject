#ifndef VENTSYSTEMSCONTAINER_H
#define VENTSYSTEMSCONTAINER_H

#include "ventsystem.h"
#include <unordered_map>

class VentSystemsContainer
{
public:
    VentSystemsContainer();

    void add(const VentSystem &vent_system);
    void add(VentElem *ve);
    void set_insulation(const std::vector<QString> &systems, VentInsulation *insulation);


private:
    std::unordered_map<QString, VentSystem> _container;

};

#endif // VENTSYSTEMSCONTAINER_H
