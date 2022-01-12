#pragma once
#include <map>
#include <vector>
#include <queue>

struct EtwProcessData
{
	ULONG readBtyes;
	ULONG writeBytes;
	int	averageLength;
	int outCount;
	EtwProcessData(){;}
	EtwProcessData(ULONG r, ULONG w)
	{
		readBtyes = r;
		writeBytes = w;
		averageLength = 1;
		outCount = AVG_CALC_COUNT;
	}
};
struct EtwProcessDataQ
{
	ULONG readBytes;
	ULONG writeBytes;

	EtwProcessDataQ(){;}
	EtwProcessDataQ(ULONG r, ULONG s)
	{
		readBytes = r;
		writeBytes = s;
	}
};
class CResourceMonitorDoc;
typedef void(*processOutFunc)(std::vector<ULONG>*);
class EtwData
{
public:
	EtwData(void);
//	virtual EtwData(CResourceMonitorDoc*) abstract;
	virtual ~EtwData(void);

	virtual void Update();
	virtual void FindOutProc();

	processOutFunc m_processOutFunc;
	CResourceMonitorDoc		*m_pDoc ;

	std::map<ULONG, EtwProcessData> m_map;
	std::map<ULONG, std::queue<EtwProcessDataQ>> m_que;
	std::vector<ULONG> m_outList;
	static std::map<ULONG, EtwProcessData> m_mapRealTime;
};
