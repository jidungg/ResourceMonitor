#include "stdAfx.h"
#include "NetworkPerformanceItem.h"

NetworkPerformanceItem::NetworkPerformanceItem(void)
{
	Pass = 0;
	BytesOut = 0;
	BytesIn = 0;
	liveFlag = true;
}

NetworkPerformanceItem::~NetworkPerformanceItem(void)
{
}
