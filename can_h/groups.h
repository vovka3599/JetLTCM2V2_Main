#ifndef GROUPS_H
#define GROUPS_H

#include "command.h"

class Groups
{
    public:
        static constexpr const Command::Group VERSION_PREFIX = 0x2100;

    public:
        enum Value : Command::Group
        {
            CONTROL  = VERSION_PREFIX | 0b0000,
            RESERVE1 = VERSION_PREFIX | 0b0001,
            ALARM    = VERSION_PREFIX | 0b0010,
            RESERVE3 = VERSION_PREFIX | 0b0011,
            STATUS   = VERSION_PREFIX | 0b0100,
            INFO     = VERSION_PREFIX | 0b0101,
            CONFIG   = VERSION_PREFIX | 0b0110,
            SCAN     = VERSION_PREFIX | 0b0111,
        };

    public:
        inline static constexpr Command::Group Raw(Value value)
        {
            return (~VERSION_PREFIX) & value;
        }
};

#endif // GROUPS_H
