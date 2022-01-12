#include "stdAfx.h"
#include "EtwData.h"
#include <windows.h>
#include <stdio.h>
#include <wbemidl.h>
#include <conio.h>
#include <strsafe.h>
#include <wmistr.h>
#include <evntrace.h>
#include <evntcons.h>
#include <in6addr.h>
#include "../../ResourceMonitorDoc.h"

using namespace std;
map<ULONG, EtwProcessData> EtwData::m_mapRealTime;

EtwData::EtwData(void)
{
}

EtwData::~EtwData(void)
{
}

void EtwData::Update()
{
	for(map<ULONG, EtwProcessData>::iterator it = m_mapRealTime.begin(); it != m_mapRealTime.end(); it ++)
	{
		EtwProcessDataQ popData;
		popData.readBytes = 0;
		popData.writeBytes = 0;
		ULONG id = it->first;

		if(m_map.count(id) == 0)
		{
			m_map[id] = EtwProcessData(0,0);
		}

		if(m_que.count(id) == 0)
		{
			queue<EtwProcessDataQ> q;
			q.push(EtwProcessDataQ(it->second.readBtyes,it->second.writeBytes));
			m_que[id] = q;
		}else
		{
			if(m_que[id].size() >= AVG_CALC_COUNT)
			{
				popData = m_que[id].front();
				m_que[id].pop();
				
			}
			m_que[id].push(EtwProcessDataQ(it->second.readBtyes,it->second.writeBytes));
			
		}
		if(!(it->second.readBtyes == 0 && it->second.writeBytes == 0))
		{
			m_map[id].outCount = AVG_CALC_COUNT;
		}else
		{
			m_map[id].outCount --;
		}
		if(m_map[id].averageLength < AVG_CALC_COUNT)
		{
			m_map[id].averageLength++;
		}
		m_map[id].readBtyes -= popData.readBytes;
		m_map[id].readBtyes += it->second.readBtyes;
		m_map[id].writeBytes -= popData.writeBytes;
		m_map[id].writeBytes += it->second.writeBytes;

		it->second.readBtyes = 0;
		it->second.writeBytes = 0;
		
	}

	FindOutProc();
}

void EtwData::FindOutProc()
{
	for(map<ULONG, EtwProcessData>::iterator it = m_map.begin(); it != m_map.end(); it ++)
	{
		if(it->second.outCount <= 0)
		{
			m_outList.push_back(it->first);
		}
	}
	for(vector<ULONG>::iterator it = m_outList.begin(); it != m_outList.end(); it++)
	{
		m_mapRealTime.erase(*it);
		m_map.erase(*it);
		m_que.erase(*it);
	}
	if(m_outList.size() > 0)
	{
		m_processOutFunc(&m_outList);
	}
	m_outList.clear();
}