#ifndef FIXINGSFASTENERS_H
#define FIXINGSFASTENERS_H

#include "mountingelements.h"
#include "slings.h"

class FixingsFasteners
{
public:
    FixingsFasteners();

private:
    std::shared_ptr<MountingElements> _mntelems;

};

#endif // FIXINGSFASTENERS_H
