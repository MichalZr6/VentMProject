#ifndef VENTELEM_H
#define VENTELEM_H

#include <QString>
#include <QFlags>
#include <QVariant>

#include <string>
#include <memory>
#include <unordered_map>

#include "ventinsulation.h"

class VentElem
{    
public:
    // not using proper powers of 2 will make QFlags work bad
    enum ElemProperty { IS_FITTING = 1 << 1,
                        IS_CIRCULAR = 1 << 2,
                        IS_FINAL = 1 << 3,
                        IS_THROTTLE = 1 << 4,
                        IS_DEVICE = 1 << 5,
                        IS_INSULABLE = 1 << 6,
                        IS_DUCTAL = 1 << 7};

    Q_DECLARE_FLAGS(ElemProperties, ElemProperty)
    enum class AirflowSystemType { SUPPLY, FRESH, EXHAUST, INDOOR_EXTRACT, SUPP_EXHAUST, UNKNOWN };

    VentElem() = default;
    VentElem(std::vector<QVariant> &&init_values);
    VentElem(QString &&init_string_value);


    void set_name(const QString &name);
    void set_system(const QString &system);
    void set_airflow_sys_type(AirflowSystemType type);
    void set_number(int number);
    void set_quantity(int quantity);
    void set_VPelem_type(const QString &type);
    void set_material(const QString &material);
    void set_area(double area);
    void set_total_area(double area);
    void set_manufacturer(const QString &manufacturer);
    void set_comments(const QString &comments);
    void set_price(double price);
    void set_properties();
    void set_dimension(const QString &dim, const QVariant &val);
    void set_init_values(QString &&val);
    void set_init_values(std::vector<QVariant> &&values);
    void set_proper_length();

    AirflowSystemType get_system_type() const
    { return _airflowtype; }
    uint16_t get_index() const
    { return _index; }
    double get_area() const
    { return _area; }
    double get_length() const
    { return _length / 1000.0; }
    QString get_name() const
    { return _name; }
    double get_total_area() const
    { return _total_area; }
    double get_total_length() const
    { return _length * _quantity / 1000.0; }
    std::vector<QVariant> get_initial_values() const;

    bool is_ductal() const;
    bool is_fitting() const;
    bool is_device() const;
    bool is_empty() const;
    bool is_valid() const;
    bool is_final() const;
    bool check_type(ElemProperty t) const;

    QString get_errors() const
    { return _err_msg; }

protected:
    QString _name;
    QString _system;
    QString _VPtype;       // VentPack style type
    QString _material;
    QString _manufacturer;
    QString _comments;

    std::unordered_map<std::string, int> _dimensions;

    VentElem::ElemProperties _properties;

    bool _is_valid = true;

    double _price = 0;
    double _area = 0;     // in m2
    double _total_area = 0; // from Ventpack listing
    int _length = 0;    // in [mm]
    uint16_t _quantity = 0;
    uint16_t _index = 0;
    AirflowSystemType _airflowtype = AirflowSystemType::UNKNOWN;
    std::vector<QVariant> _initial_values;

    std::unique_ptr<VentInsulation> _insulation = nullptr;

    bool name_contains(const QString &word) const;
    bool name_contains_OR(const std::vector<QString> &words) const;
    bool has_dimension(std::string dim, int occurences = 1) const;
    int parse_dim_value(const QVariant &value) const;

    QString _err_msg;
    void set_error(const QString &msg);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(VentElem::ElemProperties);

#endif // VENTELEM_H
