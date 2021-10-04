#include<iostream>
#include<string>
#include<sstream>
#include<stdio.h>
#include<cstdio>
#include<conio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fstream>

//Windows程序
#include<Windows.h>
//Vector变长数组
#include<vector>

#include<future>

#include "fn.h"

using namespace std;

#define N 16383
#define getche _getche
#define getch _getch

#define wout oss_last

void hideWindow();
bool file_exists2(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
bool SetConsoleColor(WORD color)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    return SetConsoleTextAttribute(handle, color); // BACKGROUND_BLUE
}
unsigned int noactcnt = (short)(unsigned)0;
void NoActiveFn() {
        SetConsoleColor((WORD)04);
            MessageBox(FindWindow("ConsoleWindowClass", NULL), "请尽快激活!", "温馨提示", MB_ICONINFORMATION);
}

#include"timer.h"
int main(int argc,char* argv[]) {
    if (argc>1&&argv[1][0]=='-') {
        if (argv[1][1] == '-') {
            if (argv[1][2] == 's'&&argv[1][3] == 'h'&&argv[1][4] == 'o'&&argv[1][5] == 'w') {
                system("echo true > maf_ma_t_sw_show_test");
                return 0;
            }
        }
    }
    HWND hwnd;
    hwnd = FindWindow("ConsoleWindowClass", NULL);	//处理顶级窗口的类名和窗口名称匹配指定的字符串,不搜索子窗口。
    if (hwnd) ShowWindow(hwnd, SW_SHOW);
    char c;
    int MALLOCSLENGTH = 64;//(argv[1] == "/length" || argv[1] == "--length") ? atoi(argv[2]) : 64;
    vector<int*> mallocs(N);
    ostringstream oss,oss_last;
    int icid;
    mallocs[0] = NULL;
    bool again = true;
    int i = 0,mcon;
    string aa,mid;

    string actkey;
    if (file_exists2("act.ok.key")) {
        if (system("act")==3) {
            MessageBox(FindWindow("ConsoleWindowClass", NULL), "程序被非法破解!", "错误", MB_ICONERROR);
            exit(3);
        }
        fstream actfp;
        actfp.open("act.ok.key", ios::in);
        char vcode[64];
        actfp.getline(vcode, 64);
        actkey = vcode;
        actfp.close();
    }
    else {
    }

    system("title int内存分配工具");
    while (again) {
        again = false;
        i = 0;
        for (;i < N;) {
            cls;cin.clear();cin.sync();
            if(actkey.length()||(!(noactcnt % 3))) SetConsoleColor((WORD)07);
            if (!actkey.length()) {
                noactcnt++;
                if (noactcnt > 2147483648) noactcnt = 0;
                if (!(noactcnt % 25)) NoActiveFn();
            }
            cout << "========\n"
                "您的选择: \n"
                "警告: [重要信息!!!!]请勿使用此程序存放重要数据!此程序在内存中存储数据,不保存!!!\n";
            if (actkey.length()) {
                SetConsoleColor((WORD)02);
                cout << "[激活密钥:" << actkey << "](已激活)\n";
            } else  {
                SetConsoleColor((WORD)04);
                cout << "[激活密钥:未激活](请尽快激活)\n";
            }
            if(actkey.length()||(!(noactcnt % 3))) SetConsoleColor((WORD)07);
            cout << "提示 (当前" << sizeof(int*) * 8 << "位程序):\n"
                <<(actkey.length()?"":"按~激活程序,\n")<<
                "按+释放所有已申请的内存,\n"
                "按其他键继续申请,\n"
                "已申请 " << i << " 次,还可申请 "/*<< N - i << */ << "无限制" << " 次,\n"
                "按&用指定的ID申请(不建议),\n"
                "按*释放指定的ID,\n"
                "按!或按\\或按/退出,\n"
                "按$隐藏窗口(运行--show恢复),\n"
                "按%显示隐藏的窗口,\n"
                "按#用ID向指定已申请的内存写入数据,\n"
                "按@用ID向指定已申请的内存读数据: ";
            cout << endl << "========" << endl;
            if(wout.str()!="") cout << endl << "上次输出: " << oss_last.str();
            oss_last.str("");
            movePosition(0,0);
            movePosition(1,12);
            c = getche();
            if (c == '+') {
                for (int j = 0; j < i; j++) {
                    if (mallocs[j] == NULL) continue;
                    ostringstream oss;
                    oss << mallocs[j];
                    free(mallocs[j]);
                    mallocs[j] = NULL;
                    wout << "成功释放内存: " << oss.str() << endl;
                }
                again = true;
                break;
            }
            else if (c == '!' || c == '！' || c == '\\' || c == '/') {
                char a = 'Y';
                cls;
                if (mallocs[0]!=NULL){
                    cout << "警告!还有未释放的内存,直接退出将导致内存泄漏!\n要强制退出,输入Y;\n" <<
                        "要释放内存,先输入N,再输入+,最后输入!或\\退出." << endl << "输入您的选择:";
                    a = getche();
                    cout << endl;
                }
                if (a == 'Y' || a == 'y') exit(0);
            }
            else if (c == '#') {
                cls;
                icid = NULL; mcon=NULL;
                cout << "======" << endl;
                cout << "输入ID: ";
                cin >> icid;
                if(cin.good()!=1){
                    wout << "输入非法:读取失败"; continue;
                }
                if(icid>mallocs.capacity()){
                    wout << "输入非法:越界"; continue;
                }
                if (mallocs[icid] == NULL) {
                    wout << "输入非法:尝试使用未申请的内存"; continue;
                }
                cout << "输入要写入的数据(int, -2147483648<=此数据<2147483648): ";
                cin >> mcon;
                if(mcon<-2147483648||mcon>2147483647){
                    wout << "输入非法:越界"; continue;
                }
                if(cin.good()!=1){
                    wout << "输入非法:读取失败"; continue;
                }
                wout << "正在写入... ";
                *mallocs[icid] = mcon;
                wout << "写入成功" << endl;
                cout << "======" << endl;
            }
            else if (c == '@') {
                icid = NULL; mcon=NULL;
                cls;
                cout << "======" << endl;
                cout << "输入ID: ";
                cin >> icid;
                if (cin.good() != 1) {
                    wout << "输入非法:读取失败"; continue;
                }
                if (icid > mallocs.capacity()) {
                    wout << "输入非法:越界"; continue;
                }
                if (mallocs[icid] == NULL) {
                    wout << "输入非法:尝试使用未申请的内存"; continue;
                }
                wout << "ID为 " << icid << " (内存地址为" << mallocs[icid] << ") 的内存值是: "<<endl << *mallocs[icid] << endl;
                cout << "======" << endl;
            }
            else if (c == '&') {
                icid = NULL; mcon=NULL;
                cls;
                cout << "======" << endl;
                cout << "输入ID: ";
                cin >> icid;
                if (cin.good() != 1) {
                    wout << "输入非法:读取失败"; continue;
                }
                mallocs.reserve(icid);
                wout << "正在申请... "; 
                if (mallocs[icid] = (int*)malloc(MALLOCSLENGTH))
                    wout << "成功申请内存: " << mallocs[icid] << " ID: " << icid << endl;
                else wout << "无法申请内存" << endl;
                cout << "======" << endl;
            }
            else if (c == '*') {
                icid = NULL; mcon=NULL;
                cls;
                cout << "======" << endl;
                cout << "输入ID: ";
                cin >> icid;
                if (cin.good() != 1) {
                    wout << "输入非法:读取失败"; continue;
                }
                if (icid > mallocs.capacity()) {
                    wout << "输入非法:越界"; continue;
                }
                if (mallocs[icid] == NULL) continue;
                ostringstream oss;
                oss << mallocs[icid];
                free(mallocs[icid]);
                mallocs[icid] = NULL;
                wout << "成功释放内存: " << oss.str() << endl;
            }
            else if (c == '$') {
                hideWindow();
                again = true;
                break;
            }
            else if (c == '%') {
                oss.str("");
                oss << argv[0] << "  --show";
                system(oss.str().c_str());
                exit(0);
            }
            else if (c == '~') {
                if (system("act")) {
                    MessageBox(FindWindow("ConsoleWindowClass", NULL), "激活失败!", "错误", MB_ICONERROR);
                    continue;
                };
                WinExec((((string)"powershell start-process -filepath \"")+GetCommandLineA()+"\"").c_str(), SW_SHOW);
                exit(0);
            }
            else {
                cout << "======" << endl;
                while (mallocs[i] && mallocs[i] != NULL) i++;
                if (mallocs[i] = (int*)malloc(MALLOCSLENGTH))
                    wout << "成功申请内存: " << mallocs[i] << " ID: " << i << endl;
                else wout << "无法申请内存" << endl;
                i++;cout << "======" << endl;
            }
        }
    }
    cls;
    cout << "无法申请内存,正在释放..." << endl; 
    for (int j = 0; j < i; j++) {
        ostringstream oss;
        oss << mallocs[j];
        free(mallocs[j]);
        mallocs[j] = NULL;
        cout << "成功释放内存: " << oss.str() << endl;
    }
    exit(0);
}

bool file_exists(const std::string& name) {
    fstream _file;
    _file.open(name, ios::in|ios::binary);
    if (!_file) {
        _file.close();
        return false;
    } else {
        _file.close();
        return true;
    }
}
void hideWindow() {
    if (MessageBox(NULL,"确定隐藏窗口?\n通过在相同目录执行 程序 --show 恢复","确定吗?",MB_ICONQUESTION|MB_OKCANCEL) == IDCANCEL) return;
    HWND hwnd;
    hwnd = FindWindow("ConsoleWindowClass", NULL);	//处理顶级窗口的类名和窗口名称匹配指定的字符串,不搜索子窗口。
    if (hwnd)
    {
        ShowWindow(hwnd, SW_HIDE);				//设置指定窗口的显示状态
        while (1) {
            bool fe = file_exists("maf_ma_t_sw_show_test");
            //MessageBox(NULL,(""+file_exists("maf_ma_t_sw_show_test")),"",MB_OK);
            if (fe) {
                ShowWindow(hwnd, SW_SHOW);
                system("del /f /s /q maf_ma_t_sw_show_test");
                return;
            } 
            Sleep(3000);
        }
    }
    else return;
}