#ifndef VENTSYSTEM_H
#define VENTSYSTEM_H

#include <memory>
#include <vector>
#include <ventelem.h>

class VentSystem
{
public:
    VentSystem() {};

    void add(VentElem *ve);
    void set_sys_name(QStringView sys_name);
    void set_insulation(VentInsulation *insulation);

    QString get_sys_name() const;

private:
    QString _systemName {};
    std::vector<VentElem *> _elems;
    int _fittings {0};
    int _devices {0};
    int _ducts {0};
};

#endif // VENTSYSTEM_H
