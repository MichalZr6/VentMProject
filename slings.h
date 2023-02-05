#ifndef SLINGS_H
#define SLINGS_H

#include <map>
#include <memory>

#include "mountingelements.h"

class Slings
{
public:
    Slings();

private:

    std::map<int, int> _circ_brackets;    // diameter / quantity - maybe a struct ?
    int _rails_length;
    std::shared_ptr<MountingElements> _mntelems;
};

#endif // SLINGS_H
