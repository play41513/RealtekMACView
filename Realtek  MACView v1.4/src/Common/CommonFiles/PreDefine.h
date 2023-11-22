// Module: PreDefine.h
// Notices: Copyright(c) 2014 Robbin
// Purpose: Common header file containing handy macros and definitions
//		    used throughout all the applications on the Microsoft Windows.
// Version: 1.0.0.2

////////////////////////////////////////////////////////////////////////////////
// For vc 每個編譯單元只能引入此標題檔一次
#if _MSC_VER > 1000
#pragma once
#endif

// For c++ builder.
#ifndef _PreDefine_h
#define _PreDefine_h
////////////////////////////////////////////////////////////////////////////////
// Windows版本
// 在sdkddkver.h中定義
//Windows XP 0x0501
//Windows Server 2003 0x0502
//Windows Vista 0x0600
//_WIN32_WINNT_LONGHORN vista
//Windows 7 0x0601
//Windows 8 0x0602
//Windows 8.1 0x0603
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0603//_WIN32_WINNT_LONGHORN
#endif

#ifndef WINVER
#ifdef _WIN32_WINNT
// set WINVER based on _WIN32_WINNT
#define WINVER  _WIN32_WINNT
#else
#define WINVER 0x0601
#endif
#endif
////////////////////////////////////////////////////////////////////////////////
// 使用Unicode版本Windows函式
//#ifndef UNICODE
//#define UNICODE
//#endif
//// 使用Unicode版本Windows函式時，也使用Unicode版本C執行階段函式
//#ifdef UNICODE
//#ifndef _UNICODE
//#define _UNICODE
//#endif
//#endif
////////////////////////////////////////////////////////////////////////////////
// 引入Windows定義(Include Windows Definitions)

//設定編譯警告等級為3，因為Windows.h含非標準c語言語法，在等級4會有大量警告
#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)

//設定編譯警告等級為4，最高階警告
#pragma warning(push, 4)

#include <windowsx.h>
#include <CommCtrl.h>
#include <process.h> //_beginthreadex
#include <tchar.h>
#include <strsafe.h>
////////////////////////////////////////////////////////////////////////////////
// 定義tstring
#include <vector>
using namespace std;
typedef std::basic_string<TCHAR> tstring;
//#ifndef UNICODE
//  typedef std::string  tstring;
//#else
//  typedef std::wstring tstring;
//#endif

