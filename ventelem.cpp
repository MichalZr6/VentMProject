#include "ventelem.h"

VentElem::VentElem(std::vector<QVariant> &&init_values)
    : VentElem()
{
    set_init_values(std::move(init_values));
}

VentElem::VentElem(QString &&init_string_value)
    : VentElem()
{
    set_init_values(std::move(init_string_value));
}

void VentElem::set_name(const QString &name)
{
    if(name.isEmpty())
    {
        set_error(QStringLiteral("Nazwa nowego elementu jest pusta"));
        return;
    }
    _name = name;
}

void VentElem::set_system(const QString &system)
{
    _system = system;

    if(_airflowtype == AirflowSystemType::UNKNOWN)
    {
        QString sys = _system.toLower();
        if(sys.contains(QStringLiteral("cz")))
            _airflowtype = AirflowSystemType::FRESH;
        else if(sys.contains(QStringLiteral("wyrz")))
            _airflowtype = AirflowSystemType::EXHAUST;
        else if(sys.contains(QLatin1String("n")) && sys.contains(QLatin1String("w")))
            _airflowtype = AirflowSystemType::SUPP_EXHAUST;
        else if(sys.contains(QLatin1String("w")))
            _airflowtype = AirflowSystemType::INDOOR_EXTRACT;
        else if(sys.contains(QLatin1String("n")))
            _airflowtype = AirflowSystemType::SUPPLY;
    }
}

void VentElem::set_airflow_sys_type(AirflowSystemType type)
{
    _airflowtype = type;
}

void VentElem::set_number(int number)
{
    if(number < 0)
    {
        set_error(QStringLiteral("Numer w zestawieniu mniejszy od 0 ?"));
        return ;
    }

    _index = number;
}

void VentElem::set_quantity(int quantity)
{
    if(quantity < 0)
    {
        set_error(QStringLiteral("Ilość elementów mniejsza od 0 ?"));
        return ;
    }
    _quantity = quantity;
}

void VentElem::set_VPelem_type(const QString &type)
{
    _VPtype = type;
}

void VentElem::set_material(const QString &material)
{
    _material = material;
}

void VentElem::set_area(double area)
{
    _area = area;
}

void VentElem::set_total_area(double area)
{
    _total_area = area;
}

void VentElem::set_manufacturer(const QString &manufacturer)
{
    _manufacturer = manufacturer;
}

void VentElem::set_comments(const QString &comments)
{
    _comments = comments;
}

void VentElem::set_price(double price)
{
    if(price < 0)
    {
        set_error(QStringLiteral("Cena nie może być niższa niż zero."));
        return;
    }
    _price = price;
}

void VentElem::set_properties()
{
    if(_name.isEmpty())
        return ;

    bool has_d = has_dimension("d");
    bool has_2times_d = has_dimension("d", 2);

    if(name_contains_OR({QStringLiteral("przep"), QStringLiteral("klapa zwrotna")}))
    {
        _properties |= IS_INSULABLE | ElemProperty::IS_THROTTLE;
        if(has_d)
            _properties |= ElemProperty::IS_CIRCULAR;
    }
    else if(name_contains(QStringLiteral("rozprężna")))
    {
        _properties |= IS_INSULABLE | IS_FITTING;
        if(name_contains(QStringLiteral("kratka")) || name_contains(QStringLiteral("anemos")))
            _properties |= IS_FINAL;
    }
    else if(name_contains_OR({QStringLiteral("wyrz"), QStringLiteral("czer"), QStringLiteral("kratka"), QStringLiteral("anemos"),
                             QStringLiteral("podstawa dachowa"), QStringLiteral("cokół"), QStringLiteral("zawór")}))
    {
        _properties |= ElemProperty::IS_FINAL;
        if(has_d)
            _properties |= ElemProperty::IS_CIRCULAR;
    }
    else if(name_contains(QStringLiteral("tłum")))
    {
        _properties |= ElemProperty::IS_INSULABLE;
        if(has_d)
            _properties |= IS_CIRCULAR;
    }
    else if(name_contains_OR({QStringLiteral("kanał "), QStringLiteral("przewód ")}))
    {
        _properties |= ElemProperty::IS_DUCTAL | ElemProperty::IS_INSULABLE;
        if(has_d)
            _properties |= ElemProperty::IS_CIRCULAR;
    }
    else if(name_contains(QStringLiteral("zaślepka")))
    {
        _properties |= IS_FITTING | IS_INSULABLE;
        if(has_d)
            _properties |= ElemProperty::IS_CIRCULAR;
    }
    else if(has_dimension("alfa"))
    {
        _properties |= IS_FITTING | IS_INSULABLE;
        if(has_d)
            _properties |= IS_CIRCULAR;
    }
    else if(name_contains(QStringLiteral("króciec")))
    {
        if(has_d)
            _properties |= IS_CIRCULAR;
    }
    else if(name_contains_OR({QStringLiteral("trójnik"), QStringLiteral("redukcja"), QStringLiteral("odsadzka"), QStringLiteral("czwórnik")}))
    {
        _properties.setFlag(ElemProperty::IS_FITTING);
        _properties.setFlag(ElemProperty::IS_INSULABLE);
        if(has_2times_d)
            _properties |= IS_CIRCULAR;
    }
    else if(name_contains_OR({QStringLiteral("muf"), QStringLiteral("nypel"), QStringLiteral("nyplowa")}))
    {
        _properties |= VentElem::ElemProperty::IS_FITTING | VentElem::ElemProperty::IS_INSULABLE;
        if(has_d)
            _properties |= IS_CIRCULAR;
    }
    else if(name_contains(QStringLiteral("przejście koło/prost")))
    {
        _properties |= IS_FITTING | IS_INSULABLE;
    }
    else if(name_contains_OR({QStringLiteral("centrala"), QStringLiteral("wentylator"), QStringLiteral("nagrzewnica"),
                             QStringLiteral("kaseta"),
                             QStringLiteral("agregat"), QStringLiteral("chłodnica"), QStringLiteral("przeciwpoż")}))
    {
        _properties |= IS_DEVICE;
    }
    else
    {
        if((_system == "-" || _system.isEmpty()) && _index ==  0)
            return ;

        set_error("No properties found for: "+ _system +"-"+
                  _index +" "+ _name);
    }
}

