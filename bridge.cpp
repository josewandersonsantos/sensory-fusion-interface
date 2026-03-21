#include "bridge.h"
#include "crc.hpp"

Bridge::Bridge() {}

void Bridge::SetOnRxDataImu(std::function<void(float, float, float, float, float, float, float)> cb)
{
    OnImuData = cb;
}

void Bridge::SetOnRxDataGps(std::function<void(char*, int)> cb)
{
    OnGpsData = cb;
}

void Bridge::SetOnRxCoords(std::function<void(float, float, float)> cb)
{
    OnCoords = cb;
}

bool Bridge::Connect(std::string port, int baud)
{
    if(serialPort.Start(port, baud))
    {
        serialPort.SetOnRx([this](char* data, size_t len)
          {
            OnNewdata((uint8_t*)data, len);
          });

        return true;
    }

    return false;
}

void Bridge::Disconnect()
{
    serialPort.Stop();
}

bool Bridge::IsConnect()
{
    return serialPort.IsOpen();
}

void Bridge::SendCfgData()
{

}

// void Bridge::ManageFrame(std::vector<uint8_t> data)

void Bridge::ManageFrame(uint8_t* data)
{
    switch(((frame_t*)&data[0])->header.type)
    {
        case FR_TYPE_VERSION:
        break;
        case FR_TYPE_CFG:
        break;
        case FR_TYPE_GPS_DATA:
            Bridge::OnGpsData((char *)&data[sizeof(frame_header_t)], (int)((frame_t*)&data[0])->header.len);
        break;
        case FR_TYPE_COORDS:
        {
            float lat = *(float*)&data[sizeof(frame_header_t)];
            float lng = *(float*)&data[sizeof(frame_header_t) + 4];
            float alt = *(float*)&data[sizeof(frame_header_t) + 8];

            Bridge::OnCoords(lat, lng, alt);
        }
        break;
        case FR_TYPE_ACC_DATA:
        break;
        case FR_TYPE_GYR_DATA:
        break;
        case FR_TYPE_MPU_DATA:
        {
            float acc_x = *(float*)&data[sizeof(frame_header_t)];
            float acc_y = *(float*)&data[sizeof(frame_header_t) + 4];
            float acc_z = *(float*)&data[sizeof(frame_header_t) + 8];
            float gyr_x = *(float*)&data[sizeof(frame_header_t) + 12];
            float gyr_y = *(float*)&data[sizeof(frame_header_t) + 16];
            float gyr_z = *(float*)&data[sizeof(frame_header_t) + 20];
            float temp  = *(float*)&data[sizeof(frame_header_t) + 24];
            
            Bridge::OnImuData(acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, temp);
        }
        break;
    }
}

// void Bridge::OnNewdata(uint8_t* data, size_t len)
// {
//     uint8_t idx = 0;
//     while(idx < len)
//     {
//         /*
//          * WAITING SOF
//          */
//         if (rx_state == BRIDGE_RX_WAITING_SOF)
//         {
//             if(data[idx++] == SOF)
//             {
//                 rx_state = BRIDGE_RX_WAITING_HEADER;
//             }
//             else
//             {
//                 continue;
//             }
//         }
//         /*
//          * WAITING HEADER
//          */
//         if (rx_state == BRIDGE_RX_WAITING_HEADER)
//         {
//             buff_rx.push_back(data[idx++]);
//             if(buff_rx.size() == sizeof(frame_header_t))
//             {
//                 rx_state = BRIDGE_RX_WAITING_PAYLOAD;
//             }
//             else
//             {
//                 continue;
//             }
//         }
//         /*
//          * WAITING PAYLOAD
//          */
//         if (rx_state == BRIDGE_RX_WAITING_PAYLOAD)
//         {
//             buff_rx.push_back(data[idx++]);
//             if(buff_rx.size() == sizeof(frame_header_t) + ((frame_t*)&buff_rx[0])->header.len)
//             {
//                 rx_state = BRIDGE_RX_WAITING_CRC;
//             }
//             else if(buff_rx.size() > PAYLOAD_SIZE_MAX + sizeof(frame_header_t))
//             {
//                 buff_rx.clear();
//                 rx_state = BRIDGE_RX_WAITING_SOF;
//                 continue;
//             }
//             else
//             {
//                 continue;
//             }
//         }
//         /*
//          * WAITING CRC
//          *
//          */
//         if (rx_state == BRIDGE_RX_WAITING_CRC)
//         {
//             buff_rx.push_back(data[idx++]);
//             if(buff_rx.size() > PAYLOAD_SIZE_MAX + sizeof(frame_header_t))
//             {
//                 buff_rx.clear();
//                 rx_state = BRIDGE_RX_WAITING_SOF;
//                 continue;
//             }
//             else if(buff_rx.size() == sizeof(frame_header_t) + ((frame_t*)&buff_rx[0])->header.len + sizeof(uint16_t))
//             {
//                 if(Bridge::VerifyFrame(buff_rx))
//                 {
//                     Bridge::ManageFrame(buff_rx);
//                 }

//                 buff_rx.clear();
//                 rx_state = BRIDGE_RX_WAITING_SOF;
//                 continue;
//             }
//             else
//             {
//                 continue;
//             }
//         }
//     }
// }

uint8_t buffer_rx[256];
size_t buff_len = 0;
uint16_t expected_payload_len;

void Bridge::OnNewdata(uint8_t* data, size_t len)
{
    while(len--)
    {
        uint8_t byte = *data++;

        switch(rx_state)
        {
        case BRIDGE_RX_WAITING_SOF:

            if(byte == SOF)
            {
                buff_len = 0;
                rx_state = BRIDGE_RX_WAITING_HEADER;
            }

            break;

        case BRIDGE_RX_WAITING_HEADER:

            buffer_rx[buff_len++] = byte;

            if(buff_len == sizeof(frame_header_t))
            {
                expected_payload_len = ((frame_header_t*)buffer_rx)->len;

                rx_state = BRIDGE_RX_WAITING_PAYLOAD;
            }

            break;

        case BRIDGE_RX_WAITING_PAYLOAD:

            buffer_rx[buff_len++] = byte;

            if(buff_len == sizeof(frame_header_t) + expected_payload_len)
            {
                rx_state = BRIDGE_RX_WAITING_CRC;
            }

            if(buff_len > 256)
            {
                rx_state = BRIDGE_RX_WAITING_SOF;
            }

            break;

        case BRIDGE_RX_WAITING_CRC:

            buffer_rx[buff_len++] = byte;

            if(buff_len == sizeof(frame_header_t) + expected_payload_len + sizeof(uint16_t))
            {
                // if(VerifyFrame(buffer_rx, buff_len))
                ManageFrame(buffer_rx);
                rx_state = BRIDGE_RX_WAITING_SOF;
            }

            break;
        }
    }
}

bool Bridge::VerifyFrame(std::vector<uint8_t> data)
{
    // uint16_t crc_rx = frame->crc;
    // uint16_t crc_cl = CRC_GetCRC16(frame, (frame->header.len + sizeof(frame_header_t)));
    
    // return (crc_cl == crc_rx)

    return true;
}
