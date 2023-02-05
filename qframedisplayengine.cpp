#include "qframedisplayengine.h"

const char *const QFrameDisplayEngine::frame_names[] = { "ducts", "circ_ducts", "rect_ducts",
                                                         "fittings", "circ_fittings", "rect_fittings",
                                                         "devices", "others" };

QString QFrameDisplayEngine::get_frame_name(GroupType t)
{
    return QString::fromStdString(frame_names[static_cast<int>(t)/10-1]);
}

QFrameDisplayEngine::QFrameDisplayEngine(std::shared_ptr<SummaryCreator> sc, QWidget *parent)
    : QWidget(parent), _sc(sc)
{

}

QLabel *QFrameDisplayEngine::make_frame_header(const QString &header)
{
    Q_ASSERT(!header.isEmpty());

    QLabel *label = new QLabel();
    label->setText(header);
    label->setStyleSheet(_headerStyle);
    label->setFixedHeight(_fixedHeaderHeight);

    return label;
}

void QFrameDisplayEngine::add_headers(std::vector<QString> &&headers)
{
    _headers = std::move(headers);
    for(auto const &h : _headers)
    {
        add_header(h);
    }
}

void QFrameDisplayEngine::add_header(const QString &header)
{
    Q_ASSERT(_gdlay && _curr_row == 0);

    auto lab = new QLabel(header);
    lab->setAlignment(Qt::AlignCenter);
    if(header.isEmpty())
        lab->setStyleSheet("border: 0;");
    _gdlay->addWidget(lab, _curr_row, _curr_col++);
}

void QFrameDisplayEngine::add_labels(const std::vector<QString> &labels)
{
    Q_ASSERT(!labels.empty());
    for(auto const &lab : labels)
        add_label(lab);
}

void QFrameDisplayEngine::add_value_label(GroupType gt, ValueType vt, const QString &name)
{
    add_label(QStrVal(_sc->get(gt, vt), vt), name);
}

void QFrameDisplayEngine::add_label(const QString &label, const QString &name)
{
    auto lab = new QLabel(label);
    lab->setObjectName(name);
    _gdlay->addWidget(lab, _curr_row, _curr_col++);
}

template<typename T>
void QFrameDisplayEngine::add_value_label(T value, ValueType vt, const QString &name)
{
    add_label(QStrVal(value, vt), name);
}

void QFrameDisplayEngine::add_priceLineEdit()
{
    _priceLE = new QLineEdit();
    _priceLE->setFixedWidth(_fixedPriceLE_width);
    _priceLE->setObjectName(prepare_widget_name(_priceLE_name));
    connect(_priceLE, &QLineEdit::textChanged, this, &QFrameDisplayEngine::set_result_valueLE);
    _gdlay->addWidget(_priceLE, _curr_row, _curr_col++);
}

void QFrameDisplayEngine::add_area_or_len_ComboBox()
{
    _areaORlenCB = new QComboBox();
    _areaORlenCB->setFixedWidth(_fixedAreaOrLenCB_width);
    _areaORlenCB->addItems({_areaCB, _lenCB});
    _areaORlenCB->setObjectName(prepare_widget_name(_area_or_len_CB_name));
    connect(_areaORlenCB, &QComboBox::activated, this, &QFrameDisplayEngine::set_result_valueCB);
    _gdlay->addWidget(_areaORlenCB);
    _curr_col++;
}

QFrame *QFrameDisplayEngine::ducts_frame()
{
    return _frames.at(GroupType::DUCTS);
}

QFrame *QFrameDisplayEngine::fittings_frame()
{
    return _frames.at(GroupType::FITTINGS);
}

QFrame *QFrameDisplayEngine::devices_frame()
{
    return _frames.at(GroupType::DEVICES);
}

