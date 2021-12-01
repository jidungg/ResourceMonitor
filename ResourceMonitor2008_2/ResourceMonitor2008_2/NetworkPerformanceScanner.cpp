#include "stdafx.h"
#include "NetworkPerformanceScanner.h"
#include "ResourceMonitorView.h"
#include "ResourceMonitorDoc.h"

NetworkPerformanceScanner::NetworkPerformanceScanner(CResourceMonitorDoc* doc)
{
	m_oldTable = new map<ULONGLONG,NetworkPerformanceItem>;
	m_newTable = new map<ULONGLONG,NetworkPerformanceItem>;
	m_diffTable = new map<ULONGLONG,NetworkPerformanceItem>;
	m_pDoc = doc;
}


NetworkPerformanceScanner::~NetworkPerformanceScanner()
{
	delete m_oldTable;
	delete m_newTable;
	delete m_diffTable;
}
void NetworkPerformanceScanner::GetData()
{
	ScanTCPPerformance();
	ScanUDPPerformance();
	ArrangeTable();
}
// TODO - implement TCP v6, UDP
void NetworkPerformanceScanner::ScanTCPPerformance()
{
	(*m_newTable).clear();
	m_stoppedProcIDs.clear();
    std::vector<unsigned char> buffer;
    DWORD dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);
    DWORD dwRetValue = 0;
    //vector<NetworkPerformanceItem> networkPerformanceItems;;

    // repeat till buffer is big enough
    do
    {
        buffer.resize(dwSize, 0);
        dwRetValue = GetExtendedTcpTable(&buffer[0], &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);

    } while (dwRetValue == ERROR_INSUFFICIENT_BUFFER);

    if (dwRetValue == ERROR_SUCCESS)
    {
        // good case

        // cast to access element values
        PMIB_TCPTABLE_OWNER_PID ptTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(&buffer[0]);

        // caution: array starts with index 0, count starts by 1
        for (DWORD i = 0; i < ptTable->dwNumEntries; i++)
        {
            NetworkPerformanceItem networkPerformanceItem;

            networkPerformanceItem.ProcessId = ptTable->table[i].dwOwningPid;
            networkPerformanceItem.State = ptTable->table[i].dwState;           

            std::ostringstream localStream;
            localStream << (ptTable->table[i].dwLocalAddr & 0xFF)
                << "."
                << ((ptTable->table[i].dwLocalAddr >> 8) & 0xFF)
                << "."
                << ((ptTable->table[i].dwLocalAddr >> 16) & 0xFF)
                << "."
                << ((ptTable->table[i].dwLocalAddr >> 24) & 0xFF)
                << ":"
                << htons((unsigned short)ptTable->table[i].dwLocalPort);
            networkPerformanceItem.LocalAddress = localStream.str();
            networkPerformanceItem.LocalPort = ptTable->table[i].dwLocalPort;

            std::ostringstream remoteStream;
            remoteStream << (ptTable->table[i].dwRemoteAddr & 0xFF)
                << "."
                << ((ptTable->table[i].dwRemoteAddr >> 8) & 0xFF)
                << "."
                << ((ptTable->table[i].dwRemoteAddr >> 16) & 0xFF)
                << "."
                << ((ptTable->table[i].dwRemoteAddr >> 24) & 0xFF)
                << ":"
                << htons((unsigned short)ptTable->table[i].dwRemotePort);
            networkPerformanceItem.RemoteAddress = remoteStream.str();
            networkPerformanceItem.RemotePort = ptTable->table[i].dwRemotePort;

            MIB_TCPROW row;
            row.dwLocalAddr = ptTable->table[i].dwLocalAddr;
            row.dwLocalPort = ptTable->table[i].dwLocalPort;
            row.dwRemoteAddr = ptTable->table[i].dwRemoteAddr;
            row.dwRemotePort = ptTable->table[i].dwRemotePort;
            row.dwState = ptTable->table[i].dwState;
            void *processRow = &row;


            if (row.dwRemoteAddr != 0)
            {
                ULONG rosSize = 0, rodSize = 0;
                ULONG winStatus;
                PUCHAR ros = NULL, rod = NULL;
                rodSize = sizeof(TCP_ESTATS_DATA_ROD_v0);
                PTCP_ESTATS_DATA_ROD_v0 dataRod = { 0 };

                if (rosSize != 0) {
                    ros = (PUCHAR)malloc(rosSize);
                    if (ros == NULL) {
                        wprintf(L"\nOut of memory");
                        return ;
                    }
                    else
                        memset(ros, 0, rosSize); // zero the buffer
                }
                if (rodSize != 0) {
                    rod = (PUCHAR)malloc(rodSize);
                    if (rod == NULL) {
                        free(ros);
                        wprintf(L"\nOut of memory");
                        return ;
                    }
                    else
                        memset(rod, 0, rodSize); // zero the buffer
                }

                winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row, TcpConnectionEstatsData, NULL, 0, 0, ros, 0, rosSize, rod, 0, rodSize);
                dataRod = (PTCP_ESTATS_DATA_ROD_v0)rod;

				if(dataRod->DataBytesIn > 0)
				{
					networkPerformanceItem.BytesIn = dataRod->DataBytesIn;
					networkPerformanceItem.BytesOut = dataRod->DataBytesOut;

				}else
				{
					networkPerformanceItem.BytesIn = 0;
					networkPerformanceItem.BytesOut = 0;
				}

				//get bandwidth 
                //PTCP_ESTATS_BANDWIDTH_ROD_v0 bandwidthRod = { 0 };

                //rodSize = sizeof(TCP_ESTATS_BANDWIDTH_ROD_v0);
                //if (rodSize != 0) {
                //    rod = (PUCHAR)malloc(rodSize);
                //    if (rod == NULL) {
                //        free(ros);
                //        wprintf(L"\nOut of memory");
                //        return ;
                //    }
                //    else
                //        memset(rod, 0, rodSize); // zero the buffer
                //}

                //winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row,TcpConnectionEstatsBandwidth, NULL, 0, 0, ros, 0, rosSize, rod, 0, rodSize);

                //bandwidthRod = (PTCP_ESTATS_BANDWIDTH_ROD_v0)rod;
                //networkPerformanceItem.OutboundBandwidth = bandwidthRod->OutboundBandwidth;
                //networkPerformanceItem.InboundBandwidth = bandwidthRod->InboundBandwidth;

            }
            //networkPerformanceItem.Pass = pass;
            //networkPerformanceItem.CollectionTime = collectionTime;
            //networkPerformanceItems.push_back(networkPerformanceItem);

			map<ULONGLONG,NetworkPerformanceItem>::iterator it = (*m_newTable).find(networkPerformanceItem.ProcessId);
			if(it != (*m_newTable).end())
			{
				(*m_newTable)[networkPerformanceItem.ProcessId].BytesIn += networkPerformanceItem.BytesIn;
				(*m_newTable)[networkPerformanceItem.ProcessId].BytesOut += networkPerformanceItem.BytesOut;
			}else
			{
				(*m_newTable)[networkPerformanceItem.ProcessId] = networkPerformanceItem;
				(*m_diffTable)[networkPerformanceItem.ProcessId] = networkPerformanceItem;
				(*m_oldTable)[networkPerformanceItem.ProcessId].liveFlag = true;
			}			
		
        }
    }
    else
    {
        // bad case, do some sh*t here
    }
 
    return ;
}

