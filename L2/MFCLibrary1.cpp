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
	int n_to;
	int n_size;
};

extern "C"
{

	__declspec(dllexport) void _stdcall createEvents()
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
		memcpy((char *)pBuff, &h, sizeof(h));
		memcpy((char *)pBuff + sizeof(h), str_box, h.n_size);
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
		memcpy(&h, pBuffRead, sizeof(Header));
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
	}
}