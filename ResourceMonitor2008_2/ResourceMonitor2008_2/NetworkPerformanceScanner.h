#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "NetworkPerformanceItem.h"
#include <iomanip>
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#include <Tcpestats.h>
#include <stdlib.h>
#include <stdio.h>


using namespace std;

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
class CResourceMonitorDoc;

class NetworkPerformanceScanner
{
public:
    NetworkPerformanceScanner(CResourceMonitorDoc* doc);
    ~NetworkPerformanceScanner();

	vector<ULONGLONG>						m_stoppedProcIDs;
	CResourceMonitorDoc						*m_pDoc ;
	map<ULONGLONG,NetworkPerformanceItem>	*m_oldTable;
	map<ULONGLONG,NetworkPerformanceItem>	*m_newTable;
	map<ULONGLONG,NetworkPerformanceItem>	*m_diffTable;

	void GetData();
    void ScanTCPPerformance();
    void ScanUDPPerformance();
	void ArrangeTable();
};