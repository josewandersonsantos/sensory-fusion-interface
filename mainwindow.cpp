#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    serialPort.Stop();
    delete ui;
}

void MainWindow::on_btnOpenSerialPort_released()
{
    if( ui->cmbSerialPorts->currentIndex() < 0)
    {
        return;
    }

    QString port = ui->cmbSerialPorts->itemText(ui->cmbSerialPorts->currentIndex());
    serialPort.Start(port.toStdString(), 9600);

}

void MainWindow::on_btnRefreshSerialPort_released()
{

}

// void MainWindow::on_pushButton_released()
// {

// }

void MainWindow::on_btnRefreshSerialPort_clicked()
{
    std::vector<std::string> ports = serialPort.GetSerialPorts();

    if(ports.size() == 0) return;

    ui->cmbSerialPorts->clear();
    for(int i = 0; i< ports.size(); i++)
    {
        ui->cmbSerialPorts->addItem(ports[i].c_str());
    }
}

