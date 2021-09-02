#include "../can_h/control.h"

template <>
std::size_t Command::DataSize<Groups::CONTROL>(Command::Type command)
{
    switch (command)
    {
        case Control::FREQ:
        {
            return 4;
        }
        case Control::SHIFT_FREQ:
        {
            return 4;
        }
        case Control::ATT:
        {
            return 2;
        }
        case Control::ATT_INDEX:
        {
            return 1;
        }
        case Control::AMP_INDEX:
        {
            return 1;
        }
        case Control::AMP_STATE:
        {
            return 1;
        }
        case Control::DRV_STATE:
        {
            return 1;
        }
        case Control::RFIN:
        {
            return 1;
        }
        case Control::POWER:
        {
            return 1;
        }
        case Control::REF:
        {
            return 4;
        }
        case Control::PFD_FREQ:
        {
            return 4;
        }
        case Control::SPUR_MODE:
        {
            return 1;
        }
        case Control::ICP:
        {
            return 4;
        }
        case Control::LEVEL:
        {
            return 1;
        }
        case Control::LNA:
        {
            return 1;
        }
        case Control::INJ:
        {
            return 2;
        }
        case Control::RFOUT:
        {
            return 1;
        }
        case Control::VOUT:
        {
            return 2;
        }
        case Control::PHASE:
        {
            return 2;
        }
    }

    return 0;
}

ControlAdapters::ControlAdapters(Node& can, uint8_t src, uint8_t dst) : can(can), src(src), dst(dst) {}

ControlAdapters::~ControlAdapters( )
{
    for (auto& entry : darray)
    {
        delete entry.second;
    }

    for (auto& entry : warray)
    {
        delete entry.second;
    }

    for (auto& entry : barray)
    {
        delete entry.second;
    }
}

bool ControlAdapters::Insert(Command::Type command)
{
    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint8_t))
    {
        return Insert(command, barray);
    }

    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint16_t))
    {
        return Insert(command, warray);
    }

    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint32_t))
    {
        return Insert(command, darray);
    }

    return false;
}

void ControlAdapters::Delete(Command::Type command)
{
    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint8_t))
    {
        Delete(command, barray);
    }

    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint16_t))
    {
        Delete(command, warray);
    }

    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint32_t))
    {
        Delete(command, darray);
    }
}

std::size_t ControlAdapters::Size( ) const
{
    return darray.size( ) + warray.size( ) + barray.size( );
}

bool ControlAdapters::Present(Command::Type command) const
{
    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint8_t))
    {
        return (barray.count(command) > 0);
    }

    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint16_t))
    {
        return (warray.count(command) > 0);
    }

    if (Command::DataSize<Groups::CONTROL>(command) == sizeof(uint32_t))
    {
        return (darray.count(command) > 0);
    }

    return false;
}

template <>
uint32_t ControlAdapters::GetValue(Command::Type command)
{
    return GetValue(command, darray);
}

template <>
uint16_t ControlAdapters::GetValue(Command::Type command)
{
    return GetValue(command, warray);
}

template <>
uint8_t  ControlAdapters::GetValue(Command::Type command)
{
    return GetValue(command, barray);
}

template <>
int ControlAdapters::SetValue(Command::Type command, uint32_t value)
{
    return SetValue(command, darray, value);
}

template <>
int ControlAdapters::SetValue(Command::Type command, uint16_t value)
{
    return SetValue(command, warray, value);
}

template <>
int ControlAdapters::SetValue(Command::Type command, uint8_t  value)
{
    return SetValue(command, barray, value);
}
