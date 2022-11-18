#ifndef VENTINSULATION_H
#define VENTINSULATION_H

class VentInsulation
{
public:
    enum class Type {WOOL_GLUE, WOOL, WOOL_PINS, ARMACELL_BLACK};

    VentInsulation() = delete;
    VentInsulation(VentInsulation::Type type, int thickness)
        : _type(type), _thickness(thickness) {};

    void set_type(VentInsulation::Type type);
    void set_thickness(int thickness);

private:
    Type _type = Type::WOOL_GLUE;
    int _thickness;

};

#endif // VENTINSULATION_H
