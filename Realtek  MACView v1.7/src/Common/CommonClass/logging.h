// Module: logging.cpp
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose: 編程Log.
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 1.0.0.2
//---------------------------------------------------------------------------
#ifndef CLASSES_LOGGING_H__
#define CLASSES_LOGGING_H__
//---------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <sstream>
#include <Windows.h>
//---------------------------------------------------------------------------
#if defined( UNICODE ) || defined( _UNICODE )
	typedef std::wostream _tostream;
	typedef std::wostringstream _tostringstream;
	typedef std::wstring _tstring;
#else
	typedef std::ostream _tostream;
	typedef std::ostringstream _tostringstream;
	typedef std::string _tstring;
#endif
//---------------------------------------------------------------------------
// 禁用預設建構函式及operator=以避免泄漏內存，故定義一組建構函式及operator=用來
// 在private區域宣告，以禁止訪問
#define DISALLOW_COPY_AND_ASSIGN(TypeName)    \
    TypeName(const TypeName&);                    \
    void operator=(const TypeName&)


// Note that the non-standard LoggingSeverity aliases exist because they are
// still in broad use.  The meanings of the levels are:
//  LS_SENSITIVE: Information which should only be logged with the consent
//   of the user, due to privacy concerns.
//  LS_VERBOSE: This level is for data which we do not want to appear in the
//   normal debug log, but should appear in diagnostic logs.
//  LS_INFO: Chatty level used in debugging for all sorts of things, the default
//   in debug builds.
//  LS_WARNING: Something that may warrant investigation.
//  LS_ERROR: Something that should not have occurred.
enum LoggingSeverity 
{ 
    LS_SENSITIVE, 
    LS_VERBOSE,
    LS_INFO,
    LS_WARNING,
    LS_ERROR,
    INFO = LS_INFO,
    WARNING = LS_WARNING,
    LERROR = LS_ERROR 
};

// LogErrorContext assists in interpreting the meaning of an error value.
//  ERRCTX_ERRNO: the value was read from global 'errno'
//  ERRCTX_HRESULT: the value is a Windows HRESULT
enum LogErrorContext
{
    ERRCTX_NONE,
    ERRCTX_ERRNO, 
    ERRCTX_HRESULT
};

class LogMessage
{
public:
    LogMessage(const char* file, // source file name,is not the log filename.
        int line,
        LoggingSeverity sev,
        LogErrorContext err_ctx = ERRCTX_NONE,
        int err = 0,
        const TCHAR *module = NULL);

    ~LogMessage();

	_tostream& stream()
    {
		return print_stream_;
	}


    // Timestamps begin with program execution, but can be reset with this
    // function for measuring the duration of an activity, or to synchronize
    // timestamps between multiple instances.
    static void ResetTimestamps();

private:
    // These assist in formatting some parts of the debug output.
    static const TCHAR* Describe(LoggingSeverity sev);
    static const char* DescribeFile(const char* file);
    static long TimeDiff(unsigned long later, unsigned long earlier);
    static bool TimeIsBetween(unsigned long later, unsigned long middle, unsigned long earlier);
    
    // The ostream that buffers the formatted message before output
	_tostringstream print_stream_;


    // The severity level of this message
    LoggingSeverity severity_;

    // String data generated in the constructor, that should be appended to
    // the message before output.
	_tstring extra_;

    // The timestamp at which logging started.
    static unsigned long start_;

    HANDLE hFile_;

	// 禁用預設建構函式及operator=以避免泄漏內存，故定義一組建構函式及operator=用來
	// 在private區域宣告，以禁止訪問
	DISALLOW_COPY_AND_ASSIGN(LogMessage);
	CRITICAL_SECTION g_cs;
};


#define LOG_INFO() LogMessage(__FILE__, __LINE__, LS_INFO).stream()

#define LOG(sev) LogMessage(__FILE__, __LINE__, sev).stream()


// The _F version prefixes the message with the current function name.
#define LOG_F(sev) LOG(sev) << __FUNCTION__ << ": "


// PLOG and LOG_ERR attempt to provide a string description of an errno derived
// error.  LOG_ERR reads errno directly, so care must be taken to call it before
// errno is reset.
#define PLOG(sev, err) LogMessage(__FILE__, __LINE__, sev, ERRCTX_ERRNO, err).stream()

#define LOG_ERR(sev) LogMessage(__FILE__, __LINE__, sev, ERRCTX_ERRNO, errno).stream()

// LOG_GLE(M) attempt to provide a string description of the HRESULT returned
// by GetLastError.  The second variant allows searching of a dll's string
// table for the error description.
#define LOG_GLE(sev) LogMessage(__FILE__, __LINE__, sev, \
    ERRCTX_HRESULT, GetLastError()).stream()

#define LOG_GLEM(sev, mod) LogMessage(__FILE__, __LINE__, sev, \
    ERRCTX_HRESULT, GetLastError(), mod) \
    .stream()
#endif  // CLASSES_LOGGING_H__



