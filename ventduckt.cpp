#include "ventduckt.h"

VentDuckt::VentDuckt()
{

}

VentDuckt::VentDuckt(int a, int b, int length, VentInsulation *insulation)
{
    VentElem::_area = (2 * a + 2 * b) * length;
    _insulation = insulation;
}
