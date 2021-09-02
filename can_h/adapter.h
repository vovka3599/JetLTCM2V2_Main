#ifndef ADAPTER_H
#define ADAPTER_H

#include "can.h"
#include "command.h"
#include "groups.h"

template <Command::Group group, typename T = uint32_t>
class BaseAdapter
{
    public:
        static_assert(std::is_pod<T>::value, "Data type for adapter must be POD!");

    public:
        BaseAdapter(Node& can, Command::Type command, uint8_t src, uint8_t dst) : can(can)
        {
            assert(Command::DataSize<group>(command) == sizeof(T));
            uint8_t group_l = Groups::Raw(Groups::CONTROL);
            pid = (group_l   << 26) | (0b0101  << 22) | (command << 14) | (src     << 7 ) | dst;;
        }

        int SetValue(T value)
        {
            Packet packet(pid, 1, 0, sizeof(value), reinterpret_cast<uint8_t*>(&value));
            return can.Write(&packet);
        }

        T GetValue()
        {
            Packet send(pid, 1, 1);
            can.Write(&send);

            Packet recv;
            can.Read(&recv);

            return *reinterpret_cast<T*>(recv.data);
        }

    private:
        Node& can;
        uint32_t pid;
};

#endif // ADAPTER_H
