#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "qframedisplayengine.h"
#include "ventpackelemfactory.h"
#include "summarycreator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;



    std::unique_ptr<VentElemFactory> _vefactory;
    std::unique_ptr<VMPOutputFileMan> _outputfile;
    std::shared_ptr<SummaryCreator> _sm;
    std::unique_ptr<QFrameDisplayEngine> _innerframe;

    std::unique_ptr<VentElemFactory> create_VentElemFactory(const std::filesystem::path &listing_file,
                                                            VentElemFileType type);

    void create_output_listing_file();
    void create_summary(const std::unique_ptr<VMPOutputFileMan> &ofile);

    void print_ducts_summary();
    void print_fittings_summary();
    void print_devices_summary();
    void make_headers_labels();
    void add_buttons_bottom();

    void show_errors(const QString &title, const std::vector<QString> &errors);
    void show_error(QString msg, bool stopApp = false);
};
#endif // MAINWINDOW_H