void QFrameDisplayEngine::set_ducts_frame()
{
    prepare_frame(GroupType::DUCTS);

    add_label("okrągłe:");
    add_value_label(GroupType::CIRC_DUCTS, ValueType::LENGTH);
    add_value_label(GroupType::CIRC_DUCTS, ValueType::AREA);
    add_value_label(GroupType::CIRC_DUCTS, ValueType::QUANTITY, _quant_label_name+"-cd");
    add_priceLineEdit();
    add_area_or_len_ComboBox();
    add_label("", _result_label_name+"-cd");
    next_row();

    add_label("prostokątne:");
    add_value_label(GroupType::RECT_DUCTS, ValueType::LENGTH);
    add_value_label(GroupType::RECT_DUCTS, ValueType::AREA);
    add_value_label(GroupType::RECT_DUCTS, ValueType::QUANTITY, _quant_label_name+"-rd");
    add_priceLineEdit();
    add_area_or_len_ComboBox();
    add_label("", _result_label_name+"-rd");
    next_row();

    add_label("RAZEM:");
    add_value_label(GroupType::DUCTS, ValueType::LENGTH);
    add_value_label(GroupType::DUCTS, ValueType::AREA);
    add_value_label(GroupType::DUCTS, ValueType::QUANTITY);

    finish_frame(GroupType::DUCTS);
}

void QFrameDisplayEngine::set_fittings_frame()
{
    prepare_frame(GroupType::FITTINGS);
    add_label("okrągłe:");
    add_value_label(GroupType::CIRC_FITT, ValueType::LENGTH);
    add_value_label(GroupType::CIRC_FITT, ValueType::AREA);
    add_value_label(GroupType::CIRC_FITT, ValueType::QUANTITY, _quant_label_name+"-cf");
    add_priceLineEdit();
    add_area_or_len_ComboBox();
    add_label("", _result_label_name+"-cf");
    next_row();

    add_label("prostokątne:");
    add_value_label(GroupType::RECT_FITT, ValueType::LENGTH);
    add_value_label(GroupType::RECT_FITT, ValueType::AREA);
    add_value_label(GroupType::RECT_FITT, ValueType::QUANTITY, _quant_label_name+"-rf");
    add_priceLineEdit();
    add_area_or_len_ComboBox();
    add_label("", _result_label_name+"-rf");
    next_row();

    add_label("RAZEM:");
    add_value_label(GroupType::FITTINGS, ValueType::LENGTH);
    add_value_label(GroupType::FITTINGS, ValueType::AREA);
    add_value_label(GroupType::FITTINGS, ValueType::QUANTITY);

    finish_frame(GroupType::FITTINGS);
}

void QFrameDisplayEngine::set_devices_frame()
{
    prepare_frame(GroupType::DEVICES);

    for(auto d : _sc->devices())
    {
        add_labels({d->get_system_name() + d->get_index(), d->get_name()});
        add_value_label(d->get_quantity(), ValueType::QUANTITY, _quant_label_name+"-dev");
        add_priceLineEdit();
        add_label("", _result_label_name+"-dev");
        next_row();
    }
    add_label("RAZEM:");
    add_label("");
    add_value_label(GroupType::DEVICES, ValueType::QUANTITY);

    finish_frame(GroupType::DEVICES);
}

void QFrameDisplayEngine::set_result_valueCB(int cb_option)
{
    QFrame *fr = nullptr;
    int row = -1;
    find_senders_frame_and_row(fr, row);
    Q_ASSERT(fr != nullptr && row >= 0);
    auto grid = static_cast<QGridLayout *>(fr->layout());
    set_result_label_text(grid, row, cb_option);
    set_total_value_label(grid);
}

void QFrameDisplayEngine::set_result_valueLE()
{
    QFrame *fr = nullptr;
    int row = -1;
    find_senders_frame_and_row(fr, row);
    Q_ASSERT(fr != nullptr && row >= 0);
    auto grid = static_cast<QGridLayout *>(fr->layout());
    set_result_label_text(grid, row);
    set_total_value_label(grid);
}

void QFrameDisplayEngine::find_senders_frame_and_row(QFrame *&frame, int &row)
{
    auto s = this->sender()->objectName().toStdString();
    for(auto f : _frames)
    {
        auto pos = s.find(f.second->objectName().toStdString());   // find sender's frame
        if(pos != std::string::npos)
        {
            for(auto i = pos+f.second->objectName().size(); i < s.size(); ++i)
            {
                if(s[i] >= 48 && s[i] <= 57)
                {
                    row = s[i]-'0';
                    break;
                }
            }
            frame = f.second;
            break;
        }
    }
}

