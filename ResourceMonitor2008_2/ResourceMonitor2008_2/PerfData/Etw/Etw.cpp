
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
#include <tlhelp32.h>  


using namespace std;

static TRACEHANDLE		m_SessionHandle;
static VOID WINAPI StaticRecordEventCallback(PEVENT_RECORD pEvent);
static DWORD WINAPI Win32TracingThread(LPVOID Parameter);
EVENT_TRACE_PROPERTIES* m_pSessionProperties;
BOOLEAN EtEtwEnabled = FALSE;
BOOLEAN EtpStartedSession = FALSE;

void ProcessDiskEvent(const ETW_DISK_EVENT& Event);
void ProcessNetworkEvent(const ETW_NETWORK_EVENT& Event);

map<ULONG, ProcessDiskData> CEtw::diskMapRealTime;
map<ULONG, ProcessNetworkData> CEtw::networkMapRealTIme;

 #define STATUS_INFO_LENGTH_MISMATCH 0xC0000004  
#define XGetPtr(base, offset) ((PVOID)((ULONG_PTR) (base) + (ULONG_PTR) (offset)))  
#define SYSTEM_PROCESS_ID ((HANDLE)4)
#define PH_NEXT_PROCESS(Process) ( \
    ((PRSYSTEM_PROCESS_INFORMATION)(Process))->NextEntryOffset ? \
    (PRSYSTEM_PROCESS_INFORMATION)PTR_ADD_OFFSET((Process), \
    ((PRSYSTEM_PROCESS_INFORMATION)(Process))->NextEntryOffset) : \
    NULL \
    )
#define PTR_ADD_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) + (ULONG_PTR)(Offset)))
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

typedef   
NTSTATUS   
(WINAPI *NtQuerySystemInformationT)  
(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);



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

	if (m_status == ERROR_SUCCESS)
    {
        EtEtwEnabled = TRUE;
        EtpStartedSession = TRUE;
    }
    else if (m_status == ERROR_ALREADY_EXISTS)
    {
        EtEtwEnabled = TRUE;

        EtpStartedSession = FALSE;

    }
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
// ToolHelper Usage
//ULONG EtThreadIdToProcessId(_In_ ULONG ThreadId)
//{
//	static HANDLE snapshot;  
//    THREADENTRY32 te32;  
//    ULONGLONG tickCount;
//	static ULONGLONG lastTickTotal = 0;
//
//	tickCount = GetTickCount();
//
//	if (tickCount - lastTickTotal >= 2 * 1000)
//	{
//		lastTickTotal = tickCount;
//		CloseHandle(snapshot);
//		snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);  
//		if(snapshot == INVALID_HANDLE_VALUE)  
//			return -1; 
//	}
//  
//    te32.dwSize = sizeof(te32);  
//    if(!Thread32First(snapshot, &te32))  
//    {  
//        CloseHandle(snapshot);  
//        return -1;  
//    }  
//  
//    do  
//    {  
//        try  
//        {  
//			if(ThreadId == te32.th32ThreadID)
//			{	
//				return te32.th32OwnerProcessID;
//			}
//  
//        }  
//        catch(...)  
//        {  
//            CloseHandle(snapshot);  
//            return -1;  
//        }  
//  
//    } while(Thread32Next(snapshot, &te32));  
//  
//    //CloseHandle(snapshot);  
//    return 4;  //system process id
//
//
//}

