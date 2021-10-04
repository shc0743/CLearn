#include <iostream>
#include "../../resource/tool.h"

vector<HANDLE> handles;

int main(int argc,char* argv[])
{
    if (argc > 1) {
        HANDLE h;
        for (int i = 1; i < argc; ++i) {
            h = CreateFileSimpleW(s2wc(argv[i]), FILE_ALL_ACCESS, OPEN_EXISTING, 0);
            if (h) {
                handles.push_back(h);
            }
            else {
                fprintf(stderr, "[%d Warn] Cannot lock file: %s\n", (int)time(0),
                    ws2c(LastErrorStr()));
            }
        }
        while (1) Sleep(MAXDWORD);
        return 0;
    }
    size_t bufsize = 8192;
    char* buffer = (char*)GetBufMemory(bufsize);
    do {
        std::cout << "输入或拖入要锁定的文件,输入0保持锁定并退出: ";
        std::cin.getline(buffer, bufsize);
        if (buffer == string("0")) {
            break;
        }
        STARTUPINFO si;
        memset(&si, 0, sizeof(si));
        si.wShowWindow = SW_HIDE;
        auto pi = Process.Start(s2ws("powershell start-process -filepath \""+GetProgramDir() + 
            "\" -argumentlist \"" + buffer + "\" -windowstyle hidden"), si);
        Process.CloseProcessHandle(pi);
        Sleep(1000);
    } while (1);
    return 0;
}
