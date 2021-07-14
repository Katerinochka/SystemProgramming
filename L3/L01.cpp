// L01.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "L01.h"
#include "MFCLibrary1.h"
#include <conio.h>
#include <thread>
#include <vector>
#include <fstream>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

HANDLE hMutex;
vector<HANDLE> ev_close;
HANDLE hEventClose;
vector<HANDLE> vEventMsg;
string msg = "";
HANDLE eConf;

struct Header
{
    int code;
    int n_size;
    int n_to;
};

extern "C"
{
    /*__declspec(dllimport) void _stdcall setEventStart();
    __declspec(dllimport) void _stdcall setEventStop();
    __declspec(dllimport) void _stdcall setEventExit();
    __declspec(dllimport) void _stdcall setEventMessage();
    __declspec(dllimport) void _stdcall waitEventConfirm();
    __declspec(dllimport) void _stdcall addMessageEvent();
    __declspec(dllimport) void _stdcall setThreadEventMessage(int thread_num);
    __declspec(dllimport) HANDLE _stdcall getMessageEvent(int thread_num);
    __declspec(dllimport) void _stdcall createEvents();
    __declspec(dllimport) void _stdcall setAllMsg();
    __declspec(dllimport) int _stdcall getEvent();
    __declspec(dllimport) void _stdcall setEventConfirm();
    __declspec(dllimport) void __stdcall setStr(LPSTR str_box, int num);
    __declspec(dllimport) char* __stdcall getStr(int& num);
    __declspec(dllimport) void _stdcall closeEvents();*/
    __declspec(dllimport) void _stdcall setConfirm();
    __declspec(dllimport) void _stdcall waitConfirm();
}

void write_to_f(int num, string msg)
{
    string fname = to_string(num) + ".txt";
    ofstream fout(fname);
    fout << msg;
    fout.close();
    //SetEvent(eConf);
    //setEventConfirm();
    //setConfirm();
}

Header getStruct()
{
    Header h;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    while (true) {
        DWORD dwRead;
        if (ReadFile(hIn, &h, sizeof(Header), &dwRead, nullptr))
            break;
    }
    return h;
}

char* getMessage(int size)
{
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwRead;
    char* message = new char[size];
    if (!ReadFile(hIn, message, size, &dwRead, nullptr) || !dwRead)
        return NULL;
    return message;
}

UINT __cdecl MyThread(LPVOID lpParameter)
{
    int id = (int)lpParameter;

    WaitForSingleObject(hMutex, INFINITE);
    cout << "Поток № " << id << " создан." << endl;
    ReleaseMutex(hMutex);
    HANDLE hEvents[] = { vEventMsg[id], ev_close[id] };
    bool flag = false;
    while (1)
    {
        switch (WaitForMultipleObjects(2, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {
        case 0:
        {
            WaitForSingleObject(hMutex, INFINITE);
            write_to_f(id, msg);
            //waitEventConfirm();
            //waitConfirm();
            //WaitForSingleObject(eConf, INFINITE);
            ReleaseMutex(hMutex);
            //int box = MessageBox(NULL, (LPCSTR)msg.c_str(), NULL, NULL);
            break;
        }
        case 1:
            WaitForSingleObject(hMutex, INFINITE);
            cout << "Поток № " << id << " завершен." << endl;
            ReleaseMutex(hMutex);
            flag = true;
            break;
        }
        if (flag)
            break;
    }
    return 0;
}

void start()
{
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    hMutex = CreateMutex(NULL, FALSE, "MyMutex");
    eConf = CreateEvent(NULL, FALSE, FALSE, NULL);
    //createEvents();
    Header h;
    int i = 0;
    bool cmd = true;

    while (cmd)
    {
        h = getStruct();
        switch (h.code)
        {
        case 0:
        {
            hEventClose = CreateEvent(NULL, FALSE, FALSE, NULL);
            ev_close.push_back(hEventClose);

            //HANDLE hThread = AfxBeginThread(MyThread, (LPVOID)i++); //2лр
            thread t(MyThread, (LPVOID)i++);
            t.detach();
            vEventMsg.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
            //addMessageEvent();
            break;
        }
        case 1:
        {
            if (!ev_close.empty())
            {
                SetEvent(ev_close[--i]);
                CloseHandle(ev_close[i]);
                ev_close.pop_back();
                break;
            }
        }
        case 2:
        {
            cmd = false;
            for (auto ev : ev_close)
            {
                CloseHandle(ev);
            }
            break;
        }
        case 3:
        {
            int num;
            char *buf = getMessage(h.n_size);
            msg = string(buf);
            num = h.n_to;
            //cout << num << endl;
            if (num == -2)
            {
                //int box = MessageBox(NULL, (LPCSTR)vEventMsg.size(), NULL, NULL);
                //cout << num << endl;
                for (auto ev : vEventMsg)
                    SetEvent(ev);
            }
            //setAllMsg();
            else if (num == -1)
            {
                //cout << num << endl;
                cout << msg << endl;
            }
            else
            {
                //cout << num << endl;
                SetEvent(vEventMsg[num]);
            }
        }
        }
        setConfirm();
    }
    for (auto ev : vEventMsg)
    {
        CloseHandle(ev);
    }
    CloseHandle(hMutex);
    //closeEvents();
}

// Единственный объект приложения

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;
    //HANDLE hMutex = CreateMutex(NULL, FALSE, "MyMutex");
    HMODULE hModule = ::GetModuleHandle(nullptr);
    
    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            //InitializeCriticalSection(&cs);
            start();
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
