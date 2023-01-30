#ifndef QSTRVAL_H
#define QSTRVAL_H

#include <QString>
#include <array>

enum class ValueType { AREA, LENGTH, QUANTITY, PRICE, NONE};

class QStrVal
{
public:
    static std::array<QString, 5> units;

    QStrVal() = default;
    QStrVal(double value, ValueType t = ValueType::NONE);

    static double to_dbl(const QString &val)
    {
        auto v = val;
        for(int i = 0; i < 5; ++i)
        {
            if(val.contains(units[i]))
                v.chop(units[i].size());
        }
        return v.simplified().toDouble();
    }

    virtual operator QString()
    {
        if(_vt == ValueType::NONE)
            return QSfromDbl(_dblval);
        else
            return QSfromDbl(_dblval) +" "+ units[static_cast<int>(_vt)];
    }

private:
    double _dblval {0};
    ValueType _vt { ValueType::NONE };

    inline QString QSfromInt(int num, int base = 10)
    {
        auto n = QString::number(num, base);
        insert_thou_sep(n);
        return n;
    }

    inline QString QSfromDbl(double num, int prec = 2)
    {
        auto n = QString::number(num, 'f', prec);
        if(num != 0)
            insert_thou_sep(n);
        return n;
    }

    inline void insert_thou_sep(QString &num)
    {
        std::reverse(num.begin(), num.end());
        int pos = num.indexOf('.');
        if(pos <= 0)
            pos = -1;
        int ctr = 0;
        for(int i = pos+1; i < num.size(); ++i)
        {
            if(ctr != 0 && ctr % 3 == 0)
                num.insert(i++, " ");

            ctr++;
        }
        std::reverse(num.begin(), num.end());
    }
};

#endif // QSTRVAL_H
