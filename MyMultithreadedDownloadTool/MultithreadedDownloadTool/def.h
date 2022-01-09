#pragma once
#include <WinSock2.h>
#include <Windows.h>

#ifndef _BUILD_INTERNAL
using DownloadTaskID = void*;
#endif

typedef struct {
	const wchar_t* url;
	enum _method_t {
		INVALID = 0, GET, POST, PUT, PATCH, DELETE_
	} method;
	size_t max_thread_count;
	void* pheader;
} DOWNLOADINFOW, *PDOWNLOADINFOW;

typedef struct {
	enum _status_t {
		INVALID = 0, NOT_STARTED, STARTING, STOPPING, RECEIVING, SENDING
	} status;
	size_t total, received;
} DownloadStatus;

typedef struct {
	size_t DownloadCancelMaxWaitTime;  // = 5000
	size_t DownloadMaxThreadCount;     // = 8
} AppConfig_t;


// Exports
#ifndef _BUILD_INTERNAL
//#ifdef __cplusplus
//extern "C" {
//#endif
__declspec(dllimport) DownloadTaskID WINAPI CreateDownloadTask(const char* url);
__declspec(dllimport) DownloadTaskID WINAPI CreateDownloadTaskW(const wchar_t* url);
__declspec(dllimport) DownloadTaskID WINAPI
	CreateDownloadTaskExW(const PDOWNLOADINFOW info);
__declspec(dllimport) bool WINAPI BeginDownloadTask(DownloadTaskID task);
__declspec(dllimport) bool WINAPI PauseDownloadTask(DownloadTaskID task);
__declspec(dllimport) bool WINAPI ResumeDownloadTask(DownloadTaskID task);
__declspec(dllimport) DownloadStatus WINAPI QueryDownloadTaskStat(DownloadTaskID task);
__declspec(dllimport) bool WINAPI CancelDownloadTask(DownloadTaskID task);
__declspec(dllimport) NTSTATUS WINAPI TerminateDownloadTask(DownloadTaskID task);
__declspec(dllimport) const AppConfig_t* WINAPI GetCurrentDownloadConfig();
__declspec(dllimport) void WINAPI ConfigDownload(const AppConfig_t*);

//#ifdef __cplusplus
//}
//#endif
#endif







/* +-----------------------------------------------------+ */
/* | The following definitions are for internal use only | */
/* +-----------------------------------------------------+ */
#if _BUILD_INTERNAL
#include <map>
#include <string>
using namespace std;

// libhv
#include <hv/requests.h>


extern AppConfig_t AppConfig;

DWORD __stdcall thrd_Downloading(void*);
class DownloadTask;
using DownloadTaskID = DownloadTask*;
// DownloadTask class
class DownloadTask {
public:
	DownloadTask() :
		max_thrds(AppConfig.DownloadMaxThreadCount),
		method(methods::INVALID),
		DeleteFlag(false),
		header(0)
	{
		ZeroMemory(&status, sizeof(status));
	};
	DownloadTask(const DownloadTask&) = delete;
	DownloadTask(DownloadTask&&) = default;
	~DownloadTask();

public:
	friend DWORD __stdcall thrd_Downloading(void*);
	friend DownloadTaskID CreateDownloadTaskExW(const PDOWNLOADINFOW info);
	friend bool BeginDownloadTask(DownloadTaskID task);
	friend bool PauseDownloadTask(DownloadTaskID task);
	friend bool ResumeDownloadTask(DownloadTaskID task);
	friend DownloadStatus QueryDownloadTaskStat(DownloadTaskID task);
	friend bool CancelDownloadTask(DownloadTaskID task);
	friend NTSTATUS TerminateDownloadTask(DownloadTaskID task);


protected:

	size_t max_thrds;
	wstring url;
	wstring SaveFileName;
	vector<HANDLE> dlThrds;
	bool pausable;
	enum class methods {
		INVALID = 0, GET, POST, PUT, PATCH, DELETE_
	} method;

	bool DeleteFlag, PauseFlag;
	DownloadStatus status;

	http_headers* header;
};

#endif
