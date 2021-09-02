#ifndef COMMAND_H
#define COMMAND_H
#include <cstdint>

class Command
{
    public:
        using Type  = uint16_t;
        using Group = uint16_t;

    public:
        template <Group group>
        static std::size_t DataSize(Command::Type command);

        template <Group group>
        static std::size_t DataSize();
};

#endif // COMMAND_H
