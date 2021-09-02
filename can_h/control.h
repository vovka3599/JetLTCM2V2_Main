#ifndef CONTROL_H
#define CONTROL_H

#include <map>
#include "adapter.h"
#include "groups.h"


class Control
{
    public:
        enum : Command::Type
        {
            FREQ       = 0x0000, // 4
            SHIFT_FREQ = 0x0001, // 4
            ATT        = 0x0002, // 2
            ATT_INDEX  = 0x0004, // 1
            AMP_INDEX  = 0x0005, // 1
            DRV_INDEX  = 0x0006, // 1
            AMP_STATE  = 0x0007, // 1
            DRV_STATE  = 0x0008, // 1
            RFIN       = 0x0009, // 1
            POWER      = 0x000A, // 1
            REF        = 0x000C, // 4
            PFD_FREQ   = 0x000E, // 4
            SPUR_MODE  = 0x000F, // 1
            ICP        = 0x0010, // 4
            LEVEL      = 0x0011, // 1
            LNA        = 0x0012, // 1
            INJ        = 0x0014, // 2
            RFOUT      = 0x0015, // 1
            VOUT       = 0x0016, // 2
            PHASE      = 0x0017, // 2
        };
};

template <typename T>
using ControlAdapter = BaseAdapter<Groups::CONTROL, T>;

class ControlAdapters
{
    public:
        ControlAdapters(Node& can, uint8_t src, uint8_t dst);
        ~ControlAdapters( );
        bool Insert(Command::Type command);
        void Delete(Command::Type command);
        std::size_t Size( ) const;
        bool Present(Command::Type command) const;

        template <typename T>
        T GetValue(Command::Type command);

        template <typename T>
        int SetValue(Command::Type command, T value);

    private:
        template <typename T>
        using Array = std::map<Command::Type, ControlAdapter<T>*>;

    private:
        template <typename T>
        bool Insert(Command::Type command, Array<T>& array)
        {
            if (array.count(command) > 0)
            {
                return false;
            }

            array[command] = new ControlAdapter<T>(can, command, src, dst);
            return true;
        }

        template <typename T>
        void Delete(Command::Type command, Array<T>& array)
        {
            auto it = array.find(command);
            if (it == std::end(array))
            {
                return;
            }

            delete it->second;
            array.erase(it);
        }

        template <typename T>
        T GetValue(Command::Type command, Array<T>& array)
        {
            auto it = array.find(command);
            if (it == std::end(array))
            {
                return T( );
            }

            return (it->second)->GetValue( );
        }

        template <typename T>
        int SetValue(Command::Type command, Array<T>& array, T value)
        {
            auto it = array.find(command);
            if (it == std::end(array))
            {
                return T( );
            }

            return (it->second)->SetValue(value);
        }

    private:
        Node& can;

        uint8_t src;
        uint8_t dst;

        Array<uint32_t> darray;
        Array<uint16_t> warray;
        Array<uint8_t > barray;
};

template <>
uint32_t ControlAdapters::GetValue(Command::Type command);

template <>
uint16_t ControlAdapters::GetValue(Command::Type command);

template <>
uint8_t  ControlAdapters::GetValue(Command::Type command);

template <>
int ControlAdapters::SetValue(Command::Type command, uint32_t value);

template <>
int ControlAdapters::SetValue(Command::Type command, uint16_t value);

template <>
int ControlAdapters::SetValue(Command::Type command, uint8_t  value);

template <>
std::size_t Command::DataSize<Groups::CONTROL>(Command::Type command);

#endif // CONTROL_H
