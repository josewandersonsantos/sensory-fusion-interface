#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QMainWindow>
#include <thread>
#include <windows.h>

class SerialPort
{
public:
    SerialPort();
    bool Start(std::string port, int baud);
    bool Stop();
    void SetOnRx(std::function<void(char*, int)> cb);

    std::vector<std::string> GetSerialPorts();
    bool IsOpen(){return isOpened;}

private:
    std::string port;
    int baud;
    bool isOpened = false;

    std::function<void(char*, int)> OnRx;

    HANDLE serialHandle;
    std::thread thRx;
    std::atomic<bool> thRxRunning = false;
    std::atomic<bool> thRxEnabled = false;

    void ThRx(void);

};

#endif // SERIALPORT_H