QWidget *QFrameDisplayEngine::get_item(QGridLayout *grid, int row, const QString &has_text)
{
    int cols = grid->columnCount();
    QLayoutItem *item;
    for(int i = 0; i < cols; ++i)
    {
        item = grid->itemAtPosition(row, i);
        if(item && item->widget()->objectName().indexOf(has_text) >= 0)    // first check is for empty cols
            return item->widget();
    }
    return nullptr;
}

QString QFrameDisplayEngine::prepare_widget_name(const QString &name)
{
    return name +"-"+ _frame->objectName() + QString::number(_curr_row) + QString::number(_curr_col);
}

void QFrameDisplayEngine::set_result_label_text(QGridLayout *grid, int row, int cb_option)
{
    auto res_lab = static_cast<QLabel *>(get_item(grid, row, _result_label_name));

    if(res_lab->objectName().indexOf(_result_label_name) >= 0)
    {
        auto price_LE = static_cast<QLineEdit *>(get_item(grid, row, _priceLE_name));
        auto price = price_LE->text().toDouble();
        if(price < 0 || price > DBL_MAX)
        {
            res_lab->setText("0");
            return ;
        }
        if(cb_option == 0)
        {
            auto len = static_cast<QLabel *>(grid->itemAtPosition(row, 2)->widget())->text();
            res_lab->setText(QStrVal(price * QStrVal::to_dbl(len), ValueType::PRICE));
        }
        else
        {
            auto ar = static_cast<QLabel *>(grid->itemAtPosition(row, 1)->widget())->text();
            res_lab->setText(QStrVal(price * QStrVal::to_dbl(ar), ValueType::PRICE));
        }
    }
}

void QFrameDisplayEngine::set_result_label_text(QGridLayout *grid, int row)
{
    auto res_lab = static_cast<QLabel *>(get_item(grid, row, _result_label_name));
    if(!res_lab)
        return ;
    if(res_lab->objectName().indexOf(_result_label_name) >= 0)
    {
        auto price_LE = static_cast<QLineEdit *>(get_item(grid, row, _priceLE_name));
        auto price = price_LE->text().toDouble();
        if(price <= 0 || price > DBL_MAX)
        {
            res_lab->setText("0");
            return ;
        }

        auto quant = static_cast<QLabel *>(get_item(grid, row, _quant_label_name))->text();
        res_lab->setText(QStrVal(price * QStrVal::to_dbl(quant), ValueType::PRICE));
    }
}

void QFrameDisplayEngine::set_total_value_label(QGridLayout *grid)
{
    auto cols = grid->columnCount();
    auto rows = grid->rowCount();
    QLayoutItem *item;
    double total = 0;
    for(int row = 0; row < rows; ++row)
    {
        item = grid->itemAtPosition(row, cols-1);
        if(item && item->widget()->objectName().contains(_result_label_name))
        {
            total += QStrVal::to_dbl(qobject_cast<QLabel *>(item->widget())->text());
        }
    }
    item = grid->itemAtPosition(rows-1, cols-1);
    auto total_str = QStrVal(total, ValueType::PRICE);
    if(item)
        qobject_cast<QLabel *>(item->widget())->setText(total_str);
    else
        grid->addWidget(new QLabel(total_str), rows-1, cols-1);
}

void QFrameDisplayEngine::prepare_frame(GroupType t)
{
    _frame = new QFrame();
    _gdlay = new QGridLayout();
    _frame->setFixedWidth(_frameFixedWidth);
    _frame->setStyleSheet(_dataStyle);
    _frame->setObjectName(get_frame_name(t));
    if(t == GroupType::DUCTS || t == GroupType::FITTINGS)
    {
        add_headers({"nazwa", "długość", "pole", "ilość", "cena", "", "wartość"});
        next_row();
    }
    else if(t == GroupType::DEVICES)
    {
        add_headers({"system", "nazwa", "ilość", "cena", "wartość"});
        next_row();
    }
}

void QFrameDisplayEngine::finish_frame(GroupType t)
{
    _frame->setLayout(_gdlay);
    _frames[t] = std::move(_frame);
    _curr_row = 0;
    _curr_col = 0;
}
