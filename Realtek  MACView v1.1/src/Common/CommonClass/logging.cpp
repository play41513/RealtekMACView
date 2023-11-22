// Module: logging.cpp
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose: 編程Log.
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 1.0.0.2

#include "logging.h"

extern "C" BOOL WINAPI IsDebuggerPresent(void);

// Program start time
unsigned long LogMessage::start_ = GetTickCount();

LogMessage::LogMessage(const char* file, int line, LoggingSeverity sev,
					   LogErrorContext err_ctx, int err, const TCHAR* module)
					   : severity_(sev)
{
	//wostream 輸出中文需變更locale為本機預設編碼
//	locale loc = locale::global(locale("") ); // 这是C++的方式
	//setlocale( LC_CTYPE, "chs" ); // 这是C的方式
	//console 平台 使用來變更代碼頁，第2個參數空白表示使用系統預設代碼頁
	_tsetlocale(LC_CTYPE,_T(""));

	InitializeCriticalSection(&g_cs);
	
    unsigned long time = TimeDiff(GetTickCount(), start_);
	print_stream_ << _T("[") << std::setfill(_T('0')) << std::setw(3) << (time / 60000)
		<< _T(":") << std::setw(2) << (time %60000)/1000
		<< _T(":") << std::setw(3) << time %1000 << std::setfill(_T(' '))
		<< _T("] ");

    DWORD id = GetCurrentThreadId();
	print_stream_ << _T("[0x") << std::setfill(_T('0')) << std::setw(8)
		<< std::hex << id << std::dec << _T("(") << id << _T(")") << _T("] ");


    print_stream_ << Describe(sev) << _T("(") << DescribeFile(file)
        << _T(":") << line << _T("): ");

    if (err_ctx != ERRCTX_NONE)
    {
		_tostringstream tmp;
		tmp << _T("[0x") << std::setfill(_T('0')) << std::hex << std::setw(8) << err << _T("]");
        switch (err_ctx)
        {
        case ERRCTX_ERRNO:
            {
				tmp << _T(" ") << strerror(err);
                break;
            }
        case ERRCTX_HRESULT: 
            {
                TCHAR msgbuf[256]={0};
                DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM;
				HMODULE hmod = GetModuleHandle(module);
				if (hmod)
                    flags |= FORMAT_MESSAGE_FROM_HMODULE;
                if (DWORD len = FormatMessage(
                    flags, hmod, err,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					msgbuf, //The output buffer cannot be larger than 64K bytes.
					sizeof(msgbuf) / sizeof(msgbuf[0]), NULL))
                {
                    while ((len > 0) &&
                        _istspace(static_cast<TCHAR>(msgbuf[len-1])))
                    {
                        msgbuf[--len] = 0;
                    }
                    tmp << _T(" ") << msgbuf;
                }
                break; 
            }

        default:
            break;
        }
		extra_ = tmp.str();
    }
}


LogMessage::~LogMessage()
{
	if (!extra_.empty())
		print_stream_ << _T(" : ") << extra_;
	print_stream_ << _T("\r\n");
	const _tstring& str = print_stream_.str();

    static bool debugger_present = (IsDebuggerPresent() != FALSE);
	if (debugger_present)
    {
        OutputDebugString(str.c_str());
    }
    else
	{
		EnterCriticalSection(&g_cs);

//		hFile_ = ::CreateFile(_T("loging.log"),GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		hFile_ = ::CreateFile(_T("loging.log"),GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

		if(INVALID_HANDLE_VALUE == hFile_){
			hFile_ = ::CreateFile(_T("loging.log"),GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);

			if(INVALID_HANDLE_VALUE != hFile_ && sizeof(TCHAR) == 2){
				DWORD unicode = 0xFEFF, dwWritten;

				::WriteFile(hFile_, &unicode, sizeof(unicode), &dwWritten, NULL);

				TCHAR tmp[] = {_T("This file can be safely removed from your computer.\r\n")};
				TCHAR tmp2[] = {_T("..................................................\r\n")};
				::SetFilePointer (hFile_, 0, NULL, FILE_END);
//				::WriteFile(hFile_, tmp, 106, &dwWritten, NULL);
				::WriteFile(hFile_, tmp, sizeof(tmp), &dwWritten, NULL);
				::SetFilePointer (hFile_, 0, NULL, FILE_END);
				::WriteFile(hFile_, tmp2, sizeof(tmp2), &dwWritten, NULL);

				FlushFileBuffers(hFile_);
			}
		}

		if(INVALID_HANDLE_VALUE != hFile_){
			unsigned long written=0L;

			::SetFilePointer (hFile_, 0, NULL, FILE_END);
			::WriteFile(hFile_, str.data(), str.size() * sizeof(TCHAR), &written, 0);

            FlushFileBuffers(hFile_);
            ::CloseHandle(hFile_);
		}

		LeaveCriticalSection(&g_cs);
		DeleteCriticalSection(&g_cs);
    }
}



void LogMessage::ResetTimestamps() 
{
    start_ = GetTickCount();
}



const TCHAR* LogMessage::Describe(LoggingSeverity sev)
{
    switch (sev)
    {
	case LS_SENSITIVE: return _T("Sensitive");
	case LS_VERBOSE:   return _T("Verbose");
	case LS_INFO:      return _T("Info");
	case LS_WARNING:   return _T("Warning");
	case LS_ERROR:     return _T("Error");
	default:           return _T("<unknown>");
    }
}

const char* LogMessage::DescribeFile(const char* file)
{
    const char* end1 = ::strrchr(file, '/');
    const char* end2 = ::strrchr(file, '\\');
    if (!end1 && !end2)
        return file;
    else
        return (end1 > end2) ? end1 + 1 : end2 + 1;
}


bool LogMessage::TimeIsBetween(unsigned long later, unsigned long middle, unsigned long earlier) 
{
    if (earlier <= later)
    {
        return ((earlier <= middle) && (middle <= later));
    } 
    else
    {
        return !((later < middle) && (middle < earlier));
    }
}


long LogMessage::TimeDiff(unsigned long later, unsigned long earlier) 
{
    unsigned long LAST = 0xFFFFFFFF;
    unsigned long HALF = 0x80000000;
    if (TimeIsBetween(earlier + HALF, later, earlier))
    {
        if (earlier <= later)
        {
            return static_cast<long>(later - earlier);
        } 
        else 
        {
            return static_cast<long>(later + (LAST - earlier) + 1);
        }
    }
    else
    {
        if (later <= earlier) 
        {
            return -static_cast<long>(earlier - later);
        } 
        else 
        {
            return -static_cast<long>(earlier + (LAST - later) + 1);
        }
    }
}



