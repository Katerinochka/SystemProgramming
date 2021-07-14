// MFCLibrary1.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "MFCLibrary1.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CMFCLibrary1App

BEGIN_MESSAGE_MAP(CMFCLibrary1App, CWinApp)
END_MESSAGE_MAP()


// Создание CMFCLibrary1App

CMFCLibrary1App::CMFCLibrary1App()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CMFCLibrary1App

CMFCLibrary1App theApp;


// Инициализация CMFCLibrary1App

BOOL CMFCLibrary1App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

vector <HANDLE> vEventMsg;
HANDLE hEvent;
HANDLE hEventStop;
HANDLE hEventQuit;
HANDLE hEventConfirm;
HANDLE hEventMsg;

struct Header
{
	int code;
	int n_size;
	int n_to;
};

HANDLE hRead, hWrite, hWriteConfirm, hReadConfirm;
PROCESS_INFORMATION pi;

extern "C"
{
	__declspec(dllexport) void _stdcall init()
	{
		SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
		CreatePipe(&hRead, &hWrite, &sa, 0);
		SetHandleInformation(hWrite, HANDLE_FLAG_INHERIT, 0);
		CreatePipe(&hReadConfirm, &hWriteConfirm, &sa, 0);
		SetHandleInformation(hWrite, HANDLE_FLAG_INHERIT, 0);

		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = hRead;
		si.hStdError = hWriteConfirm;

		CreateProcess(NULL, (LPSTR)("L01.exe"), &sa, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	}
	__declspec(dllexport) void _stdcall Cleanup()
	{
		CloseHandle(hRead);
		CloseHandle(hWrite);
		CloseHandle(hReadConfirm);
		CloseHandle(hWriteConfirm);
	}

	__declspec(dllexport) void _stdcall Send(int code, char* str, int num)
	{
		DWORD dwWrite;
		Header h;
		h.code = code;
		h.n_size = strlen(str) + 1;
		h.n_to = num;
		WriteFile(hWrite, &h, (BYTE)sizeof(Header), &dwWrite, nullptr);
		WriteFile(hWrite, str, h.n_size, &dwWrite, nullptr);
	}

	__declspec(dllexport) void _stdcall setConfirm()
	{
		HANDLE h = GetStdHandle(STD_ERROR_HANDLE);//получение HANDLE переопределенного stdError 
		DWORD dwWrite;
		WriteFile(h, "1", strlen("1"), &dwWrite, nullptr);//запись в отдельный анонимный канал подтверждения
	}

	__declspec(dllexport) void _stdcall waitConfirm()
	{
		const int MAXLEN = 1;
		while (true)
		{
			DWORD dwRead;
			char buff[MAXLEN + 1];
			if (ReadFile(hReadConfirm, buff, MAXLEN, &dwRead, nullptr) || !dwRead)//попытка считываения подверждения из анонимного канала, выйдет из цикла, когда отработает функция setConfirm
			{
				break;
			}
		}

	}

	__declspec(dllexport) bool  _stdcall ProcessisOpen()//проверка открыта ли консоль
	{
		DWORD dwExitCode;
		if (GetExitCodeProcess(pi.hProcess, &dwExitCode) != 0)
		{
			return dwExitCode == STILL_ACTIVE;
		}
	}
	/*__declspec(dllexport) void _stdcall createEvents()
	{
		hEvent = CreateEvent(NULL, FALSE, FALSE, "MyEvent");
		hEventStop = CreateEvent(NULL, FALSE, FALSE, "MyEventStop");
		hEventQuit = CreateEvent(NULL, FALSE, FALSE, "MyEventQuit");
		hEventConfirm = CreateEvent(NULL, FALSE, FALSE, "MyEventConfirm");
		hEventMsg = CreateEvent(NULL, FALSE, FALSE, "MyEventMsg");
	}

	__declspec(dllexport) void _stdcall setEventStart()
	{
		SetEvent(hEvent);
	}

	__declspec(dllexport) void _stdcall setEventStop()
	{
		SetEvent(hEventStop);
	}

	__declspec(dllexport) void _stdcall setEventExit()
	{
		SetEvent(hEventQuit);
	}

	__declspec(dllexport) void _stdcall setEventMessage()
	{
		SetEvent(hEventMsg);
	}

	__declspec(dllexport) void _stdcall waitEventConfirm()
	{
		WaitForSingleObject(hEventConfirm, INFINITE);
	}

	__declspec(dllexport) void _stdcall addMessageEvent()
	{
		vEventMsg.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
	}

	__declspec(dllexport) void _stdcall setThreadEventMessage(int thread_num)
	{
		SetEvent(vEventMsg[thread_num]);
	}

	__declspec(dllexport) void _stdcall setAllMsg()
	{
		for (int i = 0; i < int(vEventMsg.size()); i++)
		{
			SetEvent(vEventMsg[i]);
		}
	}

	__declspec(dllexport) void _stdcall setEventConfirm()
	{
		SetEvent(hEventConfirm);
	}

	__declspec(dllexport) HANDLE _stdcall getMessageEvent(int thread_num)
	{
		return vEventMsg[thread_num];
	}

	__declspec(dllexport) void _stdcall closeEvents()
	{
		CloseHandle(hEvent);
		CloseHandle(hEventStop);
		CloseHandle(hEventQuit);
		CloseHandle(hEventMsg);
		CloseHandle(hEventConfirm);
	}

	__declspec(dllexport) int _stdcall getEvent()
	{
		HANDLE hEvents[] = { hEvent, hEventStop, hEventQuit, hEventMsg };
		int eventNumber = WaitForMultipleObjects(4, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		return eventNumber;
	}

	__declspec(dllexport) void __stdcall setStr(LPSTR str_box, int num)
	{
		string s = str_box;
		Header h;
		h.n_to = num;
		h.n_size = s.length() + 1;
		HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(h) + h.n_size, "MyFileMap");
		LPVOID pBuff = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(h) + h.n_size);
		//memcpy((char *)pBuff, &h, sizeof(h));
		DWORD dwWrite;
		WriteFile(hWrite, &h, sizeof(h), &dwWrite, nullptr);
		//memcpy((char *)pBuff + sizeof(h), str_box, h.n_size);
		WriteFile(hWrite, str_box, h.n_size, &dwWrite, nullptr);
		setEventMessage();
		waitEventConfirm();
		//UnmapViewOfFile((char*)pBuff);
		CloseHandle(hFileMap);
	}

	__declspec (dllexport) char* __stdcall getStr(int& num)
	{
		Header h;
		HANDLE hFileMapRead = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(Header), "MyFileMap");
		LPVOID pBuffRead = (BYTE*)MapViewOfFile(hFileMapRead, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Header));
		//memcpy(&h, pBuffRead, sizeof(Header));
		HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
		DWORD dwRead;
		ReadFile(hIn, (LPVOID)h, sizeof(Header), &dwRead, nullptr);
		//UnmapViewOfFile(pBuffRead);
		CloseHandle(hFileMapRead);
		num = h.n_to;
		HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Header) + h.n_size, "MyFileMap");
		LPVOID pBuff = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Header) + h.n_size);
		char* msg = new char[h.n_size];
		memcpy(msg, (char*)pBuff + sizeof(h), h.n_size);
		UnmapViewOfFile(pBuff);
		CloseHandle(hFileMap);
		return msg;
	}*/
}

BOOL APIENTRY MFCLibrary1(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Cleanup();
		break;
	}
	return TRUE;
}
