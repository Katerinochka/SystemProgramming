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

#include <thread>
#include "afxsock.h"

struct Header
{
	int n_size;
	int n_to;
};

HANDLE hRead, hWrite, hWriteConfirm, hReadConfirm;
PROCESS_INFORMATION pi;
HANDLE hPipe;
CSocket Server;
CSocket s;

extern "C"
{
	_declspec(dllexport) void _stdcall InitSocket()
	{
		AfxSocketInit();
	}

	_declspec(dllexport) void _stdcall CreateSocketServer()
	{
		Server.Create(12345);
	}

	_declspec(dllexport) void _stdcall ListenSocket()
	{
		Server.Listen();
		Server.Accept(s);
	}

	_declspec(dllexport) void _stdcall SendCode(int code)
	{
		s.Send(&code, sizeof(int));
	}

	_declspec(dllexport) bool _stdcall ConnectServer()
	{
		s.Create();
		if (!s.Connect("127.0.0.1", 12345))
			return false;
		return true;
	}

	_declspec(dllexport) void _stdcall SendString(char* str, int to)
	{
		Header h;
		h.n_size = strlen(str) + 1;
		h.n_to = to;
		s.Send(&h, sizeof(Header));
		s.Send(str, h.n_size);
	}

	_declspec(dllexport) int _stdcall GetCode()
	{
		int code;
		s.Receive(&code, sizeof(int));
		return code;
	}

	_declspec(dllexport) char* _stdcall GetString(int& num)
	{
		Header h;
		s.Receive(&h, sizeof(Header));
		char* str = new char[h.n_size];
		s.Receive(str, h.n_size);
		num = h.n_to;
		return str;
	}

	_declspec(dllexport) void _stdcall Disconnect()
	{
		s.Close();
	}

	__declspec(dllexport) bool  _stdcall ProcessisOpen()//проверка открыта ли консоль
	{
		DWORD dwExitCode;
		if (GetExitCodeProcess(pi.hProcess, &dwExitCode) != 0)
		{
			return dwExitCode == STILL_ACTIVE;
		}
	}
	/*_declspec(dllexport) void _stdcall OpenPipe()
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
		//Cleanup();
		break;
	}
	return TRUE;
}
