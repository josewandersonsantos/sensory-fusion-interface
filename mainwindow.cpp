#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDateTime>

const int MAX_LINES = 100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->wdg3dviewer);
    QWidget *container = QWidget::createWindowContainer(viewer3D.window());
    layout->addWidget(container);
    on_btnRefreshSerialPort_clicked();

    logTimer = new QTimer(this);

    connect(logTimer, &QTimer::timeout, this, [this]()
    {
        QStringList buffer;

        {
            std::lock_guard<std::mutex> lock(logMutex);
            buffer.swap(logBuffer);
        }

        if(buffer.isEmpty())
            return;

        QString time = QDateTime::currentDateTime().toString("HH:mm:ss");

        for(const QString &line : buffer)
        {
            ui->ltwOutSerialPort->addItem("[" + time + "] " + line);
        }

        if(ui->rbnLogScroll->isChecked())
        {
            ui->ltwOutSerialPort->scrollToBottom();
        }
    });

    logTimer->start(500);
}

MainWindow::~MainWindow()
{
    bridge.Disconnect();
    delete ui;
}

void MainWindow::on_btnOpenSerialPort_released()
{
    if( ui->cmbSerialPorts->currentIndex() < 0)
    {
        return;
    }

    if(bridge.IsConnect())
    {
        bridge.Disconnect();
        ui->btnOpenSerialPort->setText("Open");
        ui->btnRefreshSerialPort->setEnabled(true);
        ui->cmbSerialPorts->setEnabled(true);
        return;
    }

    QString port = ui->cmbSerialPorts->itemText(ui->cmbSerialPorts->currentIndex());
    if(bridge.Connect(port.toStdString(), 9600))
    {
        bridge.SetOnRxDataGps([this](char* data, size_t len)
        {
            QString line = QString::fromUtf8(data, len);
            std::lock_guard<std::mutex> lock(logMutex);
            logBuffer.append(line);
        });

        bridge.SetOnRxDataImu([this](float acc_x, float acc_y, float acc_z, float gyr_x, float gyr_y, float gyr_z, float temp)
        {
            QMetaObject::invokeMethod(this,[=]()
            {
                ui->txtAccX->setText(QString::number(acc_x,'f',3));
                ui->txtAccY->setText(QString::number(acc_y,'f',3));
                ui->txtAccZ->setText(QString::number(acc_z,'f',3));

                ui->txtGyrX->setText(QString::number(gyr_x,'f',3));
                ui->txtGyrY->setText(QString::number(gyr_y,'f',3));
                ui->txtGyrZ->setText(QString::number(gyr_z,'f',3));

                // ui->txt->setText(QString::number(temp,'f',2));

                float roll  = atan2(acc_y, acc_z);
                float pitch = atan2(-acc_x, sqrt(acc_y*acc_y + acc_z*acc_z));

                roll  *= 180.0f / M_PI;
                pitch *= 180.0f / M_PI;

                ui->txtPitch->setText(QString::number(pitch,'f',3));
                ui->txtRoll->setText(QString::number(roll,'f',3));
                ui->txtYaw->setText(QString::number(0,'f',3));

                viewer3D.setRotation(roll, pitch, 0);
            });
        });

        ui->btnOpenSerialPort->setText("Close");
        ui->btnRefreshSerialPort->setEnabled(false);
        ui->cmbSerialPorts->setEnabled(false);
    }
}

void MainWindow::on_btnRefreshSerialPort_released()
{

}

void MainWindow::on_btnRefreshSerialPort_clicked()
{
    std::vector<std::string> ports = bridge.GetSerialPorts();

    if(ports.size() == 0) return;

    ui->cmbSerialPorts->clear();
    for(int i = 0; i< ports.size(); i++)
    {
        ui->cmbSerialPorts->addItem(ports[i].c_str());
    }
}

void MainWindow::on_btnLogClear_clicked()
{
    ui->ltwOutSerialPort->clear();
}

void MainWindow::on_rbnLogScroll_toggled(bool checked)
{
    ui->ltwOutSerialPort->scrollToBottom();
}

