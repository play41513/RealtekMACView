//---------------------------------------------------------------------------
#ifndef Uint1H
#define Uint1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <Mask.hpp>
#include <FileCtrl.hpp>
#include <Menus.hpp>
#include <pngimage.hpp>

#include <vector>
#include <fstream>
#include <iostream>
#include "MSGBOX.h"

#include <inifiles.hpp>
#include <systdate.h>
#include <time.h>
#include <tchar.h>
#include <string>
#include <Registry.hpp>

#include "ConstantString.h"

#include "Iphlpapi.h"
#pragma link "Iphlpapi.lib"

#include <windows.h>
//---------------------------------------------------------------------------
using namespace std;
#define CALLDLL extern "C" __declspec(dllexport)
//---------------------------------------------------------------------------
#define GET_DEV_KIND	1
#define DEV_BURN		2
#define BURN_VERIFY		3
#define BURN_RESULT		4
#define BURN_END		5
#define BURN_OVER		0
#define USB3_CHECK		8

#define BARCODE_FINISH	1
#define BARCODE_CHANEL  2


#define MEMO_MSG(String) memoMsg->Lines->Add(String)
#define labMSG(String,sColor) lblStatus->Caption=String;\
						  lblStatus->Color=sColor;\
						  MEMO_MSG(String);\
						  USBHIDForm->Refresh()
//---------------------------------------------------------------------------
class TUSBHIDForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *moMACRecord;
	TTimer *Timer1;
	TPanel *plSwitch1;
	TPanel *plMain;
	TPanel *Panel2;
	TPanel *plRight;
	TLabel *lblStatus;
	TPanel *plDebug;
	TMemo *memoMsg;
	TPanel *plLeft;
	TPanel *plReadDevData;
	TPopupMenu *PopupMenu1;
	TMenuItem *Log;
	TMenuItem *LOGFILE;
	TPanel *plLog;
	TPanel *Panel18;
	TPanel *plLOG_Path;
	TPopupMenu *PopupMenuLOG;
	TMenuItem *N1;
	TMenuItem *N2;
	TImage *ImgLogPath;
	TPanel *Panel44;
	TPanel *Panel43;
	TEdit *edtWorkOrderNumber;
	TPanel *Panel38;
	TEdit *edtEmployeeID;
	TPanel *Panel5;
	TEdit *edtModel;
	TPanel *Panel7;
	TEdit *edtWorkStation;
	TButton *btnAutoTest;
	TImage *ImageLogo;
	TPanel *plRealtek;
	TPanel *plChipName;
	TPanel *Panel12;
	TPanel *Panel37;
	TPanel *plMAC;
	TPanel *Panel40;
	TPanel *Panel41;
	TPanel *plLED_SEL_CFG;
	TPanel *Panel45;
	TPanel *Panel46;
	TPanel *plUSBSN;
	TPanel *Panel48;
	TPanel *Panel49;
	TPanel *plEEPROM_EN;
	TPanel *Panel51;
	TPanel *Panel52;
	TPanel *plSPI_FLASH_EN;
	TPanel *Panel1;
	TGroupBox *GroupBox3;
	TBitBtn *btnLoadDutVersion;
	TPanel *Panel4;
	TPanel *Panel54;
	TEdit *edtSPI_FLASH_EN;
	TPanel *Panel56;
	TPanel *Panel57;
	TEdit *edtEEPROM_EN;
	TPanel *Panel59;
	TPanel *Panel60;
	TEdit *edtLED_SEL_CFG;
	TGroupBox *GroupBox1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TCheckBox *ckbRealtekDocking;
	TMaskEdit *mskedtBurnSn;
	TMaskEdit *mskedBurntMacPre;
	TMaskEdit *mskedtBurnMac;
	TCheckBox *ckbBarcodeVerify;
	TCheckBox *ckbBarcode;
	TCheckBox *ckbBurnMAC;
	TCheckBox *ckbAutoIncrease;
	TBitBtn *btnGetDutVersion;
	TPanel *plCharacteristics;
	TPanel *Panel8;
	TPanel *Panel9;
	TPanel *Panel13;
	TPanel *Panel16;
	TPanel *Panel20;
	TPanel *Panel23;
	TPanel *Panel26;
	TPanel *Panel29;
	TPanel *Panel32;
	TPanel *ckbSPI_FLASH_EN;
	TPanel *Panel35;
	TPanel *Panel42;
	TPanel *plSetting_SPI_FLASH_EN;
	TPanel *plSetting_EEPROM_EN;
	TPanel *plSetting_NO_REMOTE_WAKEUP;
	TPanel *Panel25;
	TPanel *plSetting_ECM_FLOW_CONTROL;
	TPanel *plSetting_LAN_DIS;
	TPanel *plSetting_GFC;
	TPanel *Panel11;
	TPanel *plSetting_LINK_OK;
	TPanel *Panel53;
	TPanel *plSetting_LED_SEL_CFG;
	TEdit *edtSetNWAY_LINK_CAPA;
	TPanel *ckbLPM_BESL_EN;
	TPanel *ckbEEPROM_EN;
	TPanel *ckbNO_REMOTE_WAKEUP;
	TPanel *ckbBOS_DESC_SUPERSPEED;
	TPanel *ckbECM_FLOW_CONTROL;
	TPanel *ckbLAN_DIS;
	TPanel *ckbLINK_OK;
	TPanel *ckbGPHY_FLOW_CONTROL;
	TPanel *Panel6;
	TEdit *edtPassWord;
	TBitBtn *btnSet;
	TMaskEdit *edtSetLED_SEL_CFG;
	TPanel *Panel3;
	TPanel *Panel10;
	TEdit *edtDocking;
	TPanel *Panel14;
	TPanel *Panel17;
	TPanel *plEFUSE_DOCKING;
	TPanel *plSwitch2;
	TPanel *edtBarcodeMain;
	TPanel *plBarcode;
	TLabel *lbBarcode;
	TPanel *Panel15;
	TBitBtn *btnBarcodeChanel;
	TBitBtn *btnBarcodeClear;
	TEdit *edtBarcodeMAC;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnAutoTestClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall plSwitch1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall LogClick(TObject *Sender);
	void __fastcall LOGFILEClick(TObject *Sender);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall N2Click(TObject *Sender);
	void __fastcall edtWorkOrderNumberExit(TObject *Sender);
	void __fastcall edtWorkOrderNumberChange(TObject *Sender);
	void __fastcall ckbBarcodeClick(TObject *Sender);
	void __fastcall ckbBarcodeVerifyClick(TObject *Sender);
	void __fastcall btnRealtekLoadClick(TObject *Sender);
	void __fastcall btnSetClick(TObject *Sender);
	void __fastcall mskedtBurnMacChange(TObject *Sender);
	void __fastcall ckbLPM_BESL_ENClick(TObject *Sender);
	void __fastcall edtPassWordKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtPassWordEnter(TObject *Sender);
	void __fastcall edtSetLED_SEL_CFGExit(TObject *Sender);
	void __fastcall btnBarcodeChanelClick(TObject *Sender);
	void __fastcall edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtBarcodeMACChange(TObject *Sender);
	void __fastcall btnBarcodeClearClick(TObject *Sender);


