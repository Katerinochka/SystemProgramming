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

/*vector <HANDLE> vEventMsg;
HANDLE hEvent;
HANDLE hEventStop;
HANDLE hEventQuit;
HANDLE hEventConfirm;
HANDLE hEventMsg;*/
#include <thread>

struct Header
{
	int n_size;
	int n_to;
};

HANDLE hRead, hWrite, hWriteConfirm, hReadConfirm;
PROCESS_INFORMATION pi;
HANDLE hPipe;

extern "C"
{
	/*__declspec(dllexport) void _stdcall ProcessClient(HANDLE hPipe)
	{
		static int i = 0;
		while (true)
		{
			string s = GetString(hPipe);
			cout << 
		}
	}

	__declspec(dllexport) void _stdcall init()
	{
		while (true)
		{
			HANDLE hPipe = CreateNamedPipe("\\\\.\\pipe\\MyPipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 1024, 1024, 0, NULL);
			ConnectNamedPipe(hPipe, NULL);
			thread t(ProcessClient, hPipe);
			t.detach();
		}
		/*SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
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
	}*/

	_declspec(dllexport) void _stdcall OpenPipe()
	{
		hPipe = CreateNamedPipe("\\\\.\\pipe\\MyPipe",
			PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
			1024, 1024, 0, NULL);
	}

	_declspec(dllexport) void _stdcall Connect()
	{
		ConnectNamedPipe(hPipe, NULL);
	}

	_declspec(dllexport) void _stdcall DisconnectServer()
	{
		DisconnectNamedPipe(hPipe);
	}

	__declspec(dllexport) void _stdcall Cleanup()
	{
		CloseHandle(hRead);
		CloseHandle(hWrite);
		CloseHandle(hReadConfirm);
		CloseHandle(hWriteConfirm);
	}

	/*__declspec(dllexport) string getString(HANDLE hPipe)
	{
		DWORD dwDone;
		int len = getInt()
	}*/

	_declspec(dllexport) bool _stdcall ConnectServer()
	{
		if (WaitNamedPipe("\\\\.\\pipe\\MyPipe", 5000))
		{
			hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			return true;
		}
		return false;
	}

	_declspec(dllexport) void _stdcall Disconnect()
	{
		CloseHandle(hPipe);
	}

	__declspec(dllexport) void _stdcall Send(int n)
	{
		/*DWORD dwWrite;
		Header h;
		h.code = code;
		h.n_size = strlen(str) + 1;
		h.n_to = num;
		WriteFile(hWrite, &h, (BYTE)sizeof(Header), &dwWrite, nullptr);
		WriteFile(hWrite, str, h.n_size, &dwWrite, nullptr);*/
		DWORD dwDone;

		WriteFile(hPipe, &n, sizeof(n), &dwDone, NULL);
		FlushFileBuffers(hPipe);
	}

	__declspec(dllexport) int _stdcall Get()
	{
		DWORD dwDone;
		int n;

		ReadFile(hPipe, &n, sizeof(n), &dwDone, NULL);
		return n;
	}

	__declspec(dllexport) void _stdcall SendString(char* str, int num)
	{
		DWORD dwDone;
		Header h;
		h.n_size = strlen(str) + 1;
		h.n_to = num;

		WriteFile(hPipe, &h, sizeof(Header), &dwDone, NULL);
		WriteFile(hPipe, str, h.n_size, &dwDone, NULL);
		FlushFileBuffers(hPipe);
	}

	__declspec(dllexport) char* _stdcall GetString(int& num)
	{
		DWORD dwDone;
		Header h;

		ReadFile(hPipe, &h, sizeof(Header), &dwDone, NULL);
		char* str = new char[h.n_size];
		num = h.n_to;
		ReadFile(hPipe, str, h.n_size, &dwDone, NULL);
		return str;
	}

	_declspec(dllexport) inline int _stdcall confirm() {
		while (true) {
			int response = Get();
			if (response != 0)
				return response;
		}
	}

	/*__declspec(dllexport) void _stdcall setConfirm()
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
