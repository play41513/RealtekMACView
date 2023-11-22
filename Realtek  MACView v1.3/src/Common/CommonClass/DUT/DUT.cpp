//#define __VERBOSE
// Module:
// Notices: Copyright(c) 2016 Jim Chien.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 3.2.2.70
// ---------------------------------------------------------------------------

#pragma hdrstop

#include "DUT.h"
#include "logging.h"
#include "Uint1.h"
// ---------------------------------------------------------------------------

#pragma package(smart_init)

DUT::DUT()
:_Mtt(false)
,_DutMaxTimeGetId(5)
,_Kind(0)
{
	pStringList = new TStringList();
	_Dev1 = _Dev2 = _T("");
	ZeroMemory(&_If1, sizeof(InfoFlash));
	ZeroMemory(&_If2, sizeof(InfoFlash));
}

DUT::~DUT() {
	pStringList->Clear();
	delete pStringList;
}

bool DUT::Reboot(){
	return (Pipe(_T("reboot")) == 0L);
}

bool DUT::MTT(String dev){
	String parameter;

	if(_Kind == 2){
		parameter = String(_T("mtt ")) + String(_T("--dev ")) + dev;
	}else{
		parameter = String(_T("mtt"));
	}

	return (Pipe(parameter) == 0L);
}

unsigned long DUT::Pipe(String cn, String fn){
//	TMemoryStream *pMemoryStream = new TMemoryStream();
//	TReplaceFlags flag;
//
//	flag<<rfReplaceAll;
//	flag<<rfIgnoreCase;
//  pStringList->StrictDelimiter = true; //空格不視為分格符(預設空格及斷行被視為分隔符)

	unsigned long dwExitCode;
	unsigned long bytesRead, bufferSize;
	HANDLE hReadPipe, hWritePipe;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;                 //安全屬性
	STARTUPINFO   si;
	AnsiString cmd, rsltBuf;
	char *pBuf;


	//初始
	dwExitCode = 255L;  // 預設失敗
	pStringList->Clear();

	if(!FileExists(fn)){
		LOG(LS_WARNING) << "Can't find Manufacturing Test Tool: " << DL_MFG_TEST;
		MessageBox(NULL, (AnsiString("Can't find Manufacturing Test Tool: ") + DL_MFG_TEST).c_str(), "WARNING", MB_ICONWARNING);
		return dwExitCode;
	}

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;

	//創建管道
	if(CreatePipe(&hReadPipe,&hWritePipe,&sa,0)){
		Sleep(WAIT_PIPLE);
	}else{
		return dwExitCode;
	}

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb   =   sizeof(STARTUPINFO);
	si.dwFlags   =   STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdError  =   hWritePipe;
	si.hStdOutput   =   hWritePipe;
	si.wShowWindow   =   SW_HIDE;

	cmd = fn + _T(" ") + cn;
	rsltBuf = "";

	#ifdef __VERBOSE
	LOG(LS_VERBOSE) << "Kind: " << _Kind << " Command: " << cmd.c_str();
	#endif

	if(!CreateProcess(
			NULL, cmd.c_str(), NULL, NULL, true, CREATE_NEW_CONSOLE,
			NULL, NULL, &si, &pi)
	){
		return dwExitCode;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess,&dwExitCode);

	if(dwExitCode != 0){
		LOG(LS_WARNING) << "Original ExitCode= " << dwExitCode;
	}

	while(true){
		if(!PeekNamedPipe(hReadPipe,NULL,0,NULL,&bufferSize,NULL)){
				LOG(LS_ERROR) << "PeekNamedPipe fail";
			break;
		}


		if(bufferSize){
			pBuf = new unsigned char[++bufferSize];
			ZeroMemory(pBuf, bufferSize);

			if(!ReadFile(hReadPipe, pBuf, bufferSize, &bytesRead, NULL)){
				delete pBuf;
				break;
			}

			if(strstr(pBuf, "MTT") != NULL){
				_Mtt = false;
			}

//			if(cn.Pos(_T("info flash")) != 0 && pBuf[0] == 'F'){
				//替換字串
				for(unsigned long i=0; i< bytesRead; i++){
					if(pBuf[i] == '\0'){
						pBuf[i] = ' ';
					}else if(pBuf[i] == '\n'){
						pBuf[i] = ' ';
					}
				}
//			}

			rsltBuf += pBuf;

			LOG_INFO() << "Pipe out: " << pBuf;

//			//          readBuf[bytesRead] = '\0';
////			pStringList->CommaText = pBuf;
//			pStringList->Delimiter = L':';
//			pStringList->DelimitedText = pBuf;

			delete [] pBuf;

//			UnicodeString tmp = readBuf;
//			StringReplace(tmp, '/0', ' ', flag);
//			pMemoryStream->Write(readBuf, bytesRead);
//			pMemoryStream->WriteBuffer(readBuf, bytesRead);
//          SplitColumns(str, sl, ',');//TStringList會自動把空格當分割符，此函式不會
		}else{
			String tmp = rsltBuf.UpperCase();

			if(tmp.Pos(_T("FAIL"))){
            	dwExitCode = 254;
			}
			pStringList->Delimiter = L'\r';
			//不指定StrictDelimiter的話，預設還會以空格及換行符號來分隔字串
			pStringList->StrictDelimiter = true;
			pStringList->DelimitedText = rsltBuf;

			for(int i=0; i<pStringList->Count; i++){
				if((pStringList->Strings[i].Trim() == L"") && (i != 0)){
					pStringList->Delete(i);
					i--;
				}
			}

//			pStringList->Delimiter = L':';
//			pStringList->DelimitedText = rsltBuf;

			#ifdef __VERBOSE
			for(int i=0; i<pStringList->Count; i++){
				LOG(LS_VERBOSE) << "infoflash:" << i << " " << AnsiString(pStringList->Strings[i]).c_str();
            }
			#endif

//			if(
//				WaitForSingleObject(pi.hProcess,0) == WAIT_OBJECT_0
//			){
//				GetExitCodeProcess(pi.hProcess,&dwExitCode);
//				#ifdef __VERBOSE
//				LOG(LS_VERBOSE) << "ExitCode= " << dwExitCode;
//				#endif
				break;
//			}
		}
	}

	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

//	pMemoryStream->Position = 0;
//	pStringList->LoadFromStream(pMemoryStream);
//	pMemoryStream->Clear();
//	delete pMemoryStream;

	if(dwExitCode != 0){
		LOG(LS_WARNING) << "Kind: " << _Kind << " Command fail: " << cmd.c_str();
		LOG(LS_WARNING) << "ExitCode= " << dwExitCode;
	}

	return dwExitCode;
}

