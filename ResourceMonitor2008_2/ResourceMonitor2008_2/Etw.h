#pragma once
//#include <evntrace.h>
#define INITGUID
#include <map>
#include <vector>

static GUID SystemTraceControlGuid_I = { 0x9e814aad, 0x3204, 0x11d2,{ 0x9a, 0x82, 0x00, 0x60, 0x08, 0xa8, 0x69, 0x39 } };
static GUID DiskIoGuid_I = { 0x3d6fa8d4, 0xfe05, 0x11d0,{ 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c } };
static GUID FileIoGuid_I = { 0x90cbdc39, 0x4a3e, 0x11d1,{ 0x84, 0xf4, 0x00, 0x00, 0xf8, 0x04, 0x64, 0xe3 } };
static GUID TcpIpGuid_I = { 0x9a280ac0, 0xc8e0, 0x11d1,{ 0x84, 0xe2, 0x00, 0xc0, 0x4f, 0xb9, 0x98, 0xa2 } };
static GUID UdpIpGuid_I = { 0xbf3a50c5, 0xa9c9, 0x4988,{ 0xa0, 0x05, 0x2d, 0xf0, 0xb7, 0xc8, 0x0f, 0x80 } };

class CResourceMonitorDoc;

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
	HANDLE ProcessHandle;
	ULONG ProcessID;
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
	int avgOutCount;
	ProcessDiskData(){;}
	ProcessDiskData(int r, int w)
	{
		readBtyes = r;
		writeBytes = w;
		averageLength = 1;
		avgOutCount = AVG_CALC_COUNT;
	}
};
struct ProcessNetworkData
{
	ULONG receiveBytes;
	ULONG sendBytes;
	int	averageLength;
	int avgOutCount;
	ProcessNetworkData(){;}
	ProcessNetworkData(int r, int s)
	{
		receiveBytes = r;
		sendBytes = s;
		averageLength = 1;
		avgOutCount = AVG_CALC_COUNT;
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
	int CumulativeAverage(int length, int prevAvg, int newNumber);
	void Update();
	void UpdateDisk();
	void UpdateNetwork();
	void FindNetworkOutProc();
	void FIndDiskOutProc();

	CResourceMonitorDoc		*m_pDoc ;
	ULONG					m_status;
	DWORD					m_etwThreadID;
	HANDLE					m_etwThreadHandle;

	ULONG m_totalIO;
	int m_totlaAvgLength;
	std::vector<ULONG> networkOutList;
	std::vector<ULONG> diskOutList;
	std::map<ULONG, ProcessDiskData> diskMap;
	std::map<ULONG, ProcessNetworkData> networkMap;
	static std::map<ULONG, ProcessDiskData> diskMapRealTime;
	static std::map<ULONG, ProcessNetworkData> networkMapRealTIme;
};