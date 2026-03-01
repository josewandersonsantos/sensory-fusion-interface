#include "serialport.h"
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <vector>
#include <string>

#pragma comment(lib, "setupapi.lib")

SerialPort::SerialPort()
{}

bool SerialPort::Start(std::string port, int baud)
{
    serialHandle = CreateFileA(("\\\\.\\" + port).c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (serialHandle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    // Do some basic settings
    DCB serialParams = { 0 };
    serialParams.DCBlength = sizeof(serialParams);

    GetCommState(serialHandle, &serialParams);
    if (!GetCommState(serialHandle, &serialParams))
    {
        CloseHandle(serialHandle);
        return false;
    }

    serialParams.BaudRate = baud;
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = NOPARITY;
    SetCommState(serialHandle, &serialParams);
    if (!SetCommState(serialHandle, &serialParams))
    {
        CloseHandle(serialHandle);
        return false;
    }

    // Set timeouts
    COMMTIMEOUTS timeout = { 0 };
    timeout.ReadIntervalTimeout = 50;
    timeout.ReadTotalTimeoutConstant = 50;
    timeout.ReadTotalTimeoutMultiplier = 50;
    timeout.WriteTotalTimeoutConstant = 50;
    timeout.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(serialHandle, &timeout);

    thRxEnabled = true;
    thRx = std::thread(&SerialPort::ThRx, this);
    isOpened = true;
    /*
     *
     */
    return true;
}

bool SerialPort::Stop()
{
    if( ! isOpened) return true;

    thRxEnabled = false;
    uint32_t cont = 0;
    while(cont++ < 5000 && thRxRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if(thRxRunning)
    {
        thRx.detach();
    }
    thRx.join();

    CloseHandle(serialHandle);

    isOpened = false;
    return true;
}

std::vector<std::string> SerialPort::GetSerialPorts()
{
    std::vector<std::string> ports;

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        char valueName[256];
        char data[256];
        DWORD valueNameSize, dataSize, type;
        DWORD index = 0;

        while (true)
        {
            valueNameSize = sizeof(valueName);
            dataSize = sizeof(data);

            if (RegEnumValueA(hKey, index++, valueName, &valueNameSize, NULL, &type, (LPBYTE)data, &dataSize) != ERROR_SUCCESS)
            {
                break;
            }

            ports.push_back(std::string(data));
        }

        RegCloseKey(hKey);
    }

    return ports;
}

void SerialPort::SetOnRx(std::function<void(char*, int)> cb)
{
    OnRx = cb;
}

void SerialPort::ThRx()
{
    thRxRunning = true;
    uint32_t sleepMax = 1;

    char buffer[256];
    DWORD bytesRead;

    while(thRxEnabled)
    {
        if (!ReadFile(serialHandle, buffer, sizeof(buffer), &bytesRead, NULL))
        {
            continue;
        }

        if (bytesRead > 0)
        {
            std::string data(buffer, bytesRead);

            if (SerialPort::OnRx != NULL)
            {
                SerialPort::OnRx(buffer, (int)bytesRead);
            }
            //std::cout << "Recebido: " << data << std::endl;
        }

        uint32_t cont = 0;
        while(cont++ < sleepMax && thRxEnabled)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    thRxRunning = false;
}

