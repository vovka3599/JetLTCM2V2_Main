#ifndef CAN_H
#define CAN_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cassert>
#include <list>
#include <string>

typedef uint32_t Address;
const Address BROADCAST_ADDR = 0;

struct Packet
{
    static const size_t maxPacketDataLen = 8;
    uint32_t id;
    bool ide;
    bool rtr;
    uint32_t dataLen;
    uint8_t data[maxPacketDataLen];
    Packet() :
        id(0),
        ide(false),
        rtr(false),
        dataLen(0),
        data{}{
    }

    Packet(uint32_t id, int ide = 0, int rtr = 0, uint32_t dataLen = 0, const uint8_t *data = nullptr) : id(id), ide(ide), rtr(rtr), dataLen(dataLen)
    {
        memset(this->data, 0, maxPacketDataLen);
        if (dataLen)
        {
            assert(data != NULL);
            assert(dataLen <= maxPacketDataLen);
            memcpy(this->data, data, dataLen);
        }
    }
};

/**
 * Acceptance filter mask
 */
struct AFMask
{
    uint32_t id;
    bool srr;
    bool rtr;
    bool ide;
};

using Filter  = AFMask;
using Filters = std::list<Filter>;


class Socket
{
    public:
        Socket(bool loopback = false);

        ~Socket();

        bool Bind(const std::string& interface);

        int32_t Write(const Packet* packet);

        std::size_t Read(Packet* packet, int32_t timeout);

    private:
        int  sock;
        bool ready;
};


class Node
{
    public:
        static const char* DEFAULT_INTERFACE;

        static const int32_t READ_TIMEOUT_US;

    public:
        Node(const std::string& interface = DEFAULT_INTERFACE);

        uint32_t RequestSync(const Packet* send, Packet* recv);

        uint32_t Read(Packet* packet);

        int Write(const Packet* packet);

    private:
        Socket sock;
};
#endif // CAN_H
