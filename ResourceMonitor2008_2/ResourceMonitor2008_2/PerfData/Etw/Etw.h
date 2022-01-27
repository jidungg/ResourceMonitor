#pragma once
//#include <evntrace.h>
#define INITGUID
#include <map>
#include <vector>
#include <queue>
#include <winternl.h> 


static GUID SystemTraceControlGuid_I = { 0x9e814aad, 0x3204, 0x11d2,{ 0x9a, 0x82, 0x00, 0x60, 0x08, 0xa8, 0x69, 0x39 } };
static GUID DiskIoGuid_I = { 0x3d6fa8d4, 0xfe05, 0x11d0,{ 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c } };
static GUID FileIoGuid_I = { 0x90cbdc39, 0x4a3e, 0x11d1,{ 0x84, 0xf4, 0x00, 0x00, 0xf8, 0x04, 0x64, 0xe3 } };
static GUID TcpIpGuid_I = { 0x9a280ac0, 0xc8e0, 0x11d1,{ 0x84, 0xe2, 0x00, 0xc0, 0x4f, 0xb9, 0x98, 0xa2 } };
static GUID UdpIpGuid_I = { 0xbf3a50c5, 0xa9c9, 0x4988,{ 0xa0, 0x05, 0x2d, 0xf0, 0xb7, 0xc8, 0x0f, 0x80 } };

class CResourceMonitorDoc;
typedef struct _CLIENT_ID
{
    //ULONG UniqueProcess;
    //ULONG UniqueThread;
	HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef enum _KWAIT_REASON
{
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    WrKeyedEvent,
    WrTerminated,
    WrProcessInSwap,
    WrCpuRateControl,
    WrCalloutStack,
    WrKernel,
    WrResource,
    WrPushLock,
    WrMutex,
    WrQuantumEnd,
    WrDispatchInt,
    WrPreempted,
    WrYieldExecution,
    WrFastMutex,
    WrGuardedMutex,
    WrRundown,
    WrAlertByThreadId,
    WrDeferredPreempt,
    WrPhysicalFault,
    MaximumWaitReason
} KWAIT_REASON, *PKWAIT_REASON;
typedef enum _KTHREAD_STATE
{
    Initialized,
    Ready,
    Running,
    Standby,
    Terminated,
    Waiting,
    Transition,
    DeferredReady,
    GateWaitObsolete,
    WaitingForProcessInSwap,
    MaximumThreadState
} KTHREAD_STATE, *PKTHREAD_STATE;
typedef struct _RSYSTEM_THREAD_INFORMATION
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    LONG Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    KTHREAD_STATE ThreadState;
    KWAIT_REASON WaitReason;
} RSYSTEM_THREAD_INFORMATION, *PRSYSTEM_THREAD_INFORMATION;

typedef struct _RSYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize; // since VISTA
    ULONG HardFaultCount; // since WIN7
    ULONG NumberOfThreadsHighWatermark; // since WIN7
    ULONGLONG CycleTime; // since WIN7
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    LONG BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey; // since VISTA (requires SystemExtendedProcessInformation)
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    RSYSTEM_THREAD_INFORMATION Threads[1]; // SystemProcessInformation
    // SYSTEM_EXTENDED_THREAD_INFORMATION Threads[1]; // SystemExtendedProcessinformation
    // SYSTEM_EXTENDED_THREAD_INFORMATION + SYSTEM_PROCESS_INFORMATION_EXTENSION // SystemFullProcessInformation
} RSYSTEM_PROCESS_INFORMATION, *PRSYSTEM_PROCESS_INFORMATION;





typedef struct
{
	ULONG DiskNumber;
	ULONG IrpFlags;
	ULONG TransferSize;
	ULONG ResponseTime;
	ULONG64 ByteOffset;
	ULONG_PTR FileObject;
	ULONG_PTR Irp;
	ULONG64 HighResResponseTime;
	ULONG IssuingThreadId; // since WIN8 (ETW_DISKIO_READWRITE_V3)
} DiskIo_TypeGroup1;
typedef struct
{
	ULONG PID;
	ULONG size;
	//IN_ADDR daddr;
	//IN_ADDR saddr;
	//USHORT dport;
	//USHORT sport;
} TcpIpOrUdpIp_IPV4_Header;

