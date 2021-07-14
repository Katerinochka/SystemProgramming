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

HANDLE hMutex = CreateMutex(NULL, FALSE, "MyMutex");
vector<HANDLE> ev_close;
vector<HANDLE> vEventMsg;
string msg = "";

struct Header
{
    int n_size;
    int n_to;
};

extern "C"
{
    __declspec(dllimport) int _stdcall Get();
    __declspec(dllimport) void _stdcall Send(int n);
    __declspec(dllimport) char* _stdcall GetString(int& num);
    __declspec(dllimport) void _stdcall OpenPipe();
    __declspec(dllimport) void _stdcall Connect();
    __declspec(dllimport) void _stdcall DisconnectServer();
}

void write_to_f(int num, string msg)
{
    string fname = to_string(num) + ".txt";
    ofstream fout(fname);
    fout << msg;
    fout.close();
    //SetEvent(eConf);
    //setConfirm();
}

/*Header getStruct()
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
}*/

UINT __cdecl MyThread(LPVOID lpParameter)
{
    int id = (int)lpParameter;

    WaitForSingleObject(hMutex, INFINITE);
    cout << "Thread " << id << " launched" << endl;
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
            //waitConfirm();
            //WaitForSingleObject(eConf, INFINITE);
            ReleaseMutex(hMutex);
            break;
        }
        case 1:
        {
            WaitForSingleObject(hMutex, INFINITE);
            cout << "Thread " << id << " completed" << endl;
            ReleaseMutex(hMutex);
            flag = true;
            break;
        }
        }
        if (flag)
            break;
    }
    return 0;
}

void start()
{
    //SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    //hMutex = CreateMutex(NULL, FALSE, "MyMutex");
    //eConf = CreateEvent(NULL, FALSE, FALSE, NULL);
    //Header h;
    OpenPipe();
    int i = 0;
    bool cmd = true;

    while (cmd)
    {
        //h = getStruct();
        Connect();
        int code = Get();
        switch (code)
        {
        case 0:
        {
            int num = Get();
            //hEventClose = CreateEvent(NULL, FALSE, FALSE, NULL);
            for (int k = 0; k < num; k++)
            {
                ev_close.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));

                thread t(MyThread, (LPVOID)i++);
                t.detach();
                vEventMsg.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
            }
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
        /*case 2:
        {
            cmd = false;
            for (auto ev : ev_close)
            {
                CloseHandle(ev);
            }
            break;
        }*/
        case 2:
        {
            int num;
            char *buf = GetString(num);
            msg = string(buf);
            if (num == -2)
                for (int j = 0; j < ev_close.size(); j++)
                    SetEvent(vEventMsg[j]);
            else if (num == -1)
                cout << msg << endl;
            else
                SetEvent(vEventMsg[num]);
        }
        }
        Send(ev_close.size());
        DisconnectServer();
    }
    /*for (auto ev : vEventMsg)
    {
        CloseHandle(ev);
    }
    CloseHandle(hMutex);*/
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
