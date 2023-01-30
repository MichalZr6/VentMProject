#include "qstrval.h"

std::array<QString, 5> QStrVal::units =
{
    "m²", // AREA
    "mb", // LENGTH
    "szt.", // QUANTITY
    "zł", // PRICE
    ""  // NONE
};

QStrVal::QStrVal(double value, ValueType t)
    : _dblval(value),
      _vt(t)
{

}