void NetworkPerformanceScanner::ScanUDPPerformance()
{
//	(*m_newTable).clear();
//	m_stoppedProcIDs.clear();
//    std::vector<unsigned char> buffer;
//    DWORD dwSize = sizeof(MIB_UDPTABLE_OWNER_PID);
//    DWORD dwRetValue = 0;
//    //vector<NetworkPerformanceItem> networkPerformanceItems;;
//
//    // repeat till buffer is big enough
//    do
//    {
//        buffer.resize(dwSize, 0);
//        dwRetValue = GetExtendedUdpTable(&buffer[0], &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID_ALL, 0);
//
//    } while (dwRetValue == ERROR_INSUFFICIENT_BUFFER);
//
//    if (dwRetValue == ERROR_SUCCESS)
//    {
//        // good case
//
//        // cast to access element values
//        PMIB_UDPTABLE_OWNER_PID ptTable = reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>(&buffer[0]);
//
//        // caution: array starts with index 0, count starts by 1
//        for (DWORD i = 0; i < ptTable->dwNumEntries; i++)
//        {
//            NetworkPerformanceItem networkPerformanceItem;
//
//            networkPerformanceItem.ProcessId = ptTable->table[i].dwOwningPid;          
//
//            std::ostringstream localStream;
//            localStream << (ptTable->table[i].dwLocalAddr & 0xFF)
//                << "."
//                << ((ptTable->table[i].dwLocalAddr >> 8) & 0xFF)
//                << "."
//                << ((ptTable->table[i].dwLocalAddr >> 16) & 0xFF)
//                << "."
//                << ((ptTable->table[i].dwLocalAddr >> 24) & 0xFF)
//                << ":"
//                << htons((unsigned short)ptTable->table[i].dwLocalPort);
//            networkPerformanceItem.LocalAddress = localStream.str();
//            networkPerformanceItem.LocalPort = ptTable->table[i].dwLocalPort;
//
//            MIB_UDPROW row;
//            row.dwLocalAddr = ptTable->table[i].dwLocalAddr;
//            row.dwLocalPort = ptTable->table[i].dwLocalPort;
//
//            void *processRow = &row;
//
//
//            if (row.dwLocalAddr != 0)
//            {
//                ULONG rosSize = 0, rodSize = 0;
//                ULONG winStatus;
//                PUCHAR ros = NULL, rod = NULL;
//                rodSize = sizeof(UDP_ESTATS_DATA_ROD_v0);
//                PUDP_ESTATS_DATA_ROD_v0 dataRod = { 0 };
//
//                if (rosSize != 0) {
//                    ros = (PUCHAR)malloc(rosSize);
//                    if (ros == NULL) {
//                        wprintf(L"\nOut of memory");
//                        return ;
//                    }
//                    else
//                        memset(ros, 0, rosSize); // zero the buffer
//                }
//                if (rodSize != 0) {
//                    rod = (PUCHAR)malloc(rodSize);
//                    if (rod == NULL) {
//                        free(ros);
//                        wprintf(L"\nOut of memory");
//                        return ;
//                    }
//                    else
//                        memset(rod, 0, rodSize); // zero the buffer
//                }
//
//                winStatus = GetPerUdpConnectionEStats((PMIB_UDPROW)&row, UdpConnectionEstatsData, NULL, 0, 0, ros, 0, rosSize, rod, 0, rodSize);
//                dataRod = (PUDP_ESTATS_DATA_ROD_v0)rod;
//
//				if(dataRod->DataBytesIn > 0)
//				{
//					networkPerformanceItem.BytesIn = dataRod->DataBytesIn;
//					networkPerformanceItem.BytesOut = dataRod->DataBytesOut;
//
//				}else
//				{
//					networkPerformanceItem.BytesIn = 0;
//					networkPerformanceItem.BytesOut = 0;
//				}
//
//				//get bandwidth 
//                //PTCP_ESTATS_BANDWIDTH_ROD_v0 bandwidthRod = { 0 };
//
//                //rodSize = sizeof(TCP_ESTATS_BANDWIDTH_ROD_v0);
//                //if (rodSize != 0) {
//                //    rod = (PUCHAR)malloc(rodSize);
//                //    if (rod == NULL) {
//                //        free(ros);
//                //        wprintf(L"\nOut of memory");
//                //        return ;
//                //    }
//                //    else
//                //        memset(rod, 0, rodSize); // zero the buffer
//                //}
//
//                //winStatus = GetPerTcpConnectionEStats((PMIB_TCPROW)&row,TcpConnectionEstatsBandwidth, NULL, 0, 0, ros, 0, rosSize, rod, 0, rodSize);
//
//                //bandwidthRod = (PTCP_ESTATS_BANDWIDTH_ROD_v0)rod;
//                //networkPerformanceItem.OutboundBandwidth = bandwidthRod->OutboundBandwidth;
//                //networkPerformanceItem.InboundBandwidth = bandwidthRod->InboundBandwidth;
//
//            }
//            //networkPerformanceItem.Pass = pass;
//            //networkPerformanceItem.CollectionTime = collectionTime;
//            //networkPerformanceItems.push_back(networkPerformanceItem);
//
//			map<ULONGLONG,NetworkPerformanceItem>::iterator it = (*m_newTable).find(networkPerformanceItem.ProcessId);
//			if(it != (*m_newTable).end())
//			{
//				(*m_newTable)[networkPerformanceItem.ProcessId].BytesIn += networkPerformanceItem.BytesIn;
//				(*m_newTable)[networkPerformanceItem.ProcessId].BytesOut += networkPerformanceItem.BytesOut;
//			}else
//			{
//				(*m_newTable)[networkPerformanceItem.ProcessId] = networkPerformanceItem;
//				(*m_diffTable)[networkPerformanceItem.ProcessId] = networkPerformanceItem;
//				(*m_oldTable)[networkPerformanceItem.ProcessId].liveFlag = true;
//			}			
//		
//        }
//    }
//    else
//    {
//        // bad case, do some sh*t here
//    }
 
    return ;
}

void NetworkPerformanceScanner::ArrangeTable()
{
		//Arange Table
	for(map<ULONGLONG,NetworkPerformanceItem>::iterator it = m_oldTable->begin(); it != m_oldTable->end(); it++)
	{
		if(it->second.liveFlag == false)
		{
			m_stoppedProcIDs.push_back(it->first);
		}
	}
	if(m_stoppedProcIDs.size() > 0)
	{
		for (vector<ULONGLONG>::iterator iter = m_stoppedProcIDs.begin(); iter != m_stoppedProcIDs.end(); iter++)
		{
			m_diffTable->erase(*iter);
		}
		m_pDoc->AtStoppedNetProcess(&m_stoppedProcIDs);
	}

	for(map<ULONGLONG,NetworkPerformanceItem>::iterator it = m_newTable->begin(); it != m_newTable->end(); it++)
	{

		(*m_diffTable)[it->first].BytesIn = it->second.BytesIn  - (*m_oldTable)[it->first].BytesIn ;
		(*m_diffTable)[it->first].BytesOut = it->second.BytesOut  - (*m_oldTable)[it->first].BytesOut ;
		(*m_newTable)[it->first].liveFlag = false;

	}
	
	m_oldTable->swap(*m_newTable);
}