#include <assert.h>
////////////////////////////////////////////////////////////////////////////////
// 驗證使用最合適的標頭檔(Verify that the proper header files are being used)
#ifndef FILE_SKIP_COMPLETION_PORT_ON_SUCCRSS
#pragma message("You are not using the lastest platform SDK header/library ")
#pragma message("files. This may prevent the project from building correctly.")
#endif
////////////////////////////////////////////////////////////////////////////////
/* _countof helper */
#if !defined(_countof)
#if !defined(__cplusplus)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
extern "C++"
{
template <typename T, size_t N>
char (*__countof_helper(UNALIGNED T(&A)[N]))[N];
#define _countof(A) (sizeof(*__countof_helper(A)) + 0)

//template <typename T, size_t N>
//char (&ArraySizeHelper(UNALIGNED T (&array)[N]))[N];
//#define _countof(array) (sizeof(ArraySizeHelper(array)))
//
//template <typename T, size_t N>
//char (*RtlpNumberOf( UNALIGNED T (&)[N] ))[N];
//#define RTL_NUMBER_OF_V2(A) (sizeof(*RtlpNumberOf(A)))
//#define _countof(A) (sizeof(*RtlpNumberOf(A)))
}
#endif
#endif
////////////////////////////////////////////////////////////////////////////////
//執行在 warning level 4時，去除非必要警告(Allow code to compile cleanly at warning level 4)
// 使用了非標準的擴充:單行註解(nonstandard extension 'single line comment' was used)
#pragma warning(disable:4001)
// 無法參照的正規參數(unreferenced formal parameter)
#pragma warning(disable:4100)
// 注意:建立預先編譯好的標頭(Creating precompiled header)
#pragma warning(disable:4699)
// 函式並非內嵌的(function not inlined)
#pragma warning(disable:4710)
// 無法參照的內嵌函式被移除了(unreferenced inline function has been removed)
#pragma warning(disable:4514)
// 無法產生指派運算子(assignment operator could not be generated)
#pragma warning(disable:4512)
// 從LONGLONG轉換為ULONGLONG，有號/無號未匹配(conversion from 'LONGLONG' to 'ULONGLONG', signed/unsigned mismatch)
#pragma warning(disable:4245)
// type cast:從LONG轉換為最大的HINSTANCE('type cast' : conversion from 'LONG' to 'HINSTANCE' of greater size)
#pragma warning(disable:4312)
// argument:從LPARAM轉換為LONG，可能遺失資料('argument' : conversion from 'LPARAM' to 'LONG', possible loss of data)
#pragma warning(disable:4244)
// wsprintf:名稱被標示為無效的#pragma('wsprintf': name was marked as #pragma deprecated)
#pragma warning(disable:4995)
// 一元減法運算子套用於無號類型，結果仍為無號(unary minus operator applied to unsigned type, result still unsigned)
#pragma warning(disable:4146)
// argument:從size_t轉換為int，可能遺失資料(// 'argument' : conversion from 'size_t' to 'int', possible loss of data)
#pragma warning(disable:4267)
// 使用了非標準的擴充:無具名的struct/union(nonstandard extension used : nameless struct/union)
#pragma warning(disable:4201)
////////////////////////////////////////////////////////////////////////////////
// pragma訊息輔助巨集(Pragma message helper macro)
//When the compiler sees a line like this:
//   #pragma chMSG(Fix this later)
//
//it outputs a line like this:
//
//  c:\CD\CmnHdr.h(82):Fix this later
//
//You can easily jump directly to this line and examine the surrounding code.
#define pdSTR2(x) #x
#define pdSTR1(x) pdSTR2(x)
#define pdMsg(desc) message(__FILE__"("chSTR(__LINE__)"): " #desc)
////////////////////////////////////////////////////////////////////////////////
// 數字介於二者之間的話，傳回true
#define pdInRange(low, num, high) (((num) >= (low)) && ((num) <= (high)))
////////////////////////////////////////////////////////////////////////////////
// 計算字串所需的位元組數
#define pdSIZEOFSTRING(psz) ((lstrlen(psz) + 1) * sizeof(TCHAT))

////////////////////////////////////////////////////////////////////////////////
// 內嵌函式
// 將數值無條件捨去為最近倍數
template<class TV, class TM>
inline TV pdROUNDDOWN(TV value, TM multiple) {
	return((value / multiple) * multiple)
}

template<class TV, class TM>
inline TV pdROUNDUP(TV value, TM multiple) {
	return(pdROUNDDOWN(value, multiple) + ((value % multiple) ? multiple : 0))
}

////////////////////////////////////////////////////////////////////////////////
//封裝 C runtime 的函式 _beginthreadex(將參數型態轉換與windows CreateThread相容)
typedef unsigned(__stdcall*PTHREAD_START)(void*);
#define pdBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, pvParamm, dwCreateFlag,\
	pdwThreadId)                                                               \
	((HANDLE)_beginthreadex(                                                   \
		(void*)(psa),                                                          \
		(unsigned)(cbStackSize),                                               \
		(PTHREAD_START)(pfnStartAddr),                                         \
		(void*)(pvParamm),                                                     \
		(unsigned)(dwCreateFlag),                                              \
		(unsigned*)(pdwThreadId),                                              \
	))                                                                         \
////////////////////////////////////////////////////////////////////////////////
// 改善 x86平台 的 DebugBreak()
 #ifdef _X86_
 #define DebugBreak() _asm{int 3}
 #endif
