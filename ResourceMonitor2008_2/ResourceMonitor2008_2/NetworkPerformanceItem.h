#pragma once
#include "stdafx.h"
#include <string>
#include <windows.h>

class NetworkPerformanceItem
{
public:
    NetworkPerformanceItem();
    ~NetworkPerformanceItem();

    ULONGLONG ProcessId;
    INT State;
    std::string LocalAddress;
    std::string RemoteAddress;
    int LocalPort;
    int RemotePort;
    ULONGLONG BytesOut;
    ULONGLONG BytesIn;
    //LONG OutboundBandwidth;
    //LONG InboundBandwidth;
    int Pass;
    std::string CollectionTime;

};