typedef struct
{
	ULONG PID;
	ULONG size;
	//IN6_ADDR daddr;
	//IN6_ADDR saddr;
	//USHORT dport;
	//USHORT sport;
} TcpIpOrUdpIp_IPV6_Header;

typedef enum _ETW_EVENT_TYPE
{
	EtwDiskReadType = 1,
	EtwDiskWriteType,
	EtwFileNameType,
	EtwFileCreateType,
	EtwFileDeleteType,
	EtwFileRundownType,
	EtwNetworkReceiveType,
	EtwNetworkSendType,
	EtwNetworkDisconnectType,
	EtwMax
} ETW_EVENT_TYPE;
typedef enum _NETWORK_PROTOCOL_TYPE
{
	IPV4_NETWORK_TYPE = 0x1,
	IPV6_NETWORK_TYPE = 0x2,
	TCP_PROTOCOL_TYPE = 0x10,
	UDP_PROTOCOL_TYPE = 0x20
} NETWORK_PROTOCOL_TYPE;

typedef struct _ETW_DISK_EVENT
{
	ETW_EVENT_TYPE Type;
	CLIENT_ID ClientId;
	ULONG IrpFlags;
	ULONG TransferSize;
	PVOID FileObject;
	ULONG64 HighResResponseTime;
} ETW_DISK_EVENT, *PETW_DISK_EVENT;


typedef struct _ETW_NETWORK_EVENT
{
	_ETW_EVENT_TYPE Type;
	HANDLE ProcessHandle;
	ULONG ProcessID;
	ULONG ProtocolType;
	ULONG TransferSize;
	//PH_IP_ENDPOINT LocalEndpoint;
	//PH_IP_ENDPOINT RemoteEndpoint;
} ETW_NETWORK_EVENT, *PETW_NETWORK_EVENT;

struct ProcessDiskData
{
	ULONG readBtyes;
	ULONG writeBytes;
	int	averageLength;
	int outCount;
	ProcessDiskData(){;}
	ProcessDiskData(ULONG r, ULONG w)
	{
		readBtyes = r;
		writeBytes = w;
		averageLength = 1;
		outCount = AVG_CALC_COUNT;
	}
};
struct ProcessDiskDataQ
{
	ULONG readBytes;
	ULONG writeBytes;

	ProcessDiskDataQ(){;}
	ProcessDiskDataQ(ULONG r, ULONG s)
	{
		readBytes = r;
		writeBytes = s;
	}
};
struct ProcessNetworkData
{
	ULONG receiveBytes;
	ULONG sendBytes;
	int	averageLength;
	int outCount;
	ProcessNetworkData(){;}
	ProcessNetworkData(ULONG r, ULONG s)
	{
		receiveBytes = r;
		sendBytes = s;
		averageLength = 1;
		outCount = AVG_CALC_COUNT;
	}
};
struct ProcessNetworkDataQ
{
	ULONG receiveBytes;
	ULONG sendBytes;

	ProcessNetworkDataQ(){;}
	ProcessNetworkDataQ(ULONG r, ULONG s)
	{
		receiveBytes = r;
		sendBytes = s;
	}
};
class CEtw
{
public:
	CEtw(void);
	CEtw(CResourceMonitorDoc* );
	~CEtw(void);

	void SetUp();
	void CleanUp();
	void StartTraceKernelLogger();
	void OpenTraceKernelLogger();
	void Update();
	void UpdateDisk();
	void UpdateNetwork();
	void FindNetworkOutProc();
	void FindDiskOutProc();

	CResourceMonitorDoc		*m_pDoc ;
	ULONG					m_status;
	DWORD					m_etwThreadID;
	HANDLE					m_etwThreadHandle;

	ULONG m_netTotalIO;
	int m_totlaAvgLength;

	std::map<ULONG, ProcessDiskData> diskMap;
	std::map<ULONG, ProcessNetworkData> networkMap;
	std::map<ULONG, std::queue<ProcessDiskDataQ>> diskQue;
	std::map<ULONG, std::queue<ProcessNetworkDataQ>> networkQue;
	static std::map<ULONG, ProcessDiskData> diskMapRealTime;
	static std::map<ULONG, ProcessNetworkData> networkMapRealTIme;
	std::vector<ULONG> networkOutList;
	std::vector<ULONG> diskOutList;
};
