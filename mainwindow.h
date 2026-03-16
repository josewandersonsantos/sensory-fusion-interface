#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bridge.h"
#include "tcpclient.h"
#include <QMainWindow>
#include "viewer3d.h"
#include <QTimer>
#include <mutex>

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
    void on_btnRefreshSerialPort_released();
    void on_btnRefreshSerialPort_clicked();
    void on_btnLogClear_clicked();
    void on_rbnLogScroll_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    //SerialPort serialPort;
    Bridge bridge;
    TcpClient tcpClient;
    Viewer3D viewer3D;

    std::mutex logMutex;
    QStringList logBuffer;
    QTimer *logTimer;
};
#endif // MAINWINDOW_H
