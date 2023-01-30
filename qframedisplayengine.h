#ifndef QFRAMEDISPLAYENGINE_H
#define QFRAMEDISPLAYENGINE_H

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>

#include <vector>

#include "summarycreator.h"
#include "qstrval.h"

class QFrameDisplayEngine : public QWidget
{
    Q_OBJECT
public:
    static const char *const frame_names[];
    static QString get_frame_name(GroupType t);

    QFrameDisplayEngine(std::shared_ptr<SummaryCreator>, QWidget *parent = nullptr);

    QLabel* make_frame_header(const QString &header);

    void add_headers(std::vector<QString> &&headers);
    void add_header(const QString &header);
    void add_labels(const std::vector<QString> &labels);
    void add_label(const QString &label, const QString &name = "");
    void add_value_label(GroupType gt = GroupType::UNDEFINED, ValueType vt = ValueType::NONE, const QString &name = "");
    template<typename T>
    void add_value_label(T value, ValueType vt = ValueType::NONE, const QString &name = "");

    void add_priceLineEdit();
    void add_area_or_len_ComboBox();

    QFrame *ducts_frame();
    QFrame *fittings_frame();
    QFrame *devices_frame();

    void set_ducts_frame();
    void set_fittings_frame();
    void set_devices_frame();

    inline void next_row()
    {
        _curr_row++;
        _curr_col = 0;
    }

public slots:
    void set_result_valueCB(int cb_option);
    void set_result_valueLE();

private:
    std::shared_ptr<SummaryCreator> _sc;

    const QString _dataStyle = "QLabel { border: 1px solid blue; padding: 2px; }";
    const QString _headerStyle = "QLabel { margin-top: 10px; font-weight: bold; }";
    const QString _areaCB = "zł/m²";
    const QString _lenCB = "zł/mb";
    const int _frameFixedWidth = 500;
    const int _fixedHeaderHeight = 30;
    const int _fixedPriceLE_width = 50;
    const int _fixedAreaOrLenCB_width = 55;

    const QString _result_label_name = "result_LAB";
    const QString _priceLE_name = "price_LE";
    const QString _area_or_len_CB_name = "area_or_len_CB";
    const QString _quant_label_name = "quant_LAB";

    std::vector<QString> _headers;
    std::vector<QString> *_selected_systems;
    std::unordered_map<GroupType, QFrame *> _frames;
    QFrame *_frame;
    QGridLayout *_gdlay;
    QLineEdit *_priceLE;
    QComboBox *_areaORlenCB;

    int _curr_row {0};
    int _curr_col {0};

    void find_senders_frame_and_row(QFrame *&, int &);
    QWidget * get_item(QGridLayout *, int row, const QString &has_text);
    QString prepare_widget_name(const QString &name);
    void set_result_label_text(QGridLayout *, int row, int cb_option);
    void set_result_label_text(QGridLayout *, int row);
    void set_total_value_label(QGridLayout *);

    void prepare_frame(GroupType t);
    void finish_frame(GroupType t);

    void set_style(QFrame *frame);

};

#endif // QFRAMEDISPLAYENGINE_H
