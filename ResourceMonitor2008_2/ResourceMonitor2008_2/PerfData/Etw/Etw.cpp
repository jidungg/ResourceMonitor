
#include "stdAfx.h"
#include <windows.h>
#include <stdio.h>
#include <wbemidl.h>
#include <conio.h>
#include <strsafe.h>
#include <wmistr.h>
#include <evntrace.h>
#include <evntcons.h>
#include <in6addr.h>
#include "Etw.h"
#include "../../ResourceMonitorDoc.h"
#include "EtwData.h"

using namespace std;

static TRACEHANDLE		m_SessionHandle;
static VOID WINAPI StaticRecordEventCallback(PEVENT_RECORD pEvent);
static DWORD WINAPI Win32TracingThread(LPVOID Parameter);
EVENT_TRACE_PROPERTIES* m_pSessionProperties;
void ProcessDiskEvent(const ETW_DISK_EVENT& Event);
void ProcessNetworkEvent(const ETW_NETWORK_EVENT& Event);

map<ULONG, ProcessDiskData> CEtw::diskMapRealTime;
map<ULONG, ProcessNetworkData> CEtw::networkMapRealTIme;

CEtw::CEtw(void)
{
}
CEtw::CEtw(CResourceMonitorDoc* pDoc)
{
	m_pDoc =pDoc;
	m_status = ERROR_SUCCESS;
	m_SessionHandle = 0;
	m_pSessionProperties = NULL;
	m_netTotalIO =0;
	m_totlaAvgLength = 1;

}
CEtw::~CEtw(void)
{
}
void CEtw::SetUp()
{
	StartTraceKernelLogger();
	OpenTraceKernelLogger();
	m_etwThreadHandle = CreateThread(0, 0, Win32TracingThread, 0, 0, &m_etwThreadID);
}
void CEtw::CleanUp()
{
	CloseHandle(m_etwThreadHandle);
	if (m_SessionHandle)
	{
		m_status = ControlTrace(m_SessionHandle, KERNEL_LOGGER_NAME, m_pSessionProperties, EVENT_TRACE_CONTROL_STOP);

		if (ERROR_SUCCESS != m_status)
		{
			wprintf(L"ControlTrace(stop) failed with %lu\n", m_status);
		}
	}
	CloseTrace(m_SessionHandle);
	if (m_pSessionProperties)
		free(m_pSessionProperties);
}
void CEtw::StartTraceKernelLogger()
{
	ULONG BufferSize = 0;

	// Allocate memory for the session properties. The memory must
	// be large enough to include the log file name and session name,
	// which get appended to the end of the session properties structure.

	BufferSize = sizeof(EVENT_TRACE_PROPERTIES) + sizeof(KERNEL_LOGGER_NAME);
	m_pSessionProperties = (EVENT_TRACE_PROPERTIES*)malloc(BufferSize);
	if (NULL == m_pSessionProperties)
	{
		wprintf(L"Unable to allocate %d bytes for properties structure.\n", BufferSize);
		CleanUp();
	}

	// Set the session properties. You only append the log file name
	// to the properties structure; the StartTrace function appends
	// the session name for you.

	ZeroMemory(m_pSessionProperties, BufferSize);
	m_pSessionProperties->Wnode.BufferSize = BufferSize;
	m_pSessionProperties->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
	m_pSessionProperties->Wnode.ClientContext = 1; //QPC clock resolution
	m_pSessionProperties->Wnode.Guid = SystemTraceControlGuid_I;
	m_pSessionProperties->EnableFlags = EVENT_TRACE_FLAG_DISK_IO | EVENT_TRACE_FLAG_DISK_FILE_IO | EVENT_TRACE_FLAG_NETWORK_TCPIP;
	m_pSessionProperties->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
	m_pSessionProperties->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);
	StringCbCopy((LPWSTR)((char*)m_pSessionProperties + m_pSessionProperties->LoggerNameOffset), sizeof(KERNEL_LOGGER_NAME), KERNEL_LOGGER_NAME);

	// Create the trace session.


	//m_status = ControlTrace(0, KERNEL_LOGGER_NAME, m_pSessionProperties, EVENT_TRACE_CONTROL_STOP);

	m_status = StartTrace((PTRACEHANDLE)&m_SessionHandle, KERNEL_LOGGER_NAME, m_pSessionProperties);

	if (ERROR_SUCCESS != m_status)
	{
		if (ERROR_ALREADY_EXISTS == m_status)
		{
			wprintf(L"The NT Kernel Logger session is already in use.\n");
		}
		else
		{
			wprintf(L"EnableTrace() failed with %lu\n", m_status);
			CleanUp();
		}
	}

}
void CEtw::OpenTraceKernelLogger()
{

	EVENT_TRACE_LOGFILE trace;
	TRACE_LOGFILE_HEADER* pHeader = &trace.LogfileHeader;
	ZeroMemory(&trace, sizeof(EVENT_TRACE_LOGFILE));
	trace.LoggerName = KERNEL_LOGGER_NAME;
	trace.LogFileName = (LPWSTR)NULL;
	//trace.Context = this; // passes to EventRecordCallback, but only works in Vista+
	trace.EventRecordCallback = (PEVENT_RECORD_CALLBACK)(StaticRecordEventCallback);
	//trace.BufferCallback = (PEVENT_TRACE_BUFFER_CALLBACK)(StaticBufferEventCallback);
	trace.ProcessTraceMode = (PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD | PROCESS_TRACE_MODE_RAW_TIMESTAMP);

	// Open Trace

	m_SessionHandle = OpenTrace(&trace);
	if (0x00000000FFFFFFFF   == m_SessionHandle)
	{
		DWORD err = GetLastError();
		wprintf(L"PacketTraceSession: OpenTrace() failed with %lu\n", err);	// lookup in winerror.h
		CleanUp();
	}

}
static VOID WINAPI StaticRecordEventCallback(PEVENT_RECORD pEvent)
{
	if (IsEqualGUID(pEvent->EventHeader.ProviderId, DiskIoGuid_I))
	{
		ETW_DISK_EVENT diskEvent;

		memset(&diskEvent, 0, sizeof(ETW_DISK_EVENT));
		diskEvent.Type = EtwMax;


		switch (pEvent->EventHeader.EventDescriptor.Opcode)
		{
		case EVENT_TRACE_TYPE_IO_READ: // send
			diskEvent.Type = EtwDiskReadType;
			//diskReadMap[pEvent->EventHeader.ProcessId] = *data;
			break;
		case EVENT_TRACE_TYPE_IO_WRITE: // receive
			diskEvent.Type = EtwDiskWriteType;
			//diskWriteMap[pEvent->EventHeader.ProcessId] = data->TransferSize;
			break;
		}
		if (diskEvent.Type != EtwMax)
		{
			DiskIo_TypeGroup1 *data = (DiskIo_TypeGroup1 *)pEvent->UserData;
			if (pEvent->EventHeader.ProcessId != ULONG_MAX)
			{
				diskEvent.ProcessHandle = UlongToHandle(pEvent->EventHeader.ProcessId);
				diskEvent.ProcessID = pEvent->EventHeader.ProcessId;
			}
			diskEvent.IrpFlags = data->IrpFlags;
			diskEvent.TransferSize = data->TransferSize;
			diskEvent.FileObject = (PVOID)data->FileObject;
			diskEvent.HighResResponseTime = data->HighResResponseTime;

			ProcessDiskEvent(diskEvent);
		}

	}
	else if(IsEqualGUID(pEvent->EventHeader.ProviderId, TcpIpGuid_I)
		|| IsEqualGUID(pEvent->EventHeader.ProviderId, UdpIpGuid_I))
	{
		ETW_NETWORK_EVENT networkEvent;

		memset(&networkEvent, 0, sizeof(ETW_NETWORK_EVENT));
		networkEvent.Type = EtwMax;

		switch (pEvent->EventHeader.EventDescriptor.Opcode)
		{
		case EVENT_TRACE_TYPE_SEND: // send
			networkEvent.Type = EtwNetworkSendType;
			networkEvent.ProtocolType = IPV4_NETWORK_TYPE;
			break;
		case EVENT_TRACE_TYPE_RECEIVE: // receive
			networkEvent.Type = EtwNetworkReceiveType;
			networkEvent.ProtocolType = IPV4_NETWORK_TYPE;
			break;
		case EVENT_TRACE_TYPE_SEND + 16: // send ipv6	
			networkEvent.Type = EtwNetworkSendType;
			networkEvent.ProtocolType = IPV6_NETWORK_TYPE;
			break;
		case EVENT_TRACE_TYPE_RECEIVE + 16: // receive ipv6
			networkEvent.Type = EtwNetworkReceiveType;
			networkEvent.ProtocolType = IPV6_NETWORK_TYPE;
			break;
		case EVENT_TRACE_TYPE_DISCONNECT:
			networkEvent.Type = EtwNetworkDisconnectType;
			break;

		default:
			break;
		}
		if (IsEqualGUID(pEvent->EventHeader.ProviderId, TcpIpGuid_I))
			networkEvent.ProtocolType |= TCP_PROTOCOL_TYPE;
		else
			networkEvent.ProtocolType |= UDP_PROTOCOL_TYPE;
		if (networkEvent.Type != ULONG_MAX)
		{
			if (networkEvent.ProtocolType & IPV4_NETWORK_TYPE)
			{
				TcpIpOrUdpIp_IPV4_Header *data = (TcpIpOrUdpIp_IPV4_Header *)pEvent->UserData;
				networkEvent.ProcessHandle = UlongToHandle(data->PID);
				networkEvent.ProcessID = data->PID;
				networkEvent.TransferSize = data->size;

			}
			else if(networkEvent.ProtocolType & IPV6_NETWORK_TYPE)
			{
				TcpIpOrUdpIp_IPV6_Header *data = (TcpIpOrUdpIp_IPV6_Header *)pEvent->UserData;
				networkEvent.ProcessHandle = UlongToHandle(data->PID);
				networkEvent.ProcessID = data->PID;
				networkEvent.TransferSize = data->size;
			}
			ProcessNetworkEvent(networkEvent);

		}
		
	}
}
static DWORD WINAPI Win32TracingThread(LPVOID Parameter)
{
	ProcessTrace(&m_SessionHandle, 1, 0, 0);
	return(0);
}