bool DUT::IsMtt(){
	return _Mtt;
//	bool bl = false;
//
//	if(_Kind == 1){
//		if(_Dev1.Pos(_T("DL")) != 0){
//			bl = true;
//		}else{
//
//		}
//	}else if(_Kind == 2){
//		if(_Dev1.Pos(_T("DL")) != 0 && _Dev2.Pos(_T("DL")) != 0){
//			bl = true;
//		}
//	}
//
//	return bl;
}

void DUT::SetMtt(bool mtt){
	_Mtt = mtt;
}

bool DUT::SetInfoFlash(InfoFlash *if0){
	bool bl;
	TStringList *psl = new TStringList();
	int nextIndex, rslt;

	psl->StrictDelimiter = true;
	psl->Delimiter = L':';

	rslt=-1;
	nextIndex=-1;
	if0->FirmwareType = "";
	AnsiString aaa = pStringList->Strings[0];
	for(int i=0; i<pStringList->Count; i++){
		if(pStringList->Strings[i].Pos(L"Firmware Type") != 0){
			psl->DelimitedText = pStringList->Strings[i];
			if0->FirmwareType = psl->Strings[1].Trim();
			nextIndex = i;
			break;
		}
	}

	if(nextIndex == pStringList->Count){
		bl = false;
		LOG(LS_WARNING) << "flash info miss fields below: [Firmware Type]";
		MessageBox(NULL, "flash info miss fields below: [Firmware Type]", "WARNING", MB_ICONWARNING);
	}else if(nextIndex == rslt){
		bl = false;
		LOG(LS_WARNING) << "flash info miss field: [Firmware Type]";
		MessageBox(NULL, "flash info miss: [Firmware Type]", "WARNING", MB_ICONWARNING);
	}else{
		bl = true;
		rslt = nextIndex;
		if0->ReleaseLevel = "";
		for(int i=nextIndex+1; i<pStringList->Count; i++){
			if(pStringList->Strings[i].Pos(L"Release Level") != 0){
				psl->Clear();
				psl->DelimitedText = pStringList->Strings[i];
				if0->ReleaseLevel = psl->Strings[1].Trim();
				nextIndex = i;
				break;
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Release Level]";
			MessageBox(NULL, "flash info miss fields below: [Release Level]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Release Level]";
			MessageBox(NULL, "flash info miss: [Release Level]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->Version = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"Version") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->Version = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Version]";
			MessageBox(NULL, "flash info miss fields below: [Version]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Version]";
			MessageBox(NULL, "flash info miss: [Version]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->SvnVersion = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"SVN Version") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->SvnVersion = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [SVN Version]";
			MessageBox(NULL, "flash info miss fields below: [SVN Version]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [SVN Version]";
			MessageBox(NULL, "flash info miss: [SVN Version]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->BuildDate = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"Build Date") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->BuildDate = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Build Date]";
			MessageBox(NULL, "flash info miss fields below: [Build Date]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Build Date]";
			MessageBox(NULL, "flash info miss: [Build Date]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->Time = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"Time") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->Time = psl->Strings[1].Trim()+
								L":"+
								psl->Strings[2].Trim()+
								L":"+
								psl->Strings[3].Trim();

					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Time]";
			MessageBox(NULL, "flash info miss fields below: [Time]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Time]";
			MessageBox(NULL, "flash info miss: [Time]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->BackFirmware = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"Backup") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->BackFirmware = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Backup Firmeare Version]";
			MessageBox(NULL, "flash info miss fields below: [Backup Firmeare Version]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Backup Firmeare Version]";
			MessageBox(NULL, "flash info miss: [Backup Firmeare Version]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->Name = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"Name") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->Name = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Encrypted Name]";
			MessageBox(NULL, "flash info miss fields below: [Encrypted Name]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Encrypted Name]";
			MessageBox(NULL, "flash info miss: [Encrypted Name]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->Date = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"Date") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->Date = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Encrypted Date]";
			MessageBox(NULL, "flash info miss fields below: [Encrypted Date]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Encrypted Date]";
			MessageBox(NULL, "flash info miss: [Encrypted Date]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->MinFwVersion = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"Min FW Version") != 0){
					psl->Clear();
					//有些裝置缺':'分隔符，特殊處理
					psl->StrictDelimiter = false;
					psl->DelimitedText = pStringList->Strings[i];
					if0->MinFwVersion = psl->Strings[3].Trim();
					psl->StrictDelimiter = true;
					psl->Delimiter = L':';
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [Encrypted Min FW Version]";
			MessageBox(NULL, "flash info miss fields below: [Encrypted Min FW Version]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [Encrypted Min FW Version]";
			MessageBox(NULL, "flash info miss: [Encrypted Min FW Version]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			rslt = nextIndex;
			if0->MAC = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"MAC address") != 0){
					AnsiString aa = pStringList->Strings[i];
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					AnsiString SS;
					for(int i=1;i<psl->Count;i++){
						SS += psl->Strings[i].Trim();
					}
					if0->MAC = SS;

					nextIndex = i;
					break;
				}
			}
		}
	}


	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [MAC address]";
			MessageBox(NULL, "flash info miss fields below: [MAC address]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [MAC address]";
			MessageBox(NULL, "flash info miss: [MAC address]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			nextIndex = rslt;
			if0->SN = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"USB Serial No") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->SN = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		if(nextIndex == pStringList->Count){
			bl = false;
			LOG(LS_WARNING) << "flash info miss fields below: [USB Serial No]";
			MessageBox(NULL, "flash info miss fields below: [USB Serial No]", "WARNING", MB_ICONWARNING);
		}else if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [USB Serial No]";
			MessageBox(NULL, "flash info miss: [USB Serial No]", "WARNING", MB_ICONWARNING);
		}else{
			bl = true;
			nextIndex = rslt;
			if0->HDCP = "";
			for(int i=nextIndex+1; i<pStringList->Count; i++){
				if(pStringList->Strings[i].Pos(L"HDCP Flags") != 0){
					psl->Clear();
					psl->DelimitedText = pStringList->Strings[i];
					if0->HDCP = psl->Strings[1].Trim();
					nextIndex = i;
					break;
				}
			}
		}
	}

	if(bl){
		nextIndex = rslt;
		if0->EDID = "";
		for(int i=nextIndex+1; i<pStringList->Count; i++){
			if(pStringList->Strings[i].Pos(L"EDID") != 0){

			for(int j=i; j<pStringList->Count; j++){
				if0->EDID += pStringList->Strings[j].Trim();
                if0->EDID += "\r\n";
			}
				nextIndex = i;
				break;
			}
		}
	}

	/*if(bl){
		if(nextIndex == rslt){
			bl = false;
			LOG(LS_WARNING) << "flash info miss field: [EDID]";
			MessageBox(NULL, "flash info miss: [EDID]", "WARNING", MB_ICONWARNING);
		}
	}*/

	psl->Clear();
	psl->Free();
	return bl;
}

bool DUT::GetInfo(){
	bool bl;
	int len;
	String prefix, parameter;
	prefix = _T("info flash");
	parameter = prefix + _T(" --dev ") + _Dev1;
	//USBHIDForm->memoMsg->Lines->Add(_Dev1);
	bl = (Pipe(parameter) == 0L);
	Delay(100);
	//USBHIDForm->memoMsg->Lines->Add(parameter);
	//USBHIDForm->memoMsg->Lines->Add(bl);
	if(!bl){
		if(!_Mtt){
			//USBHIDForm->memoMsg->Lines->Add("ENTER MTT");
			if(!TryEnterMttMode()){
				return false;
			}
			Delay(100);
			parameter = prefix + _T(" --dev ") + _Dev1;
			bl = (Pipe(parameter) == 0L);
			Delay(100);
		}
	}

	if(bl){
//		_If1.DisplaylinkId = _Dev1;
		bl = SetInfoFlash(&_If1);
		//USBHIDForm->memoMsg->Lines->Add("1111");
	}

	if(_Kind == 2 && bl){
		parameter = prefix + _T(" --dev ") + _Dev2;
		bl = (Pipe(parameter) == 0L);
		//USBHIDForm->memoMsg->Lines->Add("2222");
		if(bl){
//			_If2.DisplaylinkId = _Dev2;
			bl = SetInfoFlash(&_If2);
		}
	}
	//USBHIDForm->memoMsg->Lines->Add("33333");
	return bl;
}

bool DUT::GetId(){
	bool bl;
	String dev1, dev2;

	_Dev1 = _T("");
	_Dev2 = _T("");

	for(int i=0; i<_DutMaxTimeGetId; i++){
		Sleep(1000);
//
		if(Pipe(_T("--dev list")) == 0L){
			if(pStringList->Count < _Kind){
				bl = false;
			}else{
				bl = true;
			}

			if(bl){
				dev1 = pStringList->Strings[0].Trim();

				if(dev1.Length() < 5){
					bl = false;
				}else{
					bl = true;
				}
			}

			if(bl && _Kind == 1){
				if(pStringList->Count > 1){
					dev2 = pStringList->Strings[1].Trim();

					if(dev2.Length() > 5){
						bl = false;
					}
				}

				if(bl){
					_Dev1 = dev1;
				}
			}else if(bl && _Kind == 2){
				if(pStringList->Count > 1){
					dev2 = pStringList->Strings[1].Trim();

					if(dev2.Length() < 5){
						bl = false;
					}else{
						if(pStringList->Count != _Kind){
							for(int i=_Kind; i<pStringList->Count; i++){
								pStringList->Delete(i);
							}
						}

//						pStringList->Sort();

						_Dev1 = pStringList->Strings[0].Trim();
						_Dev2 = pStringList->Strings[1].Trim();
						bl = true;
					}
				}else{
                	bl = false;
				}
			}
		}else{
        	bl = false;
		}

		if(bl){
			LOG_INFO() << "Get id successfuly on " << i + 1;
			return true;
		}

		if(i == _DutMaxTimeGetId-1){
//			LOG_INFO() << "GetId()..." << ios::dec << i;
			LOG_INFO() << "Try get id times..." << i + 1;
			LOG_INFO() << "Nunber of chip:" << _Kind << " Count of id..." << pStringList->Count;

			for(int j=0; j<pStringList->Count; j++){
				LOG_INFO() << "id" << j << "..." << AnsiString(pStringList->Strings[j]).c_str();
			}
		}
	}

	return false;
}

int DUT::GetKind(){
	//如果要擋掉chip就要在這邊if判斷式
	int kind;
	bool bl;

	_Dev1 = "";
	_Dev2 = "";
	String dev1, dev2;

	if(Pipe(_T("--dev list")) == 0L){
		if(pStringList->Count > 1){
			dev1 = pStringList->Strings[0].Trim();
			dev2 = pStringList->Strings[1].Trim();

			if(dev2.Length() < 5){
				kind = 1;
			}else{
				kind = 2;

				for(int i=2; i<pStringList->Count; i++){
					pStringList->Delete(i);
				}

//				pStringList->Sort();
				_Dev1 = pStringList->Strings[0].Trim();
				_Dev2 = pStringList->Strings[1].Trim();
			}
		}else if(pStringList->Count == 1){
			dev1 = pStringList->Strings[0].Trim();

			if(dev1.Length() < 5){
				kind = 0;
			}else{
				_Dev1 = dev1;
				kind = 1;
			}
//		}else if(pStringList->Count == 2){
//			dev1 = pStringList->Strings[0].Trim();
//			dev2 = pStringList->Strings[1].Trim();
//
//			if(dev2.Length() < 5){
//				kind = 1;
//				_Dev1 = dev1;
//			}else{
//				kind = 2;
//				pStringList->Sort();
//				_Dev1 = pStringList->Strings[0].Trim();
//				_Dev2 = pStringList->Strings[1].Trim();
//
//			}
		}else{
			kind = 0;
		}

//		pStringList->Clear();
	}else{
		kind = 0;
	}

	return kind;
}

bool DUT::DockRun(String parameter, bool checkKind){
	String prefix, cn;

	prefix = parameter + String(_T(" --dev "));
	cn = prefix + _Dev1;

	if(Pipe(cn) != 0L){
		return false;
	}

	if(_Kind == 2 && checkKind){
		if(parameter.Pos(_T("upd"))){
			cn = parameter + _T("B") + _T(" --dev ") + _Dev2;
		}else{
			cn = prefix + _Dev2;
		}

		if(Pipe(cn) != 0L){
			return false;
		}
	}

	return true;
}

bool DUT::DataBusTest(){
	return DockRun(_T("ddr data"));
}

bool DUT::AddressBusTest(){
	return DockRun(_T("ddr address 127"));
}

bool DUT::CheckerboardTest(){
	return DockRun(_T("ddr checker"));
}

bool DUT::ByteCountDataTest(){
	return DockRun(_T("ddr byte"));
}

bool DUT::EthernetLoopbackTest(){
	bool bl;
	int cnt = 0;

	for(int i=0; i<3; i++){
		bl = DockRun(_T("eth loop"), true);

		if(bl){
        	cnt++;
		}
	}

	if(cnt >= 2){
		return true;
	}

	return false;
}

bool DUT::ShowVideoBlack(){
	return DockRun(_T("video black"));
}

bool DUT::DisableVideo(){
	DockRun(_T("video disable 0"));
	DockRun(_T("video disable 1"));
	DockRun(_T("video disable 2"));
	DockRun(_T("video disable 3"));
	return true;
}

bool DUT::ShowVideo1080P(){
	//return DockRun(_T("video bitmap .\\RGB.bmp -m 1920 1080 60"));
	if(DockRun(_T("video bitmap .\\RGB.bmp -m 1920 1080 60"))){ // jim 20161102 遇到兩個head的情況
		return DockRun(_T("video bitmap .\\RGB.bmp -m 1920 1080 60 --head 1"));
	}
	return false;
}

bool DUT::ShowVideo4K(){
	/*if(DockRun(_T("video --detailedmode 4400 3840 176 88 1 2250 2160 8 10 1 300000"))){
		if(DockRun(_T("video bitmap .\\RGB4k.bmp -m 3840 2160 30 --head 0"))) // jim 20161102 遇到兩個head的情況
			return DockRun(_T("video bitmap .\\RGB4k.bmp -m 3840 2160 30 --head 1")); // jim 20161102 遇到兩個head的情況
	}else{
		LOG(LS_WARNING) << "Failed to execute video detailedmode!";
		MessageBox(NULL, "Failed to execute video detailedmode!", "DUT::ShowVideo4K", MB_ICONWARNING);
		return false;
	}*/
	if(DockRun(_T("video bw 1"))){
		Delay(300);
		if(DockRun(_T("video --detailedmode 4400 3840 176 88 1 2250 2160 8 10 1 300000 --head 0")))
		{
			Delay(300);
			return DockRun(_T("video --detailedmode 4400 3840 176 88 1 2250 2160 8 10 1 300000 --head 1"));// jim 20161102 遇到兩個head的情況
		}
	}
	else{
		LOG(LS_WARNING) << "Failed to execute video detailedmode!";
		MessageBox(NULL, "Failed to execute video detailedmode!", "DUT::ShowVideo4K", MB_ICONWARNING);
		return false;
	}
	return false;
}

bool DUT::ShowVideo4K60(){
	// jim 20161102 遇到兩個head的情況  4K 60 新的cmd
	/*if(DockRun(_T("video --detailedmode 4400 3840 296 176 1 2250 2160 72 8 1 594000"))){
		Delay(300);
		if(DockRun(_T("video bitmap .\\RGB4k.bmp -m 3840 2160 60 --head 0"))){
			Delay(300);
			if(DockRun(_T("video --detailedmode 4400 3840 296 176 1 2250 2160 72 8 1 594000 --head 0"))){
				Delay(300);
				if(DockRun(_T("video bitmap .\\RGB4k.bmp -m 3840 2160 60 --head 1"))){
					Delay(300);
					if(DockRun(_T("video --detailedmode 4400 3840 296 176 1 2250 2160 72 8 1 594000 --head 1")))
						return true;
				}
			}
		}
	}else{
		LOG(LS_WARNING) << "Failed to execute video detailedmode!";
		MessageBox(NULL, "Failed to execute video detailedmode!", "DUT::ShowVideo4K 60", MB_ICONWARNING);
		return false;
	} */
	if(DockRun(_T("video bw 1"))){
		Delay(300);
		if(DockRun(_T("video --detailedmode 4400 3840 296 176 1 2250 2160 72 8 1 594000 --head 0"))){
				if(DockRun(_T("video --detailedmode 4400 3840 296 176 1 2250 2160 72 8 1 594000 --head 1")))
					return true;
	}
	}else{
		LOG(LS_WARNING) << "Failed to execute video detailedmode!";
		MessageBox(NULL, "Failed to execute video detailedmode!", "DUT::ShowVideo4K 60", MB_ICONWARNING);
		return false;
	}
	return false;
}

bool DUT::ProgramUPD(String upd){
	if(_Dev1.Pos(_T("4301_")) != 0){
		return DockRun(upd, true);
	}else{
		return DockRun(upd, false);
	}
}

bool DUT::TryEnterMttMode(){
	bool bl;
	int kind;
	String dev1, dev2;

	dev1 = _Dev1;
	dev2 = _Dev2;
	_Dev1 = "";
	_Dev2 = "";

	bl = MTT(dev1);

	Sleep(1000);

	if(bl){
		bl = GetId();
	}

	if(_Kind == 1){
		if(bl){
			if(_Dev1.Pos("DL") == 0){
				bl = false;
			}
		}

		if(bl){
			_Mtt = true;
		}
	}else if(_Kind == 2){
		if(bl){
			if(_Dev1.Pos("DL") == 0 && _Dev2.Pos("DL") == 0){
				bl = false;
			}
		}
	}

	if(!bl){
		_Mtt = false;
		LOG(LS_WARNING) << "TryEnterMttMode() fail on DisplayLink1 kind=" << _Kind  << AnsiString(dev1).c_str();
		//MessageBox(NULL, "Cant enter MTT mode on Displaylink1", "DUT::TryEnterMttMode", MB_ICONWARNING);
	}else if(_Kind == 2){
		bl = MTT(dev2);

		Sleep(1000);

		if(bl){
			bl = GetId();
		}

		if(bl){
			if(_Dev1.Pos("DL") == 0 || _Dev2.Pos("DL") == 0){
				bl = false;
				_Mtt = false;
				LOG(LS_WARNING) << "TryEnterMttMode() fail on DisplayLink2 kind=" << _Kind  << AnsiString(dev1).c_str() << " " << AnsiString(dev2).c_str();;
				//MessageBox(NULL, "Cant enter MTT mode on Displaylink2", "DUT::TryEnterMttMode", MB_ICONWARNING);
			}else{
				_Mtt = true;
			}
		}
	}

	return bl;
}

void DUT::Delay(DWORD iMilliSeconds) // 原版delay time 用在thread裡面
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}
