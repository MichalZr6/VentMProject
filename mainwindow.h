#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>

#include "qframedisplayengine.h"
#include "ventpackelemfactory.h"
#include "xlsxfilemanager.h"
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
    std::unique_ptr<xls::XlsxFileManager> _outputfile;
    std::unique_ptr<SummaryCreator> _sm;
    std::unique_ptr<QFrameDisplayEngine> _innerframe;
    const QString _dataStyle = "QLabel { border: 1px solid blue; padding: 2px; }";
    const QString _headerStyle = "QLabel { margin-top: 10px; font-weight: bold; }";

    std::unique_ptr<VentElemFactory> create_VentElemFactory(const std::filesystem::path &listing_file,
                                                            VentElemFileType type);

    void create_output_listing_file();
    void create_summary(const std::unique_ptr<xls::XlsxFileManager> &ofile);

    void print_ducts_summary();
    void print_fittings_summary();
    void print_devices_summary();
    void make_headers_labels();

    void show_errors(const QString &title, const std::vector<QString> &errors);
    void show_error(QString msg, bool stopApp = false);
};
#endif // MAINWINDOW_H