void ProcessDiskEvent(const ETW_DISK_EVENT& Event)
{
	map<ULONG, ProcessDiskData>* map = &CEtw::diskMapRealTime;
	if(map->count(Event.ProcessID) == 0)
	{
		(*map)[Event.ProcessID] = ProcessDiskData(0,0);
	}

	if(Event.Type == EtwDiskReadType)
	{
		(*map)[Event.ProcessID].readBtyes += Event.TransferSize;
	}
	else if(Event.Type == EtwDiskWriteType)
	{
		(*map)[Event.ProcessID].writeBytes += Event.TransferSize;
	}

	return;
}
void ProcessNetworkEvent(const ETW_NETWORK_EVENT& Event)
{

	map<ULONG, ProcessNetworkData>* map = &CEtw::networkMapRealTIme;
	if(map->count(Event.ProcessID) == 0)
	{
		(*map)[Event.ProcessID] = ProcessNetworkData(0,0);
	}

	if(Event.Type == EtwNetworkReceiveType)
	{
		(*map)[Event.ProcessID].receiveBytes += Event.TransferSize;
	}
	else if(Event.Type == EtwNetworkSendType)
	{
		(*map)[Event.ProcessID].sendBytes += Event.TransferSize;
	}
	return;
}

void CEtw::Update()
{
	UpdateDisk();
	UpdateNetwork();
}
void CEtw::UpdateDisk()
{
	for(map<ULONG, ProcessDiskData>::iterator it = diskMapRealTime.begin(); it != diskMapRealTime.end(); it ++)
	{
		ProcessDiskDataQ popData;
		popData.readBytes = 0;
		popData.writeBytes = 0;
		ULONG id = it->first;

		if(diskMap.count(id) == 0)
		{
			diskMap[id] = ProcessDiskData(0,0);
		}

		if(diskQue.count(id) == 0)
		{
			queue<ProcessDiskDataQ> q;
			q.push(ProcessDiskDataQ(it->second.readBtyes,it->second.writeBytes));
			diskQue[id] = q;
		}else
		{
			if(diskQue[id].size() >= AVG_CALC_COUNT)
			{
				popData = diskQue[id].front();
				diskQue[id].pop();
				
			}
			diskQue[id].push(ProcessDiskDataQ(it->second.readBtyes,it->second.writeBytes));
			
		}
		if(!(it->second.readBtyes == 0 && it->second.writeBytes == 0))
		{
			diskMap[id].outCount = AVG_CALC_COUNT;
		}else
		{
			diskMap[id].outCount --;
		}
		if(diskMap[id].averageLength < AVG_CALC_COUNT)
		{
			diskMap[id].averageLength++;
		}
		diskMap[id].readBtyes -= popData.readBytes;
		diskMap[id].readBtyes += it->second.readBtyes;
		diskMap[id].writeBytes -= popData.writeBytes;
		diskMap[id].writeBytes += it->second.writeBytes;

		it->second.readBtyes = 0;
		it->second.writeBytes = 0;
		
	}

	FindDiskOutProc();
}
void CEtw::UpdateNetwork()
{
	//큐에 업데이트마다 데이터 넣음
	//큐에 들어있는 데이터 기반으로 평균 구해서 Map에 넣음
	m_netTotalIO = 0;
	for(map<ULONG, ProcessNetworkData>::iterator it = networkMapRealTIme.begin(); it != networkMapRealTIme.end(); it ++)
	{
		ProcessNetworkDataQ popData;
		popData.receiveBytes = 0;
		popData.sendBytes = 0;
		ULONG id = it->first;

		if(networkMap.count(id) == 0)
		{
			networkMap[id] = ProcessNetworkData(0,0);
		}

		if(networkQue.count(id) == 0)
		{
			queue<ProcessNetworkDataQ> q;
			q.push(ProcessNetworkDataQ(it->second.receiveBytes,it->second.sendBytes));
			networkQue[id] = q;
		}else
		{
			if(networkQue[id].size() >= AVG_CALC_COUNT)
			{
				popData = networkQue[id].front();
				networkQue[id].pop();
				
			}
			networkQue[id].push(ProcessNetworkDataQ(it->second.receiveBytes,it->second.sendBytes));
			
		}
		if(!(it->second.receiveBytes == 0 && it->second.sendBytes == 0))
		{
			networkMap[id].outCount = AVG_CALC_COUNT;
		}else
		{
			networkMap[id].outCount --;
		}
		if(networkMap[id].averageLength < AVG_CALC_COUNT)
		{
			networkMap[id].averageLength++;
		}
		networkMap[id].receiveBytes -= popData.receiveBytes;
		networkMap[id].receiveBytes += it->second.receiveBytes;
		networkMap[id].sendBytes -= popData.sendBytes;
		networkMap[id].sendBytes += it->second.sendBytes;

		m_netTotalIO += networkMap[id].receiveBytes;
		m_netTotalIO += networkMap[id].sendBytes;

		it->second.receiveBytes = 0;
		it->second.sendBytes = 0;
		
	}
	if(m_totlaAvgLength < AVG_CALC_COUNT)
	{
		m_totlaAvgLength ++;
	}
	FindNetworkOutProc();
	
}
void CEtw::FindNetworkOutProc()
{
	for(map<ULONG, ProcessNetworkData>::iterator it = networkMap.begin(); it != networkMap.end(); it ++)
	{
		if(it->second.outCount <= 0)
		{
			networkOutList.push_back(it->first);
		}
	}
	for(vector<ULONG>::iterator it = networkOutList.begin(); it != networkOutList.end(); it++)
	{
		networkMapRealTIme.erase(*it);
		networkMap.erase(*it);
		networkQue.erase(*it);
	}
	if(networkOutList.size() > 0)
	{
		m_pDoc->AtNetworkOut(&networkOutList);
	}
	networkOutList.clear();
}
void CEtw::FindDiskOutProc()
{
	for(map<ULONG, ProcessDiskData>::iterator it = diskMap.begin(); it != diskMap.end(); it ++)
	{
		if(it->second.outCount == 0)
		{
			diskOutList.push_back(it->first);
		}
	}
	for(vector<ULONG>::iterator it = diskOutList.begin(); it != diskOutList.end(); it++)
	{
		diskMap.erase(*it);
		diskMapRealTime.erase(*it);
		diskQue.erase(*it);
	}
	if(diskOutList.size() > 0)
	{
		m_pDoc->AtDiskOut(&diskOutList);
	}
	diskOutList.clear();
}