#include "ventduckt.h"

VentDuckt::VentDuckt()
{

}

VentDuckt::VentDuckt(int a, int b, int length, std::unique_ptr<VentInsulation> insulation)
{
    VentElem::_area = (2 * a + 2 * b) * length;
    _insulation = std::move(insulation);
}
