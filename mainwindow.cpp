#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <filesystem>
#include <chrono>
#include <cassert>

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
    auto outputfile = std::make_unique<VMPOutputFileMan>("Zestawienie zbiorcze.xlsx");

    auto input_file_headers = _vefactory->workbook_headers();

    outputfile->set_headers(std::move(input_file_headers));

    outputfile->write_elements(_vefactory->elements());

    if(outputfile->hasErrors())
        show_errors("Generowanie pliku wynikowego zakończyło się błędami", outputfile->getErrors());

    _outputfile = std::move(outputfile);
}

void MainWindow::create_summary(const std::unique_ptr<VMPOutputFileMan> &ofile)
{
    _sm = std::make_shared<SummaryCreator>(ofile, _vefactory->elements());

    _innerframe = std::make_unique<QFrameDisplayEngine>(_sm);
    make_headers_labels();
    print_ducts_summary();
    print_fittings_summary();
    print_devices_summary();
    add_buttons_bottom();

    ui->gridLayout->setRowStretch(5, 1);

    // create summary sheet in ofile
}

void MainWindow::print_ducts_summary()
{
    _innerframe->set_ducts_frame();

    ui->gridLayout->addWidget(_innerframe->ducts_frame(), 1, 0, Qt::AlignTop);
}

void MainWindow::print_fittings_summary()
{
    _innerframe->set_fittings_frame();

    ui->gridLayout->addWidget(_innerframe->fittings_frame(), 3, 0, Qt::AlignTop);
}

void MainWindow::print_devices_summary()
{
    _innerframe->set_devices_frame();

    ui->gridLayout->addWidget(_innerframe->devices_frame(), 5, 0, Qt::AlignTop);
}

void MainWindow::make_headers_labels()
{
    QLabel *ducts_header = _innerframe->make_frame_header("Kanały");
    QLabel *fittings_header = _innerframe->make_frame_header("Kształtki");
    QLabel *devices_header = _innerframe->make_frame_header("Urządzenia");
    QLabel *others_header = _innerframe->make_frame_header("Inne");

    auto gdlay = ui->gridLayout;

    gdlay->addWidget(ducts_header, 0, 0, Qt::AlignHCenter|Qt::AlignTop);
    gdlay->addWidget(fittings_header, 2, 0, Qt::AlignHCenter|Qt::AlignTop);
    gdlay->addWidget(devices_header, 4, 0, Qt::AlignHCenter|Qt::AlignTop);
    gdlay->addWidget(others_header, 0, 3, Qt::AlignHCenter|Qt::AlignTop);
}

void MainWindow::add_buttons_bottom()
{
    /* uncomment after inner_frame is set properly...
    QPushButton *pb_insulation_menu = new QPushButton("Izolacja");
    QPushButton *pb_work_costs_menu = new QPushButton("Robocizna");
    ui->gridLayout->addWidget(pb_insulation_menu);
    ui->gridLayout->addWidget(pb_work_costs_menu);
    */

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

