#ifndef QFRAMEDISPLAYENGINE_H
#define QFRAMEDISPLAYENGINE_H

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>

#include <vector>

class QFrameDisplayEngine : public QFrame
{
    Q_OBJECT
public:
    QFrameDisplayEngine(QFrame *parent = 0);

    void add_headers(std::vector<QString> &&headers);
    void add_header(const QString &header);
    void add_labels(const std::vector<QString> &labels);
    void add_label(const QString &label);
    void add_priceLineEdit();
    void add_area_or_len_ComboBox();
    void add_result_label();

    QFrame *get_frame();

    inline void next_row()
    {
        _curr_row++;
        _curr_col = 0;
    }

private:
    const QString _areaCB = "zł/m²";
    const QString _lenCB = "zł/mb";
    const int _fixedPriceLE_width = 50;
    const int _fixedAreaOrLenCB_width = 55;
    std::vector<QString> _headers;
    QGridLayout *_gdlay;
    QLineEdit *_priceLE;
    QComboBox *_areaORlenCB;

    int _curr_row {0};
    int _curr_col {0};
};

#endif // QFRAMEDISPLAYENGINE_H