void VentElem::set_dimension(const QString &dim, const QVariant &val)
{
    auto d = dim.toLower().toStdString();
    if(!val.isValid())
    {
        _dimensions[d] = 0;
        qDebug() << "dimension " << dim << " of element " << this->_name << " is invalid";
    }
    else
        _dimensions[d] = parse_dim_value(val);

    if(dim.startsWith('L', Qt::CaseInsensitive))
        _length = _dimensions[d];
}

void VentElem::set_init_values(QString &&val)
{
    _initial_values.push_back(std::move(val));
}

void VentElem::set_init_values(std::vector<QVariant> &&values)
{
    _initial_values = std::move(values);
}

void VentElem::set_proper_length()
{
    /*
    if(_system == "N10" && _index == 16)
        qDebug() << "";*/
    if(is_final() || is_device())
        _length = 0;
    else if(has_dimension("alfa"))
    {
        if(check_type(IS_CIRCULAR))
        {
            for(auto const &dim : _dimensions)
            {
                if(dim.first[0] == 'd')
                {
                    _length = 3.14*(1.5*dim.second*_dimensions["alfa"]/90.0)/4.0;
                    return;
                }
            }
        }
        else
        {
            int a = _dimensions["a"];
            int b = _dimensions["b"];
            _length = 3.14*(_dimensions["r"]+std::max(a, b)*_dimensions["alfa"]/90.0)/4.0;
        }
    }
}

void VentElem::set_insulation(VentInsulation *insulation)
{
    _insulation = insulation;
}

std::vector<QVariant> VentElem::get_initial_values() const
{
    return _initial_values;
}

bool VentElem::is_ductal() const
{
    return check_type(IS_DUCTAL);
}

bool VentElem::is_fitting() const
{
    return check_type(IS_FITTING);
}

bool VentElem::is_device() const
{
    return check_type(IS_DEVICE);
}

bool VentElem::is_final() const
{
    return check_type(IS_FINAL);
}

bool VentElem::is_empty() const
{
    return _name.isEmpty() && _system.isEmpty() && _quantity == 0 && _index == 0;
}

bool VentElem::is_valid() const
{
    if(_system.isEmpty() || _quantity <= 0 || !_is_valid)
        return false;

    return true;
}

bool VentElem::check_type(ElemProperty t) const
{
    return _properties.testFlag(t);
}

bool VentElem::name_contains(const QString &word) const
{
    return _name.contains(word, Qt::CaseInsensitive);
}

bool VentElem::name_contains_OR(const std::vector<QString> &words) const
{
    for(auto const &w : words)
    {
        if(name_contains(w))
            return true;
    }

    return false;
}

bool VentElem::has_dimension(std::string dim, int occurences) const
{
    int counter = 0;
    for(int i = 0; i < 6; ++i)
    {
        // check d d1 d2 d3 d4
        for(auto const &d : _dimensions)
        {
            if(d.first == dim + (i == 0 ? "" : std::to_string(i)))
                counter++;
        }
    }

    return counter == occurences;
}

int VentElem::parse_dim_value(const QVariant &value) const
{
    int res = value.value<int>();
    if(res == 0)
    {
        auto v_qs = value.value<QString>();
        v_qs.remove('m');
        v_qs.remove('.');
        res = v_qs.toInt();
        if(res != 0)
            return res*10;
        else
            return 0;
    }
    else
        return res;

}

void VentElem::set_error(const QString &msg)
{
    _err_msg += "\n"+ msg;
    _is_valid = false;
}
