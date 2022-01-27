#pragma once
#define VERSION "v3.0.1" //22-01-12 Update Interval interpolate added. File struecture reorganized. Session restart problem solved.

#define FRAME_WIDTH 55
#define FRAME_WIDTH_ITEM 30

#define UPDATE_INTERVAL 1000
#define LOG_INTERVAL 1000

#define UPDATE_TIMER 1000
#define LOG_TIMER 1001

#define LOG_CPU_THRESHOLD 10.0f
#define LOG_MEM_THRESHOLD 1048576 //1GB
#define LOG_NET_THRESHOLD 10240
#define LOG_DISK_THRESHOLD 10240
#define LOG_DIRECTORY _T("D:\\Performance_data_Log")


#define LOG_TOP_N 10

#define AVG_CALC_COUNT 30
