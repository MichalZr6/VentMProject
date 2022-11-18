#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <filesystem>
#include <chrono>
#include <cassert>

namespace outputf_sheet_names {
    const QString ducts = "Kanały";
    const QString fittings = "Kształtki";
    const QString devices = "Urządzenia";
    const QString other = "Inne";
    const QString summary = "Podsumowanie";

    const int headers_starting_row = 3;
    const int offset_rows = 1;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    auto xls_input_file = std::filesystem::u8path("../VentMProject/Zestawienie_materiałów.xlsx");

    auto VEfactory = create_VentElemFactory(xls_input_file, VentElemFileType::VENTPACK);

    assert(VEfactory != nullptr);
    VEfactory->load_elements_from_file();

    ui->setupUi(this);

    auto err_msg = VEfactory->get_errors();
    if(!err_msg.isEmpty())
        show_error(err_msg);
    else
    {
        _vefactory = std::move(VEfactory);
        create_output_listing_file();

        create_summary(_outputfile);

        _outputfile->save();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

std::unique_ptr<VentElemFactory> MainWindow::create_VentElemFactory(const std::filesystem::path &listing_file,
                                                                    VentElemFileType type)
{

    if(type == VentElemFileType::VENTPACK)
        return std::make_unique<VentPackElemFactory>(listing_file, type);
    //else if ...
    else
        return nullptr;     // std::make_unique<SomeUserXLSelemFactory> or someUserTXTelemFactory or Wentyle

}

void MainWindow::create_output_listing_file()
{
    using namespace outputf_sheet_names;

    auto outputf = std::make_unique<xls::XlsxFileManager>("Zestawienie zbiorcze.xlsx");
    outputf->add_sheets({ducts, fittings, devices, other, summary});
    auto VEfactory_headers = _vefactory->workbook_headers().begin()->headers();
    std::set<QString> sheets = {ducts, fittings, devices, other};
    xls::WorkBookHeaders ofile_hds(std::move(sheets), std::move(VEfactory_headers));
    ofile_hds.set_row(headers_starting_row);
    outputf->write(ofile_hds);

    // int name_col = ofile_hds.find(vph::name)->col();
    int first_col = ofile_hds.find(vph::system)->col();

    auto elems = _vefactory->elements();
    QString sheet_name;
    std::map<QString, int> sheet_curr_row =
        {{ ducts, headers_starting_row + 1 +offset_rows},
        { fittings, headers_starting_row + 1 + offset_rows},
        { devices, headers_starting_row + 1 + offset_rows},
        { other, headers_starting_row + 1 + offset_rows},
        { summary, 2}};

    for(auto it = elems->begin(); it != elems->end(); ++it)
    {
        if(it->get()->check_type(VentElem::IS_FITTING))
            sheet_name = fittings;
        else if(it->get()->check_type(VentElem::IS_DUCTAL))
            sheet_name = ducts;
        else if(it->get()->check_type(VentElem::IS_DEVICE))
            sheet_name = devices;
        else
            sheet_name = other;

        //qDebug() << "element: " << it->get()->get_name();

        outputf->set_curr_sheet(sheet_name);
        outputf->write(it->get()->get_initial_values(),
                       {sheet_curr_row[sheet_name]++, first_col});     // {start_row, start_col}
    }

    if(outputf->hasErrors())
        show_errors("Generowanie pliku wynikowego zakończyło się błędami", outputf->getErrors());

    _outputfile = std::move(outputf);
}

void MainWindow::create_summary(const std::unique_ptr<xls::XlsxFileManager> &ofile)
{
    _sm = std::make_unique<SummaryCreator>(ofile, _vefactory->elements());

    make_headers_labels();
    print_ducts_summary();
    print_fittings_summary();
    print_devices_summary();

    ui->gridLayout->setRowStretch(3, 1);

    // create summary sheet in ofile
}

void MainWindow::print_ducts_summary(const std::vector<QString> &system)    // if there are systems on elements -> make checkboxes on top of the form,
                                                                            // which will filter summaries by system
{
    // ducts:
    _innerframe = std::make_unique<QFrameDisplayEngine>(new QFrame);
    _innerframe->setStyleSheet(_dataStyle);
    _innerframe->setFixedWidth(400);

    _innerframe->add_headers({"nazwa", "długość", "pole", "cena", "", "wartość"});
    _innerframe->next_row();
    _innerframe->add_labels({"okrągłe:",
                             QString::number(_sm->get_circ_ducts_length())+" mb",
                             QString::number(_sm->get_circ_ducts_area())+" m²",
                            });
    _innerframe->add_priceLineEdit();
    _innerframe->add_area_or_len_ComboBox();
    _innerframe->add_result_label();


    _innerframe->next_row();
    // ...

//    inner_grid->addWidget(new QLabel("prostokątne:"), 2, 0);
//    inner_grid->addWidget(new QLabel(QString::number(123/*_sm->get_rect_ducts_len()*/)
//                                     +" mb"), 2, 1);
//    inner_grid->addWidget(new QLabel(QString::number(_sm->get_rect_ducts_area())
//                                     +" m²"), 2, 2);

//    inner_grid->addWidget(new QLabel("RAZEM:"), 3, 0);
//    inner_grid->addWidget(new QLabel(QString::number(123/*_sm->get_ducts_len()*/)
//                                     +" mb"), 3, 1);
//    inner_grid->addWidget(new QLabel(QString::number(_sm->get_ducts_area())
//                                     +" m²"), 3, 2);


    auto gdlay = ui->gridLayout;
    gdlay->addWidget(_innerframe->get_frame(), 1, 0, Qt::AlignTop);
}

void MainWindow::print_fittings_summary()
{
    // fittings:
    auto frame = new QFrame;
    frame->setStyleSheet(_dataStyle);
    auto inner_grid = new QGridLayout;
    inner_grid->addWidget(new QLabel("okrągłe:"), 0, 0);
    inner_grid->addWidget(new QLabel(QString::number(_sm->get_circ_fittings_area())
                                     +" m²"), 0, 1);
    inner_grid->addWidget(new QLabel("prostokątne:"), 1, 0);
    inner_grid->addWidget(new QLabel(QString::number(_sm->get_rect_fittings_area())
                                     +" m²"), 1, 1);
    inner_grid->addWidget(new QLabel("RAZEM:"), 2, 0);
    inner_grid->addWidget(new QLabel(QString::number(_sm->get_fittings_area())
                                     +" m²"), 2, 1);
    frame->setLayout(inner_grid);
    auto gdlay = ui->gridLayout;
    gdlay->addWidget(frame, 3, 0, Qt::AlignTop);
}

void MainWindow::print_devices_summary()
{
    // devices:
    auto frame = new QFrame;
    frame->setStyleSheet(_dataStyle);
    auto inner_grid = new QGridLayout;
    int sum = 0, row = 0;

    for(auto it = _sm->devices().begin(); it != _sm->devices().end(); ++it)
    {
        inner_grid->addWidget(new QLabel(it->first), row, 0);
        inner_grid->addWidget(new QLabel(QString::number(it->second)
                                         +" szt."), row, 1);
        sum += it->second;
        row++;
    }
    inner_grid->addWidget(new QLabel("RAZEM:"), row, 0);
    inner_grid->addWidget(new QLabel(QString::number(sum)
                                     +" szt."), row, 1);
    frame->setLayout(inner_grid);
    auto gdlay = ui->gridLayout;
    gdlay->addWidget(frame, 1, 2, Qt::AlignTop);
}

void MainWindow::make_headers_labels()
{
    QLabel *ducts_header = new QLabel();
    QLabel *fittings_header = new QLabel();
    QLabel *devices_header = new QLabel();
    QLabel *others_header = new QLabel();
    ducts_header->setText("Kanały");
    fittings_header->setText("Kształtki");
    devices_header->setText("Urządzenia");
    others_header->setText("Inne");

    ducts_header->setStyleSheet(_headerStyle);
    fittings_header->setStyleSheet(_headerStyle);
    devices_header->setStyleSheet(_headerStyle);
    others_header->setStyleSheet(_headerStyle);

    ducts_header->setFixedHeight(30);
    fittings_header->setFixedHeight(30);
    devices_header->setFixedHeight(30);
    others_header->setFixedHeight(30);

    auto gdlay = ui->gridLayout;

    gdlay->addWidget(ducts_header, 0, 0, Qt::AlignHCenter|Qt::AlignTop);
    gdlay->addWidget(fittings_header, 2, 0, Qt::AlignHCenter|Qt::AlignTop);
    gdlay->addWidget(devices_header, 0, 2, Qt::AlignHCenter|Qt::AlignTop);
    gdlay->addWidget(others_header, 0, 3, Qt::AlignHCenter|Qt::AlignTop);
}

void MainWindow::show_errors(const QString &title, const std::vector<QString> &errors)
{
    QMessageBox msgBox(parentWidget());
    msgBox.setText(title);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Ok);

    for(int i = 0; i < errors.size(); ++i)
    {
        msgBox.setInformativeText("Błąd nr: "+ QString::number(i+1) +" / "+ QString::number(errors.size()) +":\n"+
                                  errors[i]);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Close)
            break;
    }
}

void MainWindow::show_error(QString msg, bool stopApp)
{
    if(stopApp)
    {
        QMessageBox::critical(this, "Critical error", msg);
        ui->~MainWindow();
    }
    else
        QMessageBox::warning(this, "Warning", msg);
}