//USE NtQuerySystemInformation
LONG PhEnumProcesses(
    _Out_ PUCHAR *Processes
    )
{
	HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");  
    NtQuerySystemInformationT pNTQSI;  
    pNTQSI = (NtQuerySystemInformationT)   
            GetProcAddress(ntdll, "NtQuerySystemInformation"); 

	static ULONG initialBufferSize =0x4000; 
    LONG status;
    ULONG classIndex;
    PUCHAR  buffer;
    ULONG bufferSize;


    bufferSize = initialBufferSize;
    buffer = new UCHAR[bufferSize];

    while (TRUE)
    {
        status = pNTQSI(SystemProcessInformation,buffer,bufferSize,&bufferSize);
	/*	NtQuerySystemInformation(
            SystemProcessInformation,
            buffer,
            bufferSize,
            &bufferSize
            );*/

        if (status == 0xC0000023L || status == 0xC0000004L) // STATUS_BUFFER_TOO_SMALL || STATUS_INFO_LENGTH_MISMATCH
        {
            delete [] buffer;
            buffer =  new UCHAR[bufferSize];
        }
        else
        {
            break;
        }
    }

    if (!(status >= 0))
    {
        delete [] buffer;
        return status;
    }

    if (bufferSize <= 0x100000) initialBufferSize = bufferSize;
    *Processes = buffer;

    return status;
}
HANDLE EtThreadIdToProcessId(
    _In_ HANDLE ThreadId
    )
{
    // Note: no lock is needed because we use the list on the same thread (EtpEtwMonitorThreadStart). (dmex)
   
	static PUCHAR processInfo = NULL;
    static ULONG64 lastTickTotal = 0;
    PRSYSTEM_PROCESS_INFORMATION  process;
    ULONG64 tickCount;

    tickCount = GetTickCount();

    if (tickCount - lastTickTotal >= 2 * CLOCKS_PER_SEC)
    {
        lastTickTotal = tickCount;

        if (processInfo)
        {
            delete [] processInfo;
            processInfo = NULL;
        }

        PhEnumProcesses(&processInfo);
    }

    process = (PRSYSTEM_PROCESS_INFORMATION)processInfo;

	do
    {
		//PRSYSTEM_THREAD_INFORMATION threads;  
  //      threads = (PRSYSTEM_THREAD_INFORMATION) XGetPtr(process, sizeof(*process));
        for (ULONG i = 0; i < process->NumberOfThreads; i++)
        {
            //if (threads[i].ClientId.UniqueThread == ThreadId)
			if (process->Threads[i].ClientId.UniqueThread == ThreadId)
			{
                return process->UniqueProcessId;
            }
        }
		//process = (PRSYSTEM_PROCESS_INFORMATION) XGetPtr(process, process->NextEntryOffset);
	}  while (process = PH_NEXT_PROCESS(process));

    return SYSTEM_PROCESS_ID;
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
			break;
		case EVENT_TRACE_TYPE_IO_WRITE: // receive
			diskEvent.Type = EtwDiskWriteType;
			break;
		}
		if (diskEvent.Type != EtwMax)
		{

			DiskIo_TypeGroup1 *data = (DiskIo_TypeGroup1 *)pEvent->UserData;
			if(IS_WINVERSION_UPPER_8)
			{

				//diskEvent.ClientId.UniqueProcess= GetProcessIdOfThread(OpenThread(THREAD_QUERY_INFORMATION| THREAD_QUERY_LIMITED_INFORMATION ,TRUE,data->IssuingThreadId));

				//NtQuerySystemInformation으로 프로세스의 스레드 목록 얻기
				diskEvent.ClientId.UniqueThread = UlongToHandle(data->IssuingThreadId);
				diskEvent.ClientId.UniqueProcess =EtThreadIdToProcessId(diskEvent.ClientId.UniqueThread);
			}else
			{
				if (pEvent->EventHeader.ProcessId != ULONG_MAX)
				{
					diskEvent.ClientId.UniqueProcess = UlongToHandle(pEvent->EventHeader.ProcessId);
					//diskEvent.ClientId.UniqueProcess = pEvent->EventHeader.ProcessId;
				}
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
	ULONG processID = (ULONG)Event.ClientId.UniqueProcess;
	//processID = GetProcessId(Event.ClientId.UniqueProcess);
	//TRACE("ProcessID : %d, UniqueProcess : %d \n",processID, Event.ClientId.UniqueProcess);
	if(map->count(processID) == 0)
	{
		(*map)[processID] = ProcessDiskData(0,0);
	}

	if(Event.Type == EtwDiskReadType)
	{
		(*map)[processID].readBtyes += Event.TransferSize;
	}
	else if(Event.Type == EtwDiskWriteType)
	{
		(*map)[processID].writeBytes += Event.TransferSize;
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
	//Flush EtwSession
	//if (m_SessionHandle != (TRACEHANDLE)INVALID_HANDLE_VALUE)
	//{
	//	if (EtEtwEnabled)
	//	{
	//		m_pSessionProperties->LogFileNameOffset = 0; // make sure it is 0, otherwise ControlTrace crashes

	//		ControlTrace(
	//			EtpStartedSession ? m_SessionHandle : 0,
	//			EtpStartedSession ? NULL : KERNEL_LOGGER_NAME,
	//			m_pSessionProperties,
	//			EVENT_TRACE_CONTROL_FLUSH
	//			);
	//	}

	//}

	//Update
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