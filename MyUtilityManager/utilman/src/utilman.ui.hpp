#pragma once
#include "framework.h"

BOOL MyInitWndClasses();

constexpr unsigned int MYWM_CTLWND_OPENUI = WM_USER + 0x12;
constexpr unsigned int MYWM_CREATEICON = WM_USER + 0x14;
constexpr unsigned int MYWM_TASKBARICON = WM_USER + 0x15;
constexpr unsigned int MYWM_DIE = WM_USER + 444;

