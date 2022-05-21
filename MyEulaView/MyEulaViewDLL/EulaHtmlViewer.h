#pragma once
#include <Windows.h>

typedef struct __MYEULAVIEWDATA {
	_In_ DWORD cb;

	_In_ WCHAR szFile[MAX_PATH + 4];
	_In_ WCHAR szTitle[256];

	_Out_ bool bAnyOperationFailed;
	_Out_ bool bResult;
	_Out_ DWORD dwErrorCode;
	_Out_ DWORD dwErrTrack;

	_In_ DWORD dwTimesToAccept;
	_In_ DWORD dwTimeout;

	_Inout_ WCHAR szBrowser[MAX_PATH + 4];

	_In_ WCHAR szLabelAccept[64];
	_In_ WCHAR szLabelDecline[64];

	_In_ struct __position {
		bool usePosition;
		LONG x;
		LONG y;
		UINT width;
		UINT height;
	} position;

	_Inout_ void(CALLBACK* pCallback)(BOOL, struct __MYEULAVIEWDATA *);

} MYEULAVIEWDATA, *PMYEULAVIEWDATA;

typedef BOOL(WINAPI* EulaView_HTML_t)(PMYEULAVIEWDATA);
constexpr PCSTR EulaView_HTML_n = "EulaView_HTML";

