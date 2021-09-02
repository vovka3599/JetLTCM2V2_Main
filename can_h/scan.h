#ifndef SCAN_H
#define SCAN_H

#include <string>

#include "adapter.h"
#include "groups.h"

class ScanAdapter
{
    public:
        ScanAdapter(Node& can, uint8_t src, const std::string& device);
        uint8_t GetValue();

    private:
        Node& can;
        uint32_t pid;
        std::string device;
};
#endif // SCAN_H
