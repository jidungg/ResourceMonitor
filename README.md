# ResourceMonitor

## Description  
시스템 자원 모니터링이 필요한데 윈도우 기본 프로그램인 작업관리자, 리소스모니터 등에는 로그를 남기는 기능이 없었기 때문에 자체적으로 제작하게 되었다.
따라서 이 프로그램은 기존 윈도우 기본 프로그램 리소스모니터 에서 로그 기능을 추가한 형태가 될 것이다.  


## Environment
기본적으로 윈도우 10, Visual Studio 2015/2008,  c++ 환경에서 개발했다. (2021-12-08기준 2008에서만 개발중, 2015는 중단)  
GUI는 MFC를 통해 구현했으며, Resource Data는 WMI, ETW를 사용해 개발했다.  
실행 환경은 윈도우 7 으로 상정하였음.

## Files
* View Files :  
MFC CScrollView 를 상속받는 파일들.
    * ResourceMonitorView :  
    아래 4개의 View 파일들의 부모 클래스. 
    * CPUMonitorView :  
    CPU에 관련된 데이터를 보여주는 뷰 클래스.
    * MemoryMonitorView :  
    메모리에 관련된 데이터를 보여주는 뷰 클래스.
    * DiskMoniotrView :  
    디스크에 관련된 데이터를 보여주는 뷰 클래스.
    * NetworkMonitorView :  
    네트워크에 관련된 데이터를 보여주는 뷰 클래스.
* PerfData Files :  
WMI를 사용해 성능 데이터를 가져오는 파일들.
    * PerfDataManager :  
    성능 데이터 클래스를 관리하고, Doc과 통신하며 성능 데이터를 제공하는 클래스.
    * PerfData :
    성능 데이터 클래스의 부모 클래스. 가상 클래스이다.
    * PerfDataPerProcess :
    프로세스 성능 데이터 클래스.
    * PerfDataOS :
    운영체제 성능 데이터 클래스.
    * PerfDataOSProcessor :
    운영체제 성능 데이터 클래스. CPU 총 사용량.  
ETW를 사용해 성능 데이터를 가져오는 파일.
    * Etw :
     Etw 사용. 디스크, 네트워크 성능 데이터 클래스.
* Basic Files :  
기본 파일
    * ResourceMonitor:  
    앱 파일, main함수 존재.
    * ResourceMonitorDoc :  
    도큐먼트 클래스.
    * MainFrm :  
    메인 프레임 클래스.
    * Version_Dialog :  
    Version 메뉴 클릭시 출력되는 다이얼로그. Version 변경 이력을 표시함.
* Logger Files :  
로그 기능을 구현한 파일들.

## Usage
단순히 exe파일을 실행시키면 된다. 
CPU, Memory, Disk, Network 정보를 보여주고, 로그를 디스크에 기록한다.
Log 메뉴를 통해 로그 출력 주기(Log Interval), 로그 출력 조건(Log threshold), 로그파일 생성 경로(Log path) 를 설정할 수 있다.  
설정한 로그 주기(ms)마다 설정된 파일 경로(설정한 적 없다면 [D:\Performance_data_Log\{날짜}]) 에 기록한다. 
설정한 로그 출력 조건에 따라 설정 값 이상의 데이터만 로그로 남긴다.

## Caution
현재(2022-01-26 기준) Windows 7은 모든 기능 정상 작동 중. Windows 10에서는 Disk 데이터 표시가 정확하지 않음. 
현재 2008 버전만 업데이트 중. 2015버전은 잠정 중단.  

실행화면  
![실행화면](https://github.com/jidungg/ResourceMonitor/blob/master/img/v2.0.0.PNG?raw=true)
![로그설정](https://github.com/jidungg/ResourceMonitor/blob/master/img/v1.2.0.PNG?raw=true)

* * *  


2022-01-07 v2.1.0 (제목목록.목록.내용)


