#include "../can_h/scan.h"

ScanAdapter::ScanAdapter(Node& can, uint8_t src, const std::string& device) : can(can), device(device)
{
    uint8_t group = Groups::Raw(Groups::SCAN);
    pid = (group << 26) | (0b1 << 25) | (src << 18);
}

uint8_t ScanAdapter::GetValue()
{
    int32_t res = 0;

    Packet send(pid, 1, 1);
    can.Write(&send);

    bool next = false;
    do
    {
        Packet recv;
        next = can.Read(&recv);

        if (device.length( ) <= recv.dataLen)
        {
            bool equal = true;
            for (std::size_t i = 0; i < device.length( ); ++i)
            {
                equal &= (device[i] == static_cast<char>(recv.data[i]));
            }

            if (equal)
            {
                res = (recv.id >> 7) & 0b1111111;
                break;
            }
        }
    } while (next);

    return res;
}
