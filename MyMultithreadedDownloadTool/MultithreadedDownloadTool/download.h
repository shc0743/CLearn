#pragma once
#include "def.h"
#include "../../resource/tool.h"
using namespace std;

// Create a download task
DownloadTaskID CreateDownloadTask(const char* url);
DownloadTaskID CreateDownloadTaskW(const wchar_t* url);
DownloadTaskID CreateDownloadTaskExW(const PDOWNLOADINFOW info);

// Start a download task
bool BeginDownloadTask(DownloadTaskID task);

// Control a download task
bool PauseDownloadTask(DownloadTaskID task);
bool ResumeDownloadTask(DownloadTaskID task);

// Query download progress
DownloadStatus QueryDownloadTaskStat(DownloadTaskID task);

// Cancel a download task
bool CancelDownloadTask(DownloadTaskID task);

// Terminate a download task
NTSTATUS TerminateDownloadTask(DownloadTaskID task);


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
const AppConfig_t* GetCurrentDownloadConfig();
void ConfigDownload(const AppConfig_t*);
