/*******
Download Manager
*******/
#include "download.h"
//#define DOWNLOADMANAGERLOADED 1
#pragma warning(disable: 26812)
#pragma warning(disable: 6258)

extern HINSTANCE hInst;
extern size_t dlLastId;
extern AppConfig_t AppConfig;


DownloadTaskID CreateDownloadTask(const char* url) {
	return CreateDownloadTaskW(s2wc(url));
}
DownloadTaskID CreateDownloadTaskW(const wchar_t* url) {
	DOWNLOADINFOW dlInfo; memset(&dlInfo, 0, sizeof(dlInfo));
	dlInfo.url = url;
	dlInfo.method = dlInfo.GET;
	return CreateDownloadTaskExW(&dlInfo);
}

DownloadTaskID CreateDownloadTaskExW(const PDOWNLOADINFOW info) {
	// verify paramter
	if (info->url == NULL || info->method == info->INVALID) {
		SetLastError(ERROR_INVALID_PARAMETER); // The parameter is incorrect.
		return 0;
	}

	// Create object
	DownloadTask* tsk = new DownloadTask;
	tsk->url = info->url;
	tsk->max_thrds = info->max_thread_count ?
		info->max_thread_count : AppConfig.DownloadMaxThreadCount;
	tsk->method = (DownloadTask::methods)info->method;

	tsk->status.status = DownloadStatus::NOT_STARTED;

	return tsk;
}

bool BeginDownloadTask(DownloadTaskID task) {
	// do some init

	// and call ResumeDownloadTask
	return ResumeDownloadTask(task);
}

bool PauseDownloadTask(DownloadTaskID task) {
	if (!task->pausable) return false;
	bool bOk = true; vector<size_t> rm_pos;
	rm_pos.reserve(task->dlThrds.size());
	for (size_t i = 0; i < task->dlThrds.size();++i) {
		if (WAIT_TIMEOUT == ::WaitForSingleObject
		(task->dlThrds[i], AppConfig.DownloadCancelMaxWaitTime)) bOk = false;
		else {
			::CloseHandle(task->dlThrds[i]);
			rm_pos.push_back(i);
		}
	}
	for (size_t i = rm_pos.size() - 1; i >= 0; --i) {
		task->dlThrds.erase(task->dlThrds.begin() + rm_pos[i]);
	}
	return bOk;
}

bool ResumeDownloadTask(DownloadTaskID task) {
	return false;
}

DownloadStatus QueryDownloadTaskStat(DownloadTaskID task) {
	return DownloadStatus();
}

DownloadTask::~DownloadTask() {
	if (!dlThrds.empty()) {
		this->DeleteFlag = true;
		for (auto i : dlThrds) {
			if (WaitForSingleObject(i,
				AppConfig.DownloadCancelMaxWaitTime) == WAIT_TIMEOUT) {
				::TerminateThread(i, 1);
			}
		}
	}
}

bool CancelDownloadTask(DownloadTaskID task) {
	return false;
}

NTSTATUS TerminateDownloadTask(DownloadTaskID task) {
	return NTSTATUS();
}


//void _donothing_c76795d0(size_t n) { n++; }
DWORD __stdcall thrd_Downloading(void* pId) {
	DlThrdInfo dinf = *((pDlThrdInfo)pId);
	//auto _donothing = _donothing_c76795d0;
	//__try {
	//	size_t a = 0;
	//	a = *(int*)dinf.task_id;
	//	_donothing(a);
	//	a = dinf.thread_id;
	//	_donothing(a);
	//	a = dinf.total_size;
	//	_donothing(a);
	//	a = dinf.begin;
	//	_donothing(a);
	//}
	//__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?
	//	EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
	//	return EXCEPTION_ACCESS_VIOLATION;
	//}
	DownloadTask* task_details = dinf.task_id;
	//__try { task_details = dinf.task_id; }
	//__except (EXCEPTION_EXECUTE_HANDLER) {
	//	return ERROR_NOT_FOUND;
	//}
	if (!task_details) return ERROR_NOT_FOUND;

	using namespace requests;

	HANDLE hFile = NULL;
	hFile = CreateFileW(
		(L"\\\\?\\" + task_details->SaveFileName + L".dltask_" +
			to_wstring(dinf.thread_id) + L".tmp").c_str(),
		GENERIC_ALL, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, 0);
	if (!hFile) {
		return GetLastError();
	}

	Response resp;
	if (task_details->method == DownloadTask::methods::GET) {
		const char* url = ws2c(task_details->url);
		http_headers header = 
			(task_details->header) ? (*task_details->header) : DefaultHeaders;
		auto hesp = head(url); // hesp: Head of response
		bool range_supported = false;
		std::string accept_ranges = resp->GetHeader("Accept-Ranges");
		// use Range if server accept_ranges
		if (hesp->status_code == 200 &&
			accept_ranges == "bytes") {
			range_supported = true;
		}
		
		// Not use range
		if (!range_supported) {
			resp = requests::get(url, header);
			if (resp == NULL) {
				::CloseHandle(hFile);
				return -1;
			}

			return 0;
		}

		 resp = requests::get(url, header);
		 ::CloseHandle(hFile);
		 return 0;
	}

	::CloseHandle(hFile);
	return 0;
}


const AppConfig_t* GetCurrentDownloadConfig() {
	return &AppConfig;
}

void ConfigDownload(const AppConfig_t* newConfig) {
#ifdef _MSVlC_LANG
	memcpy_s(&AppConfig, newConfig, sizeof(AppConfig));
#else
	memcpy(&AppConfig, newConfig, sizeof(AppConfig));
#endif
}

