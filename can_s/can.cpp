#include <unistd.h>
#include <fcntl.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <cassert>
#include <cstring>
#include <cstdio>

#include "../can_h/can.h"

// ------------------------------------------------------------------------------------------
// Socket
// ------------------------------------------------------------------------------------------
Socket::Socket(bool loopback)
{
    sock  = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    ready = false;

    if (!loopback)
    {
        int param = 0;
        setsockopt(sock, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &param, sizeof(param));
    }
}

Socket::~Socket()
{
    close(sock);
}

bool Socket::Bind(const std::string& interface)
{
    ifreq ifr;
    sockaddr_can addr;

    std::strcpy(ifr.ifr_name, interface.c_str( ));

    int ret = ioctl(sock, SIOCGIFINDEX, &ifr);
    if (ret != 0)
    {
        return false;
    }
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    ret = bind(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));

    return (ready = (ret == 0));
}

int32_t Socket::Write(const Packet* packet)
{
    can_frame frame;
    frame.can_id = packet->id;
    if (packet->rtr)
    {
        frame.can_id |= CAN_RTR_FLAG;
    }
    if (packet->ide)
    {
        frame.can_id |= CAN_EFF_FLAG;
    }

    frame.can_dlc = packet->dataLen;
    std::memcpy(frame.data, packet->data, packet->dataLen);

    return write(sock, &frame, sizeof(frame));
}

std::size_t Socket::Read(Packet* packet, int32_t timeout)
{
    can_frame frame;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock, &set);

    timeval timing =
    {
        .tv_sec  = timeout / 1000000,
        .tv_usec = timeout % 1000000
    };

    int ret = select(sock + 1, &set, nullptr, nullptr, &timing);
    if (ret <= 0)
    {
        return 0;
    }

    int32_t bytes = read(sock, &frame, sizeof(can_frame));
    if (bytes < 0)
    {
        return 0;
    }

    packet->id  =  frame.can_id   & (~CAN_RTR_FLAG) & (~CAN_EFF_FLAG);
    packet->rtr = (CAN_RTR_FLAG   & frame.can_id) != 0;
    packet->ide = (CAN_EFF_FLAG   & frame.can_id) != 0;

    packet->dataLen = frame.can_dlc;
    std::memcpy(packet->data, frame.data, packet->dataLen);

    return bytes;
}

// ------------------------------------------------------------------------------------------
// Node
// ------------------------------------------------------------------------------------------
const char*   Node::DEFAULT_INTERFACE = "can0";

const int32_t Node::READ_TIMEOUT_US   = 10000;
//
Node::Node(const std::string& interface)
{
    sock.Bind(interface);
}

uint32_t Node::RequestSync(const Packet* send, Packet* recv)
{
    Write(send);
    return Read(recv);
}

uint32_t Node::Read(Packet* packet)
{
    return sock.Read(packet, READ_TIMEOUT_US);
}

int Node::Write(const Packet* packet)
{
    return sock.Write(packet);
}
