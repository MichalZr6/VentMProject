#include "qframedisplayengine.h"

QFrameDisplayEngine::QFrameDisplayEngine(QFrame *parent)
    : QFrame(parent)
{
    _gdlay = new QGridLayout();
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

    _gdlay->addWidget(new QLabel(header), _curr_row, _curr_col++, Qt::AlignCenter);
}

void QFrameDisplayEngine::add_labels(const std::vector<QString> &labels)
{
    for(auto const &lab : labels)
        add_label(lab);
}

void QFrameDisplayEngine::add_label(const QString &label)
{
    _gdlay->addWidget(new QLabel(label), _curr_row, _curr_col++);
}

void QFrameDisplayEngine::add_priceLineEdit()
{
    _priceLE = new QLineEdit();
    _priceLE->setFixedWidth(_fixedPriceLE_width);
    _gdlay->addWidget(_priceLE);
    _curr_col++;
}

void QFrameDisplayEngine::add_area_or_len_ComboBox()
{
    _areaORlenCB = new QComboBox();
    _areaORlenCB->setFixedWidth(_fixedAreaOrLenCB_width);
    _areaORlenCB->addItems({_areaCB, _lenCB});
    _gdlay->addWidget(_areaORlenCB);
    _curr_col++;
}

void QFrameDisplayEngine::add_result_label()
{
    if(_areaORlenCB->currentText() == _areaCB)
        add_label("123");   //result of price * area or price * length based on chosen option
    else
        add_label("345");
}

QFrame *QFrameDisplayEngine::get_frame()
{
    this->setLayout(_gdlay);
    return this;
}
