#include "../../resource/tool.h"
using namespace std;

HHOOK hHook;
void UnlockKeyboardAndExit(int);
LRESULT CALLBACK hHookFunc_FuckKeyboard(int nCode, WPARAM wParam, LPARAM lParam);

int main() {
    cout << "FuckKeyboard.exe\n"
        "Disable your keyboard.\n\n";
    cout << "Press any key to disable keyboard..." << string(19, '\b');
    signal(SIGINT, UnlockKeyboardAndExit);
	bool kdisabled = false;
    while (1) {
		(void)_getch();
		kdisabled = !kdisabled;
		if (kdisabled) { // Disable keyboard
			printf(" en\b\b\b");
			if (!(hHook = ::SetWindowsHookExW(WH_KEYBOARD_LL,
				hHookFunc_FuckKeyboard, GetModuleHandle(NULL), 0))) {
				cerr << "[ERROR] " << LastErrorStrA() << endl;
			}
		} else { // Enable keyboard
			printf("dis\b\b\b");
			if (!hHook || !::UnhookWindowsHookEx(hHook)) {
				cerr << "[ERROR] " << LastErrorStrA() << endl;
			}
		}
    }
    return 0;
}

void UnlockKeyboardAndExit(int) {
	if (!::UnhookWindowsHookEx(hHook)) {
		cerr << "[ERROR] " << LastErrorStrA() << endl;
	}
	::ExitProcess(0);
}

// 钩子子程
LRESULT CALLBACK hHookFunc_FuckKeyboard(int nCode, WPARAM wParam, LPARAM lParam) {
	PKBDLLHOOKSTRUCT pVirKey = (PKBDLLHOOKSTRUCT)lParam;

	// MSDN说了,nCode < 0的时候别处理
	if (nCode >= 0)
	{
		// 按键消息
		/* 吃掉消息 */ return 1;
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}
