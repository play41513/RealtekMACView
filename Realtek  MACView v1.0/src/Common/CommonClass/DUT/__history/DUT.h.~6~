// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef DUTH
#define DUTH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <Classes.hpp>
#include <system.hpp>
#include <tchar.h>

typedef struct tagInfoFlash{
	UnicodeString FirmwareType;
	UnicodeString ReleaseLevel;
	UnicodeString Version;
	UnicodeString SvnVersion;
	UnicodeString BuildDate;
	UnicodeString Time;
	UnicodeString BackFirmware;
	UnicodeString Name;
	UnicodeString Date;
	UnicodeString MinFwVersion;
	UnicodeString MAC;
	UnicodeString SN;
	UnicodeString EDID;
	UnicodeString HDCP;
}InfoFlash;

#define WAIT_PIPLE 100
#define DL_MFG_TEST _T("dl_mfg_test.exe")

#define DUT_STRING_NOT_SET_KIND _T("Number of chip 欄位未設定!")
#define DUT_STRING_NOT_FIND _T("Can't find device! Please try again.")
#define DUT_STRING_INFO_FLASH _T("裝置資訊:")
#define DUT_STRING_NUM_OF_CHIP _T("NUMBER OF CHIP: ")

#define DUT_FT _T("Firmware Type:  ")
#define DUT_RL _T("Release Level:  ")
#define DUT_VN _T("Version:  ")
#define DUT_SV _T("SVN Version:  ")
#define DUT_BD _T("Build Date:  ")
#define DUT_TE _T("Time: ")
#define DUT_BF _T("Backup Firmware:  ")
#define DUT_EN _T("Name:  ")
#define DUT_ED _T("Date:  ")
#define DUT_EM _T("Min FW Version:  ")
#define DUT_MAC _T("MAC address:  ")
#define DUT_SN _T("USB Serial No:  ")
//#define DUT_EDID _T("EDID:  ")
#define DUT_HDCP _T("HDCP Flag:  ")

class DUT
{
private:
	bool _Mtt;
	int _DutMaxTimeGetId;
	TStringList *pStringList;
public:
	int _Kind;
	String _Dev1, _Dev2;
	InfoFlash _If1, _If2;
private:
	bool DUT::SetInfoFlash(InfoFlash *if0);
	bool DUT::DockRun(String parameter, bool checkKind = true);
	bool DUT::MTT(String dev);
	unsigned long DUT::Pipe(String cn, String fn = DL_MFG_TEST);
public:
	DUT();
	~DUT();
	//Setting
	void SetDutMaxTimeGetId(int level){_DutMaxTimeGetId = level;}

	bool DUT::IsMtt();
	bool DUT::Reboot();
	bool DUT::GetInfo();
	bool GetId();
	int DUT::GetKind();
	int GetDutMaxTimeGetId(){return _DutMaxTimeGetId;}
	void DUT::SetMtt(bool mtt);
	void DUT::SetKind(int kind){_Kind = kind;}

	//DDR Test
	bool DUT::DataBusTest();
	bool DUT::AddressBusTest();
	bool DUT::CheckerboardTest();
	bool DUT::ByteCountDataTest();

	bool DUT::EthernetLoopbackTest();
	bool DUT::ShowVideoBlack();
	bool DUT::ShowVideo1080P();
	bool DUT::ShowVideo4K();
	bool DUT::ShowVideo4K60();
	bool DUT::DisableVideo();
	bool DUT::ProgramUPD(String upd);

	bool DUT::TryEnterMttMode();
	void DUT::Delay(int iMilliSeconds);
};
#endif
