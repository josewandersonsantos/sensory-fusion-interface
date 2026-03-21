#ifndef BRIDGE_H
#define BRIDGE_H

#include "bridgetypes.h"
#include "serialport.h"
#include <vector>
/*
 * ENUMS
 */
typedef enum __bridge_rx_states_t
{
    BRIDGE_RX_WAITING_SOF,
    BRIDGE_RX_WAITING_HEADER,
    BRIDGE_RX_WAITING_PAYLOAD,
    BRIDGE_RX_WAITING_CRC,

} bridge_rx_states_t;

class Bridge
{
public:
    Bridge();

    void OnNewdata(uint8_t* data, size_t len);
    void SetOnRxDataImu(std::function<void(float, float, float, float, float, float, float)> cb);
    void SetOnRxDataGps(std::function<void(char*, int)> cb);
    void SetOnRxCoords(std::function<void(float, float, float)> cb);
    void SendCfgData();

    bool Connect(std::string port, int baud);
    bool IsConnect();
    void Disconnect();

    inline std::vector<std::string> GetSerialPorts()
    {
        return serialPort.GetSerialPorts();
    }
    
    private:
    SerialPort serialPort;
    std::function<void(float, float, float, float, float, float, float)> OnImuData;
    std::function<void(char*, int)> OnGpsData;
    std::function<void(float, float, float)> OnCoords;
    
    bridge_rx_states_t rx_state = BRIDGE_RX_WAITING_SOF;
    std::vector<uint8_t> buff_rx;

    bool VerifyFrame(std::vector<uint8_t> data);
    // void ManageFrame(std::vector<uint8_t> data);
    void ManageFrame(uint8_t* data);

};

#endif // BRIDGE_H
