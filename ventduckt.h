#ifndef VENTDUCKT_H
#define VENTDUCKT_H

#include "ventelem.h"

class VentDuckt : public VentElem
{
public:
    VentDuckt();
    VentDuckt(int a, int b, int length, std::unique_ptr<VentInsulation> insulation = nullptr);

};

#endif // VENTDUCKT_H
