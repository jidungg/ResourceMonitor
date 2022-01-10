#pragma once
#define VERSION "v2.1.0" //21-12-27 PerfDataOSProcessor 클래스 추가. Window7 프로세스 종료문제 해결

#define FRAME_WIDTH 55
#define FRAME_WIDTH_ITEM 30

#define UPDATE_INTERVAL 1000
#define LOG_INTERVAL 1000

#define LOG_CPU_THRESHOLD 10.0f
#define LOG_MEM_THRESHOLD 1048576 //1GB
#define LOG_DIRECTORY _T("D:\\Performance_data_Log")

#define LOG_TOP_N 10

#define AVG_CALC_COUNT 60
