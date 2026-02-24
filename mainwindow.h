#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialport.h"
#include "tcpclient.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpenSerialPort_released();

    void on_pushButton_released();

    void on_btnRefreshSerialPort_released();

    void on_btnRefreshSerialPort_clicked();

private:
    Ui::MainWindow *ui;
    SerialPort serialPort;
    TcpClient tcpClient;
};
#endif // MAINWINDOW_H
