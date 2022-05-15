#pragma once
#include "cppwin_include.h"

typedef struct __MYEULAVIEWDATA {
	DWORD cb;

	WCHAR szFile[MAX_PATH + 4];
	WCHAR szTitle[256];

	void(CALLBACK* pCallback)(BOOL, struct __MYEULAVIEWDATA);

	DWORD dwTimesToAccept;
	DWORD dwTimeout;

	struct __position {
		bool usePosition;
		LONG x;
		LONG y;
		UINT width;
		UINT height;
	} position;

} MYEULAVIEWDATA, *PMYEULAVIEWDATA;

