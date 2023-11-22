// Module: AsyncIO.h
// Notices: Copyright(c) 2014 Robbin Hsiao.
// Purpose: 非同步IO讀寫物件.
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 1.0.2
//---------------------------------------------------------------------------
#ifndef AsyncIOH
#define AsyncIOH
//---------------------------------------------------------------------------
#include <tchar.h>
//---------------------------------------------------------------------------
class Report{
private:
	unsigned long _cbBuffer;
	void *_pvBuffer;
protected:
	bool SetBuffer(const void *pBuffer){
		if(_pvBuffer){
//			for(int i=0; i<_cbBuffer; i++){
//				_pvBuffer[i] = pBuffer[i];
//			}
			memcpy(_pvBuffer, pBuffer, _cbBuffer);
			return true;
		}
		return false;

	}

	void FreeBuffer(){
		if(_pvBuffer != NULL){
			VirtualFree(_pvBuffer, 0, MEM_RELEASE);
			_pvBuffer = NULL;
		}
	}

	bool AllocBuffer(){
		FreeBuffer();
		_pvBuffer = VirtualAlloc(NULL, _cbBuffer, MEM_COMMIT, PAGE_READWRITE);
		return (_pvBuffer != NULL);
	}
public:
	void* GetBuffer(){
		return _pvBuffer;
	}

	Report(unsigned cbBuffer, const void *buffer):_cbBuffer(cbBuffer), _pvBuffer(NULL){
		AllocBuffer();
		SetBuffer(buffer);
	};
	Report(unsigned cbBuffer):_cbBuffer(cbBuffer), _pvBuffer(NULL){
		AllocBuffer();
	};
	~Report(){
        FreeBuffer();
	};
};

class AsyncIO : public OVERLAPPED
{
private:
	Report *_prpt;
protected:
	void FreeReport(Report **prt){
		if(*prt){
			delete *prt;
			*prt = NULL;
		}
	}
public:
	bool Read(HANDLE hDevice, unsigned long cbBuffer, int waitTime, PLARGE_INTEGER pliOffset = NULL){
		//配置 OVERLAPPED
		if(pliOffset != NULL){
			Offset = pliOffset->LowPart;
			OffsetHigh = pliOffset->HighPart;
		}

		//配置 InputReport
		FreeReport(&_prpt);
		_prpt = new Report(cbBuffer);

		if(ReadFile(hDevice, _prpt->GetBuffer(), cbBuffer, NULL, this)){
			return true;
		}else{
			if(GetLastError() == ERROR_IO_PENDING){
				WaitForSingleObject(hDevice, waitTime);

				if(GetOverlappedResult(hDevice, this, &cbBuffer, false)){
					return true;
				}else{
					CancelIo(hDevice);
				}
			}
		}

		FreeReport(&_prpt);
		return false;
	}

	bool Write(HANDLE hDevice, unsigned long cbBuffer, const void *buffer, int waitTime, PLARGE_INTEGER pliOffset = NULL){
        //配置 OVERLAPPED
		if(pliOffset != NULL){
			Offset = pliOffset->LowPart;
			OffsetHigh = pliOffset->HighPart;
		}

		//配置 OutputReport
		FreeReport(&_prpt);
		_prpt = new Report(cbBuffer, buffer);

		if(WriteFile(hDevice, _prpt->GetBuffer(), cbBuffer, NULL, this)){
			return true;
		}else{
			if(GetLastError() == ERROR_IO_PENDING){
				WaitForSingleObject(hDevice, waitTime);

				if(GetOverlappedResult(hDevice, this, &cbBuffer, false)){
					return true;
				}else{
					CancelIo(hDevice);
				}
			}
		}

		FreeReport(&_prpt);
		return false;
	}

	AsyncIO(size_t nBufSize){
		AsyncIO();
		_prpt = new Report(nBufSize);
	}

	AsyncIO(){
		Internal = InternalHigh = 0;
		Offset = OffsetHigh = 0;
		hEvent = NULL;
		_prpt = NULL;
	}

	~AsyncIO(){
		if(_prpt){
        	delete _prpt;
		}
	}

//	BOOL WINAPI CancelIo(_In_  HANDLE hFile);
};
//---------------------------------------------------------------------------
#endif
