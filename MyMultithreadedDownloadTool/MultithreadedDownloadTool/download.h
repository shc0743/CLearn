#pragma once
#include "def.h"
#include "../../resource/tool.h"
using namespace std;

// Create a download task
DownloadTaskID WINAPI CreateDownloadTask(const char* url);
DownloadTaskID WINAPI CreateDownloadTaskW(const wchar_t* url);
DownloadTaskID WINAPI CreateDownloadTaskExW(const PDOWNLOADINFOW info);

// Start a download task
bool WINAPI BeginDownloadTask(DownloadTaskID task);

// Control a download task
bool WINAPI PauseDownloadTask(DownloadTaskID task);
bool WINAPI ResumeDownloadTask(DownloadTaskID task);

// Query download progress
DownloadStatus WINAPI QueryDownloadTaskStat(DownloadTaskID task);

// Cancel a download task
bool WINAPI CancelDownloadTask(DownloadTaskID task);

// Terminate a download task
NTSTATUS WINAPI TerminateDownloadTask(DownloadTaskID task);


// download core components
typedef struct {
	DownloadTaskID task_id;
	size_t thread_id;
	size_t total_size;
	size_t begin;
} DlThrdInfo, * pDlThrdInfo;
// download core thread
DWORD __stdcall thrd_Downloading(void*);


// App.Config
const AppConfig_t* WINAPI GetCurrentDownloadConfig();
void WINAPI ConfigDownload(const AppConfig_t*);