////////////////////////////////////////////////////////////////////////////////
// 軟體例外巨集
#define MAKESOFTWAREEXCEPTION(severity, facility, exception) (                 \
	(DWORD) ((severity)|(1<<29)|(0<<28)|(facility<<16)|(exception<<0))         \
)                                                                              \
		(Severity       ) |                                                    \
		(1         << 29) |                                                    \
		(0         << 28) |                                                    \
		(Facility  << 16) |                                                    \
		(Exception <<  0)))                                                    \
//Severity code
//MS(0) or Cust(1)
//Reserved(0)
//Facility code
//Exception code
////////////////////////////////////////////////////////////////////////////////
// MessageBox
inline void pdMb(PCTSTR szMsg) {
	TCHAR szTitle[MAX_PATH];
	GetModuleFileName(NULL, szTitle, _countof(szTitle));
	MessageBox(GetActiveWindow(), szMsg, szTitle, MB_OK);
}


////////////////////////////////////////////////////////////////////////////////
// 判斷提示/驗證巨集
//inline void pdFail(PCTSTR szMsg) {
//	pdMb(szMsg);
//	DebugBreak();
//}
// 提示失敗訊息視窗
//inline void pdAssertFail(LPCSTR file, int line, PCSTR expr) {
//	TCHAR sz[MAX_PATH];
//
//	_stprintf_s(sz, MAX_PATH, _T("File %s, line %d : %s"), file, line, expr);
//	pdFail(sz);
//}


// 偵錯模式，顯示的訊息
#ifdef _DEBUG
#define pdAssert(x) if(!(x)) pdAssertFail(__FILE__, __LINE__, #x)
#else
#define pdAssert(x)
#endif

// 任何模式，皆顯示的訊息
#ifdef _DEBUG
#define pdVerify(x) pdAssert(x)
#else
#define pdVerify(x) (x)
#endif
////////////////////////////////////////////////////////////////////////////////
// The normal HANDLE_MSG macro in WindowsX.h does not work properly for dialog
// boxes because DlgProc returns a BOOL instead of an LRESULT (like
// WndProcs). This chHANDLE_DLGMSG macro corrects the problem:
#define pdHandleDlgMsg(hWnd, msg, fn) case (msg): return(SetDlgResult(hWnd, uMsg, HANDLE_##msg((hWnd), (wParam), (lParam), (fn))))
////////////////////////////////////////////////////////////////////////////////
//設定Icon
inline void pdSetDlgIcon(HWND hWnd, int id){
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(id)));
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(id)));
}
////////////////////////////////////////////////////////////////////////////////
//通用連結程式設定
//#pragma comment(linker, "/nodefaultlib:oldnames.lib")

//支援XP/VISTA樣式
#if defined(_M_IA64)
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' procssorArchitecture='IA64' publicKeyToken='6595b64144ccf1df' language='*'"")
#endif

#if defined(_M_X64)
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.6000.0' procssorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'"")
#endif

#if defined(M_X86)
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' procssorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'"")
#endif
////////////////////////////////////////////////////////////////////////////////
//SAFE MACRO
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x!=NULL){x->Release();x = NULL;}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x!=NULL){delete x;x=NULL;}
#endif

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(x) if(x!=NULL){delete[] x;x=NULL;}
#endif

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(h) if(h!=NULL){CloseHandle(h);h=NULL;}
#endif

#ifndef SAFE_CLOSE_INVALID_HANDLE
#define SAFE_CLOSE_INVALID_HANDLE(h) if(h!=INVALID_HANDLE_VALUE){CloseHandle(h);h=INVALID_HANDLE_VALUE;}
#endif
//~SAFE MACRO
////////////////////////////////////////////////////////////////////////////////
//Failed macro
#ifndef GOTO_EXIT_IF_FAILED
#define GOTO_EXIT_IF_FAILED(hr) if(FAILED(hr)) goto Exit;
#endif

#ifndef BREAK_IF_FAILED
#define BREAK_IF_FAILED(hr) if(FAILED(hr)) break;
#endif

#ifndef RETURN_IF_FAILED
#define RETURN_IF_FAILED(hr) if(FAILED(hr)) return hr;
#endif
//~Failed macro
////////////////////////////////////////////////////////////////////////////////
#endif