private:	// User declarations
	int getKindCnt;
	bool bDDR_Checck; //DDR測試
	bool m_bAutoIncrease; //燒錄成功

	AnsiString LOG_PATH;
	AnsiString DISK_NAME;
	AnsiString g_CFGFile;

	void Delay(DWORD iMilliSeconds);
	bool GetDutInfo(bool bLOAD,bool bBurnVerify);
	void ClearDevInfo(bool bLOAD);
	bool writeLOG(AnsiString Msg,bool bAction);
	void SaveLogLine(AnsiString FileName,AnsiString FileLine);
	AnsiString GetStringLine(AnsiString String,AnsiString Keyword);
	bool Analyze_USBView(AnsiString VID,AnsiString PID);
	bool CheckChipUSB3();
	bool FindDevice();
	void SetUIEnabled(bool bEnabled,bool bSet);
	//realtek
	void Realtek_Chip_Burn();
	bool UpdateRealtekCFGFile(AnsiString MAC);
	bool CallCommand_S(AnsiString filename, AnsiString cmd,
	AnsiString path);
	AnsiString Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum);
	AnsiString DosCommand(AnsiString sCmdline);
	bool DUTV_Efuse();
protected:
	void AutoIncrease();  // 自動跳號
public:		// User declarations
	__fastcall TUSBHIDForm(TComponent* Owner);

	int  __fastcall HexToInt(AnsiString HexStr);
	char __fastcall DUTB(void);
	char __fastcall DUTV(void);

	//==============================================================
};
//---------------------------------------------------------------------------
extern PACKAGE TUSBHIDForm *USBHIDForm;
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
