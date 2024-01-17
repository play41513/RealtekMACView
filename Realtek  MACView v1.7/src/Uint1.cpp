﻿// Module:
// Notices: Copyright(c) 2019 Leno
// Purpose:
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 1.0.0
// --------------------------------------------------------------------------
#include <vcl.h>
#include <vfw.h>
#include <direct.h>
#include <stdio.h>
#pragma hdrstop

#include "Uint1.h"
#include "USBDevConnectionInfo.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Gauges"
#pragma resource "*.dfm"
TUSBHIDForm *USBHIDForm;
// ---------------------------------------------------------------------------
char autoERRC, autoSTEP;
bool G_bCheckDevInfo = false;
bool bPlugIn = false;
bool bStart = false;


AnsiString MAC_FILE_PATH  	= "RealtekMAC\\2015\\";
AnsiString MAC_LOG_PATH  	= "RealtekMAC\\2015\\pgtool.log";

AnsiString ErrorMsg="",Tool_Ver="";
DWORD TestTime = 0;
DWORD numBarcodeResult,dwLeft,dwRight;
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();

__fastcall TUSBHIDForm::TUSBHIDForm(TComponent* Owner) : TForm(Owner)
{

}
void __fastcall TUSBHIDForm::FormShow(TObject *Sender)
{
	//
	Caption = APP_TITLE;
	//	FindIniFile();	dwLeft = plLeft->Width;	//
	ShowSettingForm();
	edtBarcodeMain->Height = 0;
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::FormClose(TObject *Sender, TCloseAction &Action) {
	SetIniFile();
	// 寫入Config檔案
	AnsiString totalmsg = "[DEV_NAME]\n"+cbDevName->Text;
	ofstream oConfigFile("INI\\Config.ini");
	oConfigFile << totalmsg.c_str();
	oConfigFile.close();
}
// ---------------------------------------------------------------------------
char __fastcall TUSBHIDForm::DUTB(void) {
	int ERRC = 0;
	AnsiString SN, MAC,SS;
	char macbuf[18];
	//memoMsg->Lines->Add(BURN_DUT);
	// -----------------------------------------------------upd flash
	if (!ERRC)
	{
		labMSG(MSG_DUT_START_BURN,clYellow);
		//MAC
		SS = mskedBurntMacPre->Text;
		strncpy(&macbuf[0], SS.c_str(), 6);
		SS = mskedtBurnMac->Text;
		strncpy(&macbuf[6], SS.c_str(), 6);
		macbuf[17] = 0x00;
		macbuf[16] = macbuf[11];
		macbuf[15] = macbuf[10];
		macbuf[14] = ' ';
		macbuf[13] = macbuf[9];
		macbuf[12] = macbuf[8];
		macbuf[11] = ' ';
		macbuf[10] = macbuf[7];
		macbuf[9] = macbuf[6];
		macbuf[8] = ' ';
		macbuf[7] = macbuf[5];
		macbuf[6] = macbuf[4];
		macbuf[5] = ' ';
		macbuf[4] = macbuf[3];
		macbuf[3] = macbuf[2];
		macbuf[2] = ' ';
		macbuf[1] = macbuf[1];
		macbuf[0] = macbuf[0];
		MAC = macbuf;
		if(UpdateRealtekCFGFile(MAC))
		{
			CallCommand_S(MAC_TOOL,CMD_BURN_MAC,MAC_FILE_PATH);
			ERRC = Findfilemsg(MAC_LOG_PATH, BURN_SUCCESSFUL,0) == "" ? 0x01:0x00;
			if(Findfilemsg(MAC_LOG_PATH, RUN_AGAIN_TOOL,0) != "")
			{
				Delay(1000);
				ERRC = Findfilemsg(MAC_LOG_PATH, BURN_SUCCESSFUL,0) == "" ? 0x01:0x00;
			}
		}
		else  ERRC = 0x01;
	}
	if (ERRC)
	{
		if(Findfilemsg(MAC_LOG_PATH, EFUSE_SPCAE_FULL,0) != "")
		{
			labMSG(MSG_EFUSE_SPACE_FULL,clRed);
			ErrorMsg = MSG_EFUSE_SPACE_FULL;
		}
		else
		{
			labMSG(MSG_BURN_ERROR,clRed);
			ErrorMsg = MSG_BURN_ERROR;
		}
	}
	return(ERRC);
}

// ---------------------------------------------------------------------------
char __fastcall TUSBHIDForm::DUTV(void) {
	int ERRC =0x00;
	UnicodeString DUT_MAC,DUT_SN,DUT_SPI_FLASH_EN,DUT_EEPROM_EN,DUT_LED_SEL_CFG;
	UnicodeString refMac = mskedBurntMacPre->Text + mskedtBurnMac->Text;
	labMSG(VERIFY_DUT_EFUSE,clYellow);
	//讀取晶片info flash
	ERRC = GetDutInfo(false,ckbBurnMAC->Checked) ? 0x00:0x40;
	if(!ERRC)
	{
		//讀取成功
		if(ckbBarcodeVerify->Checked)
		{
			numBarcodeResult = 0;
			edtBarcodeMAC->Text = "";
			edtBarcodeMain->Height = 160;
			edtBarcodeMain->Top = USBHIDForm->Height/2 - 80;
			edtBarcodeMain->Left = USBHIDForm->Width/2 - edtBarcodeMain->Width/2;
			lbBarcode->Caption = "請掃描MAC燒錄條碼";
			edtBarcodeMAC->SetFocus();
			while(true)
			{
				if(numBarcodeResult == BARCODE_FINISH)
				{
					edtBarcodeMain->Height = 0;
					ERRC =  0x00;
					break;
				}
				if(numBarcodeResult == BARCODE_CHANEL)
				{
					edtBarcodeMain->Height = 0;
					ERRC =  0x40;
					if(ERRC)
					{
						labMSG("(!)Barcode輸入異常",clRed);
						ErrorMsg = "(!)Barcode輸入異常";
					}
					break;
				}
				Delay(100);
			}
			refMac = mskedBurntMacPre->Text + mskedtBurnMac->Text;
		}
	}
	else
		ERRC = 0xFF;
	/*if(Findfilemsg(MAC_LOG_PATH,EFUSE_SPACE_NULL, 0)!="")
	{
		ERRC = 0xFF;
		labMSG(MSG_EFUSE_SPACE_NULL,clRed);
		ErrorMsg = MSG_EFUSE_SPACE_NULL;
	}*/
	//---------------------------------------------------------Verify
	if (!ERRC)
	{
		if(ckbBurnMAC->Checked || ckbBarcodeVerify->Checked)
		{   //比對SN
			memoMsg->Lines->Add(VERIFY_DUT_SERIAL);
			if(plUSBSN->Caption != mskedtBurnSn->Text){
				plUSBSN->Font->Color = clRed;;
				labMSG(MSG_SN_ERROR,clRed);
				ErrorMsg = MSG_SN_ERROR+mskedtBurnSn->Text;
				ERRC = 0x40;
			}
			//比對MAC
			memoMsg->Lines->Add(VERIFY_DUT_MAC);
			if(plMAC->Caption != refMac){
				plMAC->Font->Color = clRed;
				labMSG(MSG_MAC_ERROR,clRed);
				ErrorMsg += MSG_MAC_ERROR+refMac;
				ERRC = 0x40;
			}
		}
		//
		labMSG(VERIFY_DUT_EFUSE,clYellow);
		memoMsg->Lines->Add(VERIFY_DUT_EFUSE);
		if(plSPI_FLASH_EN->Caption != edtSPI_FLASH_EN->Text){
			plSPI_FLASH_EN->Font->Color = clRed;
			plSetting_SPI_FLASH_EN->Color = clRed;
			labMSG(MSG_SPI_FLASH_EN_ERROR,clRed);
			ErrorMsg +=MSG_SPI_FLASH_EN_ERROR+plSPI_FLASH_EN->Caption;
			ERRC = 0x40;
		}
		if(plEEPROM_EN->Caption != edtEEPROM_EN->Text){
			plEEPROM_EN->Font->Color = clRed;
			plSetting_EEPROM_EN->Color = clRed;
			labMSG(MSG_EEPROM_EN_ERROR,clRed);
			ErrorMsg +=MSG_EEPROM_EN_ERROR+plEEPROM_EN->Caption;
			ERRC = 0x40;
		}
		if(plLED_SEL_CFG->Caption != edtLED_SEL_CFG->Text){
			plLED_SEL_CFG->Font->Color = clRed;
			plSetting_LED_SEL_CFG->Color = clRed;
			labMSG(MSG_LED_SEL_CFG_ERROR,clRed);
			ErrorMsg +=MSG_LED_SEL_CFG_ERROR+plLED_SEL_CFG->Caption;
			ERRC = 0x40;
		}
		if(plEFUSE_DOCKING->Caption != edtDocking->Text){
			plEFUSE_DOCKING->Font->Color = clRed;
			labMSG(MSG_DOCKING_ERROR,clRed);
			ErrorMsg +=MSG_DOCKING_ERROR+plEFUSE_DOCKING->Caption;
			ERRC = 0x40;
		}
		if(!DUTV_Efuse())
		{
			lblStatus->Font->Size = 18;
			labMSG(MSG_EFUSE_ERROR,clRed);
			ErrorMsg +=MSG_EFUSE_ERROR;
			ERRC = 0x40;
        }
		//
		if(!ERRC)
		{
			labMSG(MSG_VERIFY_FINISH,clYellow);
		}
	}
	return(ERRC);
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnAutoTestClick(TObject *Sender) {
	if(btnAutoTest->Caption=="Start")
	{
		if(edtSPI_FLASH_EN->Text == ""
			|| edtEEPROM_EN->Text == ""
			|| edtLED_SEL_CFG->Text == ""
			|| edtDocking->Text == "")
			MessageBoxA(NULL,MSG_LOAD_ERROR.c_str(),"WARNING", MB_ICONEXCLAMATION);
		else
		{
			writeLOG("",true);

			SetUIEnabled(false,false);
			labMSG(MSG_WAIT_DUT,clSkyBlue);
			btnAutoTest->Caption = "Stop";
			bStart = true; //Timer測試FUNCTION啟動
		}
	}
	else
	{
		bStart = false;

		SetUIEnabled(true,false);
		ckbBarcodeClick(NULL);
		ckbBarcodeVerifyClick(NULL);
		btnAutoTest->Caption="Start";
		bPlugIn = false;
	}
}

// ---------------------------------------------------------------------------
void TUSBHIDForm::Realtek_Chip_Burn()
{
	DWORD TimeOUT = 0;
	while(autoSTEP)
	{
		Delay(200);
		int kind;
		if(btnAutoTest->Caption=="Start")
		{
			labMSG(MSG_WAIT_DUT,clSkyBlue);
			break;
		}
		if(!G_bCheckDevInfo)
		{
			switch(autoSTEP)
			{
				case GET_DEV_KIND:
					if(FindDevice())
					{
						lblStatus->Font->Size = 32;
						btnAutoTest->Enabled = false;
						ErrorMsg = "PASS";
						autoERRC = 0;
						autoSTEP = USB3_CHECK;
						memoMsg->Lines->Clear();
						labMSG(MSG_DUT_READY,clYellow);
						/*if(ckbBurnMAC->Checked)
						  autoSTEP = DEV_BURN;
						else autoSTEP = BURN_VERIFY;  */
					}
					break;
				case USB3_CHECK:
					if(CheckChipUSB3())
					{
						if(ckbBurnMAC->Checked)
						  autoSTEP = DEV_BURN;
						else autoSTEP = BURN_VERIFY;
					}
					else
					{
						autoERRC = 0x40;
						labMSG(MSG_USB3_ERROR,clRed);
						ErrorMsg = MSG_USB3_ERROR;
						TimeOUT = GetTickCount()+3000;
						autoSTEP = CHECK_DUT;
					}
					break;
				case DEV_BURN:
					labMSG(MSG_DUT_READY_BURN,clYellow);
					if(ckbBarcode->Checked)
					{
						numBarcodeResult = 0;
						edtBarcodeMAC->Text = "";
						edtBarcodeMain->Height = 160;
						edtBarcodeMain->Top = USBHIDForm->Height/2 - 80;
						edtBarcodeMain->Left = USBHIDForm->Width/2 - edtBarcodeMain->Width/2;
						lbBarcode->Caption = "請掃描MAC燒錄條碼";
						edtBarcodeMAC->SetFocus();
						while(true)
						{
							if(numBarcodeResult == BARCODE_FINISH)
							{
								edtBarcodeMain->Height = 0;
								switch(CheckMAC(USBHIDForm->mskedBurntMacPre->Text+USBHIDForm->mskedtBurnMac->Text))
								{
									case ERROR_MAC_OUTRANGE:
										autoERRC =  0x40;
										labMSG("(!)MAC超出範圍設定",clRed);
										ErrorMsg = "(!)MAC超出範圍設定";
										break;
									case ERROR_MAC_REPEAT:
										autoERRC =  0x40;
										labMSG("(!)此MAC已燒錄過",clRed);
										ErrorMsg = "(!)此MAC已燒錄過";
										break;
									default:
										autoERRC = DUTB();
										break;
								}
								break;
							}
							if(numBarcodeResult == BARCODE_CHANEL)
							{
								edtBarcodeMain->Height = 0;
								autoERRC =  0x40;
								labMSG("(!)Barcode輸入異常",clRed);
								ErrorMsg = "(!)Barcode輸入異常";
								break;
							}
							Delay(100);
						}
					}
					else
						autoERRC = DUTB();

					if (autoERRC) {
						if(ErrorMsg == "PASS")
						{
							labMSG(MSG_BURN_ERROR,clRed);
							ErrorMsg = MSG_BURN_ERROR;
						}
						autoSTEP = CHECK_DUT;
						TimeOUT = GetTickCount()+3000;
						break;
					}

					autoSTEP = BURN_VERIFY;
					break;
				case BURN_VERIFY:
					labMSG(MSG_DUT_READY_VERIFY,clYellow);
					autoERRC = DUTV();
					autoSTEP = CHECK_DUT;
					TimeOUT = GetTickCount()+3000;
					Delay(1000);
					break;
				case CHECK_DUT:
					if(!FindDevice()&&GetTickCount()>TimeOUT)
					{
						labMSG(MSG_NOT_FIND_DUT,clRed);
						ErrorMsg = MSG_NOT_FIND_DUT;
						autoERRC = 0x40;
						autoSTEP = BURN_RESULT;
					}
					else	autoSTEP = BURN_RESULT;
					break;
				case BURN_RESULT:
					if(!writeLOG(ErrorMsg,false))
					{
						autoERRC = 0x40;
						labMSG("(!)LOG寫入失敗",clRed);
						ErrorMsg = "(!)寫檔失敗";
					}
					if (autoERRC)
					{
						if(lblStatus->Color!= clRed)
						{
							labMSG(MSG_ERROR,clRed);
							ErrorMsg = MSG_ERROR;
						}
					}
					else {
						labMSG("PASS",clGreen);
						m_bAutoIncrease = true;
					}
					//
					autoSTEP = DELAY_DUT_STATUS;
					TimeOUT = GetTickCount()+2000;

					break;
				case DELAY_DUT_STATUS:
					if(GetTickCount()>TimeOUT)
					{
						autoSTEP = BURN_END;
					}
					break;
				case BURN_END:
					if(autoERRC){
						MessageBeep(MB_ICONERROR);
					}else{
						MessageBeep(MB_OK);
					}
					btnAutoTest->Enabled = true;
					autoSTEP = BURN_OVER;
					break;
			} //End switch case.
		}
	}
}

void TUSBHIDForm::AutoIncrease(){
	AnsiString SS, SD, SN, MAC;
	int tmp;

	if(ckbAutoIncrease->Checked && m_bAutoIncrease && !ckbBarcode->Checked) {
		m_bAutoIncrease = false;

		tmp = mskedtBurnSn->Text.ToInt();
		if(ckbBurnMAC->Checked )
		{
			tmp++;
			SS = IntToStr(tmp);
			SD = "";

			for (int j = 0; j < (6 - SS.Length()); j++) {
				SD += "0";
			}

			SD += SS.c_str();
			mskedtBurnSn->Text = SD;
		}
		tmp = HexToInt(mskedtBurnMac->Text);
		if(ckbBurnMAC->Checked )
		{
			tmp++;
			SD = IntToHex(tmp, 6);
			mskedtBurnMac->Text = SD;
		}
	}
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::GetDutInfo(bool bLOAD,bool bBurnVerify)
{
	btnAutoTest->Enabled = false;
	ClearDevInfo(bLOAD);
	labMSG(MSG_GETTING_DUT_DATA,clYellow);

	bool bResult = true;
	G_bCheckDevInfo = true;
	AnsiString MAC,SN;
	if(!bBurnVerify)
	{    //Get DUT eFuse Data
		DeleteFile(MAC_LOG_PATH);
		CallCommand_S(MAC_TOOL, CMD_GET_DEV_DATA, MAC_FILE_PATH);
		if(Findfilemsg(MAC_LOG_PATH, RUN_AGAIN_TOOL,0) != "")
			CallCommand_S(MAC_TOOL, CMD_GET_DEV_DATA, MAC_FILE_PATH);
		if(Findfilemsg(MAC_LOG_PATH, ANSI_NOT_FIND_DUT,0) != ""
			||Findfilemsg(MAC_LOG_PATH,BURN_SUCCESSFUL, 0)=="")
		{

			for(int i =0;i<3;i++)
			{
				labMSG(MSG_GETTING_DUT_DATA+AnsiString(i+1),clYellow);
				Delay(1000);
				CallCommand_S(MAC_TOOL, CMD_GET_DEV_DATA, MAC_FILE_PATH);
				if(Findfilemsg(MAC_LOG_PATH, BURN_SUCCESSFUL,0) != "")
					break;
			}
		}
	}
	AnsiString MSG;
	//
	if(Findfilemsg(MAC_LOG_PATH,BURN_SUCCESSFUL, 0) != "")
	{
		if(Findfilemsg(MAC_LOG_PATH,EFUSE_SPACE_NULL, 0)!=""&&bLOAD)
				MB_WARNING(NULL,MSG_EFUSE_SPACE_NULL.c_str());
		//NODEID
		MSG = Findfilemsg(MAC_LOG_PATH, "NODEID =", 0);
		if(MSG.Length())
		{
			MAC = MSG.SubString(MSG.Pos("= ")+2,MSG.Length() - MSG.Pos("= ")-1);
			MAC = StringReplace(MAC, " ", "", TReplaceFlags()<<rfReplaceAll);
			plMAC->Caption = MAC;
		}
		else plMAC->Caption = "000000000000";
		// SPI_FLASH_EN
		MSG = Findfilemsg(MAC_LOG_PATH, "SPI_FLASH_EN =", 0);
		if(MSG.Length())
			plSPI_FLASH_EN->Caption = AnsiString(MSG).SubString(22,2);
		else
			plSPI_FLASH_EN->Caption = "NULL";
		if(cbDevName->Text.Pos("IA-CED5"))
			plSPI_FLASH_EN->Caption = "NULL";
		if(bLOAD)
		{
			edtSPI_FLASH_EN->Text = plSPI_FLASH_EN->Caption;
		}
		// SPI_EEPROM_EN
		MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_EEPROM_EN =", 0);
		if(MSG.Length())
			plEEPROM_EN->Caption = AnsiString(MSG).SubString(19,2);
		else
			plEEPROM_EN->Caption = "NULL";
		if(bLOAD)
		{
			edtEEPROM_EN->Text = plEEPROM_EN->Caption;
		}
		// LED_SEL_CFG
		MSG = Findfilemsg(MAC_LOG_PATH, "LED_SEL_CFG =", 0);
		MSG = MSG.Delete(1,MSG.Pos("= "));
		MSG = StringReplace(MSG, " ", "", TReplaceFlags()<<rfReplaceAll);
		plLED_SEL_CFG->Caption = AnsiString(MSG);
		{
			if(bLOAD) edtLED_SEL_CFG->Text = plLED_SEL_CFG->Caption;
			edtSetLED_SEL_CFG->Text = edtLED_SEL_CFG->Text;
		}
		// EFUSE_DOCKING
		MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_DOCKING =", 0);
		if(MSG.Length())
			plEFUSE_DOCKING->Caption = AnsiString(MSG).SubString(17,2);
		else
        	plEFUSE_DOCKING->Caption = "NULL";
		if(bLOAD) edtDocking->Text = plEFUSE_DOCKING->Caption;
		// SN
		MSG = Findfilemsg(MAC_LOG_PATH, "SN =", 0);
		if(MSG.Length())
		{
			MSG = MSG.Delete(1,MSG.Pos("= "));
			MSG = StringReplace(MSG, " ", "", TReplaceFlags()<<rfReplaceAll);
			plUSBSN->Caption = MSG;
		}
	}
	else
	{
		MB_WARNING(NULL,MSG_NOT_FIND_DUT.c_str());
		bResult = false;
	}


	G_bCheckDevInfo = false;
	if(btnSet->Name.Pos("Enter"))
		btnAutoTest->Enabled = true;

	return bResult;
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::Timer1Timer(TObject *Sender)
{
	if(btnAutoTest->Caption.Pos("Start")&&btnAutoTest->Enabled==false&&lblStatus->Caption=="等候中"&&btnSet->Caption.Pos("Enter"))
	{
		bStart = false;

		//btnLoadDutVersion->Enabled = true;
		//mskedtBurnSn->Enabled = true;
		if(!ckbBarcode->Checked)
		{
			ckbAutoIncrease->Enabled = true;
			ckbBurnMAC->Enabled = true;
		}
		mskedBurntMacPre->Enabled = true;
		mskedtBurnMac->Enabled = true;
		btnGetDutVersion->Enabled = true;
		btnAutoTest->Enabled = true;
		bPlugIn = false;
	}
	if(bStart)
	{
		bStart = false;
		try
		{
			if(USBCONInfo->DevNameGetVPID("vid_0bda"))
			{
				if(!bPlugIn)
				{
					TestTime = GetTickCount();
					autoSTEP = GET_DEV_KIND;
					Realtek_Chip_Burn();
				}
				bPlugIn = true;
			}
			else if(USBCONInfo->DevNameGetVPID("vid_056e"))
			{
				if(!bPlugIn)
				{
					TestTime = GetTickCount();
					autoSTEP = GET_DEV_KIND;
					Realtek_Chip_Burn();
				}
				bPlugIn = true;
			}
			else
			{
				if(m_bAutoIncrease) AutoIncrease();
				ClearDevInfo(false);
				labMSG("等候中",clSkyBlue);
				//memoMsg->Text = "No matching devices found";
				bPlugIn = false;
			}
		}
		catch(...)
		{
			//
		}
		bStart = true;
		if(ErrorMsg == "(!)寫檔失敗")
		{
			bStart = false;
			MessageBoxA(Handle,"請確認LOG隨身碟狀態","WARNING", MB_ICONEXCLAMATION);
			btnAutoTestClick(NULL);
			ShowSettingForm();
		}
	}
}
//---------------------------------------------------------------------------
void TUSBHIDForm::ClearDevInfo(bool bLOAD)
{
	plUSBSN->Caption 		= "";
	plMAC->Caption 			= "";
	plSPI_FLASH_EN->Caption = "";
	plEEPROM_EN->Caption 	= "";
	plLED_SEL_CFG->Caption 	= "";
	plEFUSE_DOCKING->Caption 	= "";
	plChipName->Caption = "Realtek";
	lblStatus->Font->Size = 32;

	plUSBSN->Font->Color 		= clBlue;
	plMAC->Font->Color 			= clBlue;
	plSPI_FLASH_EN->Font->Color = clBlue;
	plEEPROM_EN->Font->Color 	= clBlue;
	plLED_SEL_CFG->Font->Color 	= clBlue;
	plEFUSE_DOCKING->Font->Color 	= clBlue;

	plSetting_SPI_FLASH_EN->Color 		= clWhite;
	plSetting_EEPROM_EN->Color 			= clWhite;
	plSetting_NO_REMOTE_WAKEUP->Color 	= clWhite;
	plSetting_ECM_FLOW_CONTROL->Color 	= clWhite;
	plSetting_LAN_DIS->Color 			= clWhite;
	plSetting_LINK_OK->Color 			= clWhite;
	plSetting_GFC->Color 				= clWhite;
	plSetting_LED_SEL_CFG->Color 		= clWhite;
	plVID->Color						= clWhite;
	plPID->Color						= clWhite;
	plVer->Color						= clWhite;
	plManfcture->Color					= clWhite;
	plProduct->Color					= clWhite;
	plCTAPSHORT->Color					= clWhite;
	plLAN_WAKE_EN->Color				= clWhite;

	if(bLOAD)
	{
		edtSPI_FLASH_EN->Text 	= "";
		edtEEPROM_EN->Text 		= "";
		edtLED_SEL_CFG->Text 	= "";
		edtDocking->Text 		= "";
    }
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::writeLOG(AnsiString Msg,bool bAction) {
	bool bPASS[4] = {true,true,true,true};
	AnsiString time = FormatDateTime("mm-dd-yyyy hh:nn:ss", Now());
	AnsiString SS;
	if(bAction)
	{
		AnsiString Auto = ckbAutoIncrease->Checked ? "true":"false";
		AnsiString burn_mac = ckbBurnMAC->Checked ? "true":"false";
		AnsiString barcode_burn = ckbBarcode->Checked ? "true":"false";
		AnsiString barcode_verify = ckbBarcodeVerify->Checked ? "true":"false";
		AnsiString sn = mskedtBurnSn->Text;
		AnsiString mac = mskedBurntMacPre->Text+mskedtBurnMac->Text;
		AnsiString Setting ="\r\n===========\r\n "
							+time
							+"[自動跳號] "+Auto
							+"[燒錄MAC] "+burn_mac+"\r\n"
							+"[SN] "+sn+"[MAC] "+mac
							+"[Barcode MAC燒錄]"+barcode_burn
							+"[Barcode MAC驗證]"+barcode_verify
							+"[MODEL]"+cbDevName->Text
							+"[SPI_FLASH_EN]"+edtSPI_FLASH_EN->Text
							+"[EEPROM_EN]"+edtEEPROM_EN->Text
							+"[LED_SEL_CFG]"+edtLED_SEL_CFG->Text
							+"[DOCKING]"+edtDocking->Text;
		Setting+=" [操作人員工號]"+edtEmployeeID->Text+"\r\n==========\r\n";
		//
		bPASS[0] = SaveLogLine(RDLOGFilePath,Setting);
		if(LOGDiskPath.Pos("csv") && ImgDisk->Visible)
		{
		   bPASS[1] = 	SaveLogLine(RDLOGDiskPath,Setting);
		}
		for(int i = 0 ; i < 2 ; i++)
			if(!bPASS[i]) return false;
	}
	else
	{
		AnsiString sn="NULL",mac="NULL",spi_flash_en="NULL",efuse_eeprom_en="NULL",efuse_led_sel_cfg="NULL"
					,efuse_docking="NULL",TestNUM="";
		double test_time = double(GetTickCount() - TestTime)/(double)1000;
		spi_flash_en 		= plSPI_FLASH_EN->Caption;
		efuse_eeprom_en 	= plEEPROM_EN->Caption;
		efuse_led_sel_cfg	= plLED_SEL_CFG->Caption;
		efuse_docking		= plEFUSE_DOCKING->Caption;
		sn = plUSBSN->Caption;
		mac = plMAC->Caption;

		if(Msg=="PASS")
		{
			dwTestCount++;
			plCount->Caption = dwTestCount;
		}
		TestNUM.sprintf("%04d",dwTestCount);

		if(Msg=="PASS")
		{
			SS = TestNUM+",[PASS],"+time + ",[SN],"+sn+",[MAC],"+mac+",[SPI_FLASH_EN],"+spi_flash_en+",[EFUSE_EEPROM_EN],"+efuse_eeprom_en+",[EFUSE_LED_SEL_CFG],"+efuse_led_sel_cfg+",[EFUSE_DOCKING],"+efuse_docking;
			SS += ",[Model],"+cbDevName->Text;
			SS+= ",[MACStartCode],"+mskedBurntMacStart->Text+",[MACLastCode],"+mskedBurntMacLast->Text+",[WorkOrder],"+edtWorkOrderNumber->Text+",[EmployeeID],"+edtEmployeeID->Text;
			SS += ",[TEST-TIME],"+AnsiString(test_time)+"\r\n";
		}
		else
		{
			SS = TestNUM+",[FAIL],"+time + ",[SN],"+sn+",[MAC],"+mac+",[SPI_FLASH_EN],"+spi_flash_en+",[EFUSE_EEPROM_EN],"+efuse_eeprom_en+",[EFUSE_LED_SEL_CFG],"+efuse_led_sel_cfg+",[EFUSE_DOCKING],"+efuse_docking;
			SS += ",[Model],"+cbDevName->Text;
			SS+= ",[MACStartCode],"+mskedBurntMacStart->Text+",[MACLastCode],"+mskedBurntMacLast->Text+",[WorkOrder],"+edtWorkOrderNumber->Text+",[EmployeeID],"+edtEmployeeID->Text;
			SS += ",[TEST-TIME],"+AnsiString(test_time)+",[ERROR],"+Msg+"\r\n";
		}
		bPASS[0] = SaveLogLine(RDLOGFilePath,SS);
		bPASS[1] = SaveLogLine(LOGFilePath,SS);
		if(LOGDiskPath.Pos("csv") && ImgDisk->Visible)
		{
			bPASS[2] = SaveLogLine(RDLOGDiskPath,SS);
			bPASS[3] = SaveLogLine(LOGDiskPath,SS);
		}
		for(int i = 0 ; i < 4 ; i++)
			if(!bPASS[i]) return false;
	}
	//
	return true;
}

bool  TUSBHIDForm::SaveLogLine(AnsiString FileName,AnsiString FileLine)
{
	FILE * fp;
	fp = fopen(FileName.c_str(),"a+");
	if(NULL == fp)
		return false;
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
	return true;
}
AnsiString TUSBHIDForm::GetStringLine(AnsiString String,AnsiString Keyword) {
	char *pch = strtok(String.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	AnsiString SS;
	while (pch != NULL)
	{
		SS = AnsiString(pch);

		if(SS.Pos(Keyword))
		{
			delete NList;
			return SS;
		}
		pch = strtok(NULL, "\r\n");
	}
	delete NList;
	return "";
}
void __fastcall TUSBHIDForm::LogClick(TObject *Sender)
{
	if(btnAutoTest->Enabled)
	{
		bStart = false;

		SetUIEnabled(true,false);
		ckbBarcodeClick(NULL);
		ckbBarcodeVerifyClick(NULL);
		btnAutoTest->Caption="Start";
		bPlugIn = false;

		ShowSettingForm();
	}
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::LOGFILEClick(TObject *Sender)
{
	ShellExecute(NULL,NULL,LOGFilePath.c_str(),NULL,NULL,SW_SHOW);
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::edtWorkOrderNumberExit(TObject *Sender)
{
	if(edtWorkOrderNumber->Text=="")
		edtWorkOrderNumber->Text = "000-00000000000";
}
//---------------------------------------------------------------------------

int __fastcall TUSBHIDForm::HexToInt(AnsiString HexStr) {
	int iIndex, iHex, totalChar;
	TCHAR HexChar[8];

	HexStr.UpperCase(); // 把字串轉成全大寫

	_tcscpy_s(HexChar, 8, HexStr.c_str());
	iHex = 0;
	totalChar = HexStr.Length(); // 取得字串的長度
	for (iIndex = 0; iIndex < totalChar; iIndex++) {
		iHex <<= 4;
		if ((HexChar[iIndex] >= 0x30) && (HexChar[iIndex] <= 0x39)) {
			iHex += (HexChar[iIndex] - 0x30); // 把 0 - 9 字元轉成數字
		}
		else if ((HexChar[iIndex] >= 0x41) && (HexChar[iIndex] <= 0x46)) {
			iHex += (HexChar[iIndex] - 0x37); // ­把 A - F 字元轉成數字
		}
		else {
			iHex = 0;
			break;
		}
	}
	return iHex;
}
//---------------------------------------------------------------------------
void TUSBHIDForm::Delay(DWORD iMilliSeconds) // 原版delay time 用在thread裡面
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::Analyze_USBView(AnsiString VID,AnsiString PID)
{
	if(FileExists("USB_INFO.txt")) DeleteFile("USB_INFO.txt");
	DosCommand("Taskkill /im DisplayUSB.exe /F"); //有時USBVIEW無法完全關閉
	DosCommand("DisplayUSB.exe SAVE");
	DosCommand("Taskkill /im DisplayUSB.exe /F"); //有時USBVIEW無法完全關閉
	if(FileExists("USB_INFO.txt"))
	{
		AnsiString filename = "USB_INFO.txt";
		ifstream lanfile(filename.c_str());
		std::string filemsg;
		if (lanfile.is_open())
		{
			while (!lanfile.eof())
			{
				getline(lanfile, filemsg);
				if(strstr(filemsg.c_str(),("wVendorID :0x"+VID).c_str()))//確認裝置VID
				{
					getline(lanfile, filemsg);
					if(strstr(filemsg.c_str(),("wProductID :0x"+PID).c_str()))//確認裝置PID
					{
						while(!lanfile.eof())
						{
							getline(lanfile, filemsg);
							if(strstr(filemsg.c_str(),"wMaxPacketSize "))
							{
								if(atoi(filemsg.substr(20,4).c_str())==400)
								{
									 lanfile.close();
									 DeleteFile("USB_INFO.txt");
									 return true;
								}
								else if(atoi(filemsg.substr(20,41).c_str())==200)
								{
									 lanfile.close();
									 DeleteFile("USB_INFO.txt");
									 return false;
								}
							}
						}
					}
				}
			}
			lanfile.close();
			DeleteFile("USB_INFO.txt");
			return false;
		}
		else
		{
			return false;
		}
	}
	return false;
}
bool TUSBHIDForm::CheckChipUSB3()
{
	return USBCONInfo->GetDevUSB(g_DevVPID.SubString(5,4),g_DevVPID.SubString(14,4));
}
bool TUSBHIDForm::UpdateRealtekCFGFile(AnsiString MAC)
{
	// 修改CFG檔的設定值
	// 讀取CFG檔案內容
	AnsiString CFGtotalmsg = ""; // 檔案內容
	bool bW;
	AnsiString Temp;
	ifstream imsgfile(AnsiString(MAC_FILE_PATH+g_CFGFile).c_str());
	std::string CFGmsg;
	if (imsgfile.is_open()) {
		while (!imsgfile.eof())
		{
			getline(imsgfile, CFGmsg);
			bW = false;
			if(MAC=="")
			{
				for(int i =0;i<9;i++)
				{
					if (strstr(CFGmsg.c_str(), zCharacteristics[i].c_str()))
					{
						TPanel* ckb = (TPanel*)FindComponent("ckb"+zCharacteristics[i]);
						Temp = ckb->Caption.Pos("√")?"01":"00";
						CFGtotalmsg += (zCharacteristics[i]+" = "+ Temp + "\n");
						bW = true;
						break;
					}

				}
				if (strstr(CFGmsg.c_str(), "LED_SEL_CFG")&&!strstr(CFGmsg.c_str(), ";")) {
					for(int j = 1;j < edtSetLED_SEL_CFG->Text.ToInt();j+=2)
					{
						Temp+= " "+edtSetLED_SEL_CFG->Text.SubString(j,2);
					}
					CFGtotalmsg += "LED_SEL_CFG ="+ Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "LINK_CAPA")) {
					Temp = edtSetNWAY_LINK_CAPA->Text;
					CFGtotalmsg += "LINK_CAPA = "+ Temp + "\n";
				}
				else if(!bW)
					CFGtotalmsg += AnsiString(CFGmsg.c_str()) + "\n";
			}
			else if(MAC.Length()==17)
			{
				if (strstr(CFGmsg.c_str(), "NODEID = ")) {
					CFGtotalmsg += "NODEID = " + MAC + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "SN = ")) {
					CFGtotalmsg += "SN = "
						+ AnsiString(mskedtBurnSn->Text).SubString(1,2)+" "
						+ AnsiString(mskedtBurnSn->Text).SubString(3,2)+" "
						+ AnsiString(mskedtBurnSn->Text).SubString(5,2)
						+"\n";
				}
				else if (strstr(CFGmsg.c_str(), "DOCKING = ")) {
					Temp = ckbRealtekDocking->Checked?"01":"00";
					CFGtotalmsg += "DOCKING = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "SPI_FLASH_EN = ")) {
					Temp = ckbSPI_FLASH_EN->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "SPI_FLASH_EN = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "EEPROM_EN = ")) {
					Temp = ckbEEPROM_EN->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "EEPROM_EN = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "NO_REMOTE_WAKEUP = ")) {
					Temp = ckbNO_REMOTE_WAKEUP->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "NO_REMOTE_WAKEUP = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "ECM_FLOW_CTRL = ")) {
					Temp = ckbECM_FLOW_CONTROL->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "ECM_FLOW_CTRL = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "LAN_DIS = ")) {
					Temp = ckbLAN_DIS->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "LAN_DIS = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "LINK_OK = ")) {
					Temp = ckbLINK_OK->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "LINK_OK = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "GPHY_FLOW_CTRL = ")) {
					Temp = ckbGPHY_FLOW_CONTROL->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "GPHY_FLOW_CTRL = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "LAN_WAKE_EN = ")) {
					Temp = ckbLAN_WAKE_EN->Caption.Pos("√")?"01":"00";
					CFGtotalmsg += "LAN_WAKE_EN = " + Temp + "\n";
				}
				else if (strstr(CFGmsg.c_str(), "LED_SEL_CFG = ")
					&&!strstr(CFGmsg.c_str(), ";")) {
					Temp = "";
					for(int j = 1;j < edtSetLED_SEL_CFG->Text.Length();j+=2)
						Temp+= " "+edtSetLED_SEL_CFG->Text.SubString(j,2);
					CFGtotalmsg += "LED_SEL_CFG =" + Temp + "\n";
				}
				else
					CFGtotalmsg += AnsiString(CFGmsg.c_str()) + "\n";
			}
			else
			{
				imsgfile.close();
				return false;
			}
			if (strstr(CFGmsg.c_str(), "VERSION_INFO"))
				break;
		}
		imsgfile.close();
	}
	// 寫入檔案
	ofstream omsgfile(AnsiString(MAC_FILE_PATH+g_CFGFile).c_str());
	omsgfile << CFGtotalmsg.c_str();
	omsgfile.close();
	return true;
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::btnRealtekLoadClick(TObject *Sender)
{
	SetUIEnabled(false,false);

	TBitBtn* btn = (TBitBtn*)Sender;
	if(btn->Name.Pos("Load"))
		GetDutInfo(true,false);
	else GetDutInfo(false,false);

	SetUIEnabled(true,false);
	labMSG(MSG_WAIT_DUT,clSkyBlue);
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::CallCommand_S(AnsiString filename, AnsiString cmd,
	AnsiString path) { // 下Command指令 BY ShellExecuteEx
	SHELLEXECUTEINFOA ShExecInfo = {
		0
	};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.lpFile = filename.c_str();
	ShExecInfo.lpParameters = cmd.c_str();
	ShExecInfo.lpDirectory = path.c_str();
	ShExecInfo.nShow = SW_HIDE;
	ShellExecuteExA(&ShExecInfo);
	DWORD dw = WaitForSingleObject(ShExecInfo.hProcess, 10000);
	TerminateProcess(ShExecInfo.hProcess,0);
	if(dw == WAIT_TIMEOUT)
		return false;
	return true;
}
AnsiString TUSBHIDForm::Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum) { // 找檔案找到字串行回傳幾行後的字串
	ifstream lanfile(filename.c_str());
	std::string filemsg;
	if (lanfile.is_open()) {
		while (!lanfile.eof()) {
			getline(lanfile, filemsg);
			if (strstr(filemsg.c_str(), findmsg.c_str())) {
				for (int i = 0; i < rownum; i++)
					getline(lanfile, filemsg);
				lanfile.close();
				return(AnsiString)filemsg.c_str();
			}
		}
		lanfile.close();
		return "";
	}
	else
	{
	DosCommand("Taskkill /im RTUNicPG64.exe /F"); //有時USBVIEW無法完全關閉
		return "";
	}
}
bool TUSBHIDForm::FindDevice()
{
	g_CFGFile = "";
	CallCommand_S(MAC_TOOL, CMD_GET_DEV, MAC_FILE_PATH);
	AnsiString Tempmsg = Findfilemsg(MAC_LOG_PATH, "Realtek USB", 0);
	if(!Tempmsg.Pos("Realtek USB"))
		Tempmsg = Findfilemsg(MAC_LOG_PATH, "EDC-G01", 0);
	if(Tempmsg == "")
		Tempmsg = Findfilemsg(MAC_LOG_PATH, "Realtek Gaming USB", 0);
	memoMsg->Lines->Add(Tempmsg);
	if(Tempmsg.Pos("RTL8153BvB")) // 2024Tool
	{
		if(!g_CFGFile.Pos("EF8153BvB.CFG"))
			UpdateRealtekCFGFile("");
		g_CFGFile = "EF8153BvB.CFG";
		plChipName->Caption =
			Tempmsg.SubString(Tempmsg.Pos("Realtek USB"),Tempmsg.Pos(" (")-Tempmsg.Pos("Realtek USB"));
	}
	else  if(Tempmsg.Pos("RTL8153VC")) // 8153Chip 2015Tool
	{
		if(!g_CFGFile.Pos("EF8153C.CFG"))
			UpdateRealtekCFGFile("");
		g_CFGFile = "EF8153C.CFG";
		plChipName->Caption =
			Tempmsg.SubString(Tempmsg.Pos("Realtek USB"),Tempmsg.Pos(" (")-Tempmsg.Pos("Realtek USB"));
	}
	else if(Tempmsg.Pos("RTL8153vC"))//8153Chip PGTOOL_v2.0.12 // 8153Chip 2021Toolver.201420
	{
		if(!g_CFGFile.Pos("EF8153vC.CFG"))
			UpdateRealtekCFGFile("");
		g_CFGFile = "EF8153vC.CFG";
		plChipName->Caption =
			Tempmsg.SubString(Tempmsg.Pos("Realtek USB"),Tempmsg.Pos(" (")-Tempmsg.Pos("Realtek USB"));
	}
	else if(Tempmsg.Pos("RTL8153AD")) //Slayer2 2015tool
	{
		if(!g_CFGFile.Pos("EF8153B.CFG"))
			UpdateRealtekCFGFile("");
		g_CFGFile = "EF8153B.CFG";
		plChipName->Caption =
			Tempmsg.SubString(Tempmsg.Pos("Realtek USB"),Tempmsg.Pos(" (")-Tempmsg.Pos("Realtek USB"));
	}
	else if(Tempmsg.Pos("RTL8158vA"))// IA-UED6 2019tool
	{
		if(!g_CFGFile.Pos("EF8158vA.CFG"))
			UpdateRealtekCFGFile("");
		g_CFGFile = "EF8158vA.CFG";
		if(Tempmsg.Pos("Realtek USB"))
			plChipName->Caption =
				Tempmsg.SubString(Tempmsg.Pos("Realtek USB"),Tempmsg.Pos(" (")-Tempmsg.Pos("Realtek USB"));
		else
			plChipName->Caption =
				Tempmsg.SubString(Tempmsg.Pos("ELECOM"),Tempmsg.Pos(" (")-Tempmsg.Pos("ELECOM"));
	}
	else if(Tempmsg.Pos("RTL8156BG")) // 8156GChip 2023Tool
	{
		if(!g_CFGFile.Pos("EF8156BG.CFG"))
			UpdateRealtekCFGFile("");
		g_CFGFile = "EF8156BG.CFG";
		plChipName->Caption =
			Tempmsg.SubString(Tempmsg.Pos("Realtek"),Tempmsg.Pos(" (")-Tempmsg.Pos("Realtek"));
	}
	else if(Tempmsg.Pos("RTL8156B")) // 8156Chip 2020Tool
	{
		if(!g_CFGFile.Pos("EF8156B.CFG"))
			UpdateRealtekCFGFile("");
		g_CFGFile = "EF8156B.CFG";
		plChipName->Caption =
			Tempmsg.SubString(Tempmsg.Pos("Realtek USB"),Tempmsg.Pos(" (")-Tempmsg.Pos("Realtek USB"));
	}
	else
		return false;
	return true;
}



void __fastcall TUSBHIDForm::btnSetClick(TObject *Sender)
{
	if(btnSet->Caption == "Enter")
	{
		if(edtPassWord->Text.UpperCase() == PASSWORD)
		{
			edtPassWord->Text ="";
			//plCharacteristics->Height = 410;
			btnSet->Caption		 = "END";
			SetUIEnabled(true,true);
		}
		else if(edtPassWord->Text.UpperCase() == MEMOWORD)
		{
			edtPassWord->Text ="";
			plCharacteristics->Height = plCharacteristics->Height==443?71:443;
		}
		else edtPassWord->Font->Color = clRed;
	}

	else
	{
		btnSet->Caption	  	= "Enter";
		edtPassWord->Text 	= "";
		SetUIEnabled(false,true);
		SetIniFile();
	}
}
//---------------------------------------------------------------------------
//UI Change Function
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::mskedtBurnMacChange(TObject *Sender)
{
	//mskedtBurnSn->Text = mskedtBurnMac->Text;
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::ckbLPM_BESL_ENClick(TObject *Sender)
{
	TPanel* ckb = (TPanel*) Sender;
	ckb->Caption = ckb->Caption.Pos("√")?"":"√";
	if(ckb->Name.Pos("ckbSPI_FLASH_EN"))
		edtSPI_FLASH_EN->Text = ckb->Caption.Pos("√") ? "01":"00";
	if(ckb->Name.Pos("ckbEEPROM_EN"))
		edtEEPROM_EN->Text = ckb->Caption.Pos("√") ? "01":"00";
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::edtPassWordKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if(Key==13)
	{
		btnSet->SetFocus();
		btnSetClick(NULL);
	}
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::edtPassWordEnter(TObject *Sender)
{
	if(edtPassWord->Font->Color==clRed)
	{
		edtPassWord->Font->Color = clBlue;
		edtPassWord->Text = "";
    }
}
//---------------------------------------------------------------------------
void TUSBHIDForm::SetUIEnabled(bool bEnabled,bool bSet)
{
	if(bSet)
	{
		btnAutoTest->Enabled			= !bEnabled;
		//ckbLPM_BESL_EN->Enabled 		= bEnabled;
		ckbSPI_FLASH_EN->Enabled 		= bEnabled;
		ckbEEPROM_EN->Enabled 			= bEnabled;
		ckbNO_REMOTE_WAKEUP->Enabled 	= bEnabled;
		//ckbBOS_DESC_SUPERSPEED->Enabled = bEnabled;
		ckbECM_FLOW_CONTROL->Enabled 	= bEnabled;
		ckbLAN_DIS->Enabled 			= bEnabled;
		ckbLINK_OK->Enabled 			= bEnabled;
		ckbGPHY_FLOW_CONTROL->Enabled 	= bEnabled;
		edtSetLED_SEL_CFG->Enabled		= bEnabled;
		edtSetNWAY_LINK_CAPA->Enabled	= bEnabled;
		btnLoadDutVersion->Enabled 		= bEnabled;
		cbDevName->Enabled				= bEnabled;
	}
	else
	{
		btnSet->Enabled             = bEnabled;
		//btnLoadDutVersion->Enabled 	= bEnabled;
		if(!ckbBarcode->Checked)
		{
			ckbAutoIncrease->Enabled 	= bEnabled;
			ckbBurnMAC->Enabled 		= bEnabled;
		}
		cbDevName->Enabled			= bEnabled;
		edtPassWord->Enabled		= bEnabled;
		mskedtBurnSn->Enabled 		= bEnabled;
		mskedBurntMacPre->Enabled 	= bEnabled;
		mskedtBurnMac->Enabled 		= bEnabled;
		btnGetDutVersion->Enabled 	= bEnabled;
		edtEmployeeID->Enabled 		= bEnabled;
		ckbBarcode->Enabled 		= bEnabled;
		ckbBarcodeVerify->Enabled 	= bEnabled;
		//ckbRealtekDocking->Enabled 	= bEnabled;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::ckbBarcodeVerifyClick(TObject *Sender)
{
	if(ckbBarcodeVerify->Checked)
	{
	   ckbAutoIncrease->Checked = false;
	   ckbAutoIncrease->Enabled = false;
	}
	else
	{
	   ckbAutoIncrease->Enabled = true;
	   ckbAutoIncrease->Enabled = ckbBarcode->Checked ? false:true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::ckbBarcodeClick(TObject *Sender)
{
	if(ckbBarcode->Checked)
	{
		ckbBurnMAC->Checked = true;
		ckbBurnMAC->Enabled = false;
		ckbAutoIncrease->Checked = false;
		ckbAutoIncrease->Enabled = false;
	}
	else
	{
		ckbBurnMAC->Enabled = true;
		ckbAutoIncrease->Enabled = ckbBarcodeVerify->Checked ? false:true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::plSwitch1Click(TObject *Sender)
{
	TPanel* pl = (TPanel*)Sender;
	if(pl->Name.Pos("1"))
	{
		if(pl->Caption == ">")
		{
			if(cbDevName->Text.Pos("IA-UED6"))
				plIA_UEA6->Width = 295;
			plDebug->Width = 337;
			plIA_UEA6->Left =  plDebug->Left+850;
			pl->Caption = "<";
			USBHIDForm->Width += dwRight;
		}
		else
		{
			USBHIDForm->Width -= dwRight;
			plIA_UEA6->Width = 1;
			plDebug->Width = 1;
			pl->Caption = ">";
		}
	}
	else
	{
		if(pl->Caption == ">")
		{
			USBHIDForm->Width -= plLeft->Width;
			plLeft->Width = 0;
			pl->Caption = "<";
		}
		else
		{
			plLeft->Width = dwLeft;
			pl->Caption = ">";
			USBHIDForm->Width += plLeft->Width;
		}
    }
}
AnsiString TUSBHIDForm::DosCommand(AnsiString sCmdline)
{
	PROCESS_INFORMATION proc = {0}; //關於進程資訊的一個結構
	long ret;
	bool sPipe;
	STARTUPINFO start = {0};
	SECURITY_ATTRIBUTES sa = {0};
	HANDLE hReadPipe ;
	HANDLE hWritePipe;
	AnsiString sOutput;
	AnsiString sBuffer;
	unsigned long lngBytesRead;
	char cBuffer[256];
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor=0;
	sa.bInheritHandle = TRUE;
	sPipe=::CreatePipe(&hReadPipe, &hWritePipe,&sa, 0); //創建管道
	if (!sPipe)
	{
	sOutput="CreatePipe failed. Error: "+AnsiString(GetLastError());
	//memoMsg->Lines->Add("CreatePipe failed. Error: "+AnsiString(GetLastError()));
	return sOutput;
	}
	start.cb = sizeof(STARTUPINFOA);
	start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	start.hStdOutput = hWritePipe;
	start.hStdError = hWritePipe;
	start.wShowWindow = SW_HIDE;
	sBuffer = sCmdline;
	ret =::CreateProcess(0, sBuffer.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc);
	if (ret == 0)
	{
	sOutput="Bad command or filename";
	memoMsg->Lines->Add("Bad command or filename");
	return sOutput;
	}
	::CloseHandle(hWritePipe);
	if(!sCmdline.Pos("DisplayLinkUSBIOMTT.inf"))
	{
		DWORD dw = WaitForSingleObject(proc.hProcess, 10000);
		if(dw == WAIT_TIMEOUT)
		{
			::CloseHandle(proc.hProcess);
			::CloseHandle(proc.hThread);
			::CloseHandle(hReadPipe);
			return "WAIT_TIMEOUT";
		}
	}
	do
	{
	memset(cBuffer,'\0',sizeof(cBuffer));
	ret = ::ReadFile(hReadPipe, &cBuffer, 255, &lngBytesRead, 0);
	//替換字串
	for(unsigned long i=0; i< lngBytesRead; i++){
		if(cBuffer[i] == '\0'){
			cBuffer[i] = ' ';
		}else if(cBuffer[i] == '\n'){
			cBuffer[i] = ' ';
		}
	}
	sBuffer=StrPas(cBuffer);
	sOutput = sOutput+sBuffer;
	Application->ProcessMessages();

	} while (ret != 0 );
	::CloseHandle(proc.hProcess);
	::CloseHandle(proc.hThread);
	::CloseHandle(hReadPipe);
	return sOutput;
}
bool TUSBHIDForm::DUTV_Efuse()
{
	AnsiString MSG,ckbMSG;
	bool bResult = true;
	if(Findfilemsg(MAC_LOG_PATH,BURN_SUCCESSFUL, 0)!="")
	{
		// EFUSE_DOCKING
		MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_NO_REMOTE_WAKEUP =", 0);
		if(!MSG.Length()) MSG = "EFUSE_NO_REMOTE_WAKEUP = 00";
		MSG = AnsiString(MSG).SubString(26,2);
		ckbMSG = ckbNO_REMOTE_WAKEUP->Caption.Pos("√") ? "01":"00";
		if(ckbMSG != MSG)
		{
			plSetting_NO_REMOTE_WAKEUP->Color = clRed;
			bResult = false;
        }

		// EFUSE_DOCKING
		MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_LINK_OK =", 0);
		if(!MSG.Length()) MSG = "EFUSE_LINK_OK = 00";
		MSG = AnsiString(MSG).SubString(17,2);
		ckbMSG = ckbLINK_OK->Caption.Pos("√") ? "01":"00";
		if(ckbMSG != MSG)
		{
			plSetting_LINK_OK->Color = clRed;
			bResult = false;
		}

		// EFUSE_DOCKING
		MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_LAN_DIS =", 0);
		if(!MSG.Length()) MSG = "EFUSE_LAN_DIS = 00";
		MSG = AnsiString(MSG).SubString(17,2);
		ckbMSG = ckbLAN_DIS->Caption.Pos("√") ? "01":"00";
		if(ckbMSG != MSG)
		{
			plSetting_LAN_DIS->Color = clRed;
			bResult = false;
		}

		// EFUSE_DOCKING
		MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_ECM_FLOW_CTRL =", 0);
		if(!MSG.Length()) MSG = "EFUSE_ECM_FLOW_CTRL = 00";
		MSG = AnsiString(MSG).SubString(23,2);
		ckbMSG = ckbECM_FLOW_CONTROL->Caption.Pos("√") ? "01":"00";
		if(ckbMSG != MSG)
		{
			plSetting_ECM_FLOW_CONTROL->Color = clRed;
			bResult = false;
		}

		// EFUSE_DOCKING
		MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_GPHY_FLOW_CTRL =", 0);
		if(!MSG.Length()) MSG = "EFUSE_GPHY_FLOW_CTRL = 00";
		MSG = AnsiString(MSG).SubString(24,2);
		ckbMSG = ckbGPHY_FLOW_CONTROL->Caption.Pos("√") ? "01":"00";
		if(ckbMSG != MSG)
		{
			plSetting_GFC->Color = clRed;
			bResult = false;
		}
		if(cbDevName->Text.Pos("IA-UED6"))
		{
			// EFUSE_DOCKING
			MSG = Findfilemsg(MAC_LOG_PATH, "VID =", 0);
			if(!MSG.Length()) MSG = "VID = 00 00";
			MSG = AnsiString(MSG).SubString(7,2)+AnsiString(MSG).SubString(10,2);
			ckbMSG = edtSetIDVendor->Text;
			if(ckbMSG != MSG)
			{
				plVID->Color = clRed;
				bResult = false;
			}
			// EFUSE_DOCKING
			MSG = Findfilemsg(MAC_LOG_PATH, "PID =", 0);
			if(!MSG.Length()) MSG = "VID = 00 00";
			MSG = AnsiString(MSG).SubString(7,2)+AnsiString(MSG).SubString(10,2);
			ckbMSG = edtSetIDProduct->Text;
			if(ckbMSG != MSG)
			{
				plPID->Color = clRed;
				bResult = false;
			}
			// EFUSE_DOCKING
			MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_BCD_DEVICE =", 0);
			if(!MSG.Length()) MSG = "EFUSE_BCD_DEVICE = 00 00";
			MSG = AnsiString(MSG).SubString(20,2)+AnsiString(MSG).SubString(23,2);
			ckbMSG = edtSetIDbcdDevice->Text;
			if(ckbMSG != MSG)
			{
				plVer->Color = clRed;
				bResult = false;
			}
			// EFUSE_DOCKING
			MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_PRODUCT =", 0);
			if(!MSG.Length()) MSG = "EFUSE_PRODUCT = 0";
			MSG = AnsiString(MSG).SubString(17,7);
			ckbMSG = edtSetProduct->Text;
			if(ckbMSG != MSG)
			{
				plProduct->Color = clRed;
				bResult = false;
			}
			// EFUSE_DOCKING
			MSG = Findfilemsg(MAC_LOG_PATH, "EFUSE_LAN_WAKE_EN =", 0);
			if(!MSG.Length()) MSG = "EFUSE_LAN_WAKE_EN = 00";
			MSG = AnsiString(MSG).SubString(21,2);
			ckbMSG = ckbLAN_WAKE_EN->Caption.Pos("√") ? "01":"00";
			if(ckbMSG != MSG)
			{
				plLAN_WAKE_EN->Color = clRed;
				bResult = false;
			}
		}
	}
	else	bResult = false;

	return	bResult;
}
void __fastcall TUSBHIDForm::edtSetLED_SEL_CFGExit(TObject *Sender)
{
	edtLED_SEL_CFG->Text = edtSetLED_SEL_CFG->Text;
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::btnBarcodeChanelClick(TObject *Sender)
{
	numBarcodeResult = BARCODE_CHANEL;
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(edtBarcodeMAC->Text.Length()==12&&Key==13)
	{
		USBHIDForm->mskedtBurnSn->Text = edtBarcodeMAC->Text.SubString(7,6);
		USBHIDForm->mskedBurntMacPre->Text = edtBarcodeMAC->Text.SubString(1,6);
		USBHIDForm->mskedtBurnMac->Text = edtBarcodeMAC->Text.SubString(7,6);
		numBarcodeResult = BARCODE_FINISH;
	}
	else if(edtBarcodeMAC->Text.Length()<12&&Key==13)
		edtBarcodeMAC->Text = "";
	else if(Key == 8)
		edtBarcodeMAC->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::edtBarcodeMACChange(TObject *Sender)
{
	TEdit* edt=(TEdit*)Sender;
	TCHAR HexChar[15];

	edt->Text = edt->Text.UpperCase();// 把字串轉成全大寫
	_tcscpy_s(HexChar, 15, AnsiString(edt->Text).c_str());
	for(int i=0;i <edt->Text.Length();i++)
	{
		if(HexChar[i] < 0x30 || HexChar[i] > 0x39)//非數字
		{
			if(HexChar[i] < 0x41 || HexChar[i] > 0x46)//非大寫字母a~f
			{
				edtBarcodeMAC->Text =
					edtBarcodeMAC->Text.Delete(edtBarcodeMAC->Text.Length(),1);
            }
		}
	}
	if(edtBarcodeMAC->Text.Length()> 12)
	  edtBarcodeMAC->Text = "";
	edt-> SelStart=edt-> Text.Length();
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::btnBarcodeClearClick(TObject *Sender)
{
	edtBarcodeMAC->Text = "";
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::FindIniFile()
{
	TSearchRec Sr;
	TStringList*FileList=new TStringList();
	cbDevName->Items->Clear();
	AnsiString FILE_DUT_SET_INI = "INI\\"+Findfilemsg("INI\\Config.ini","DEV_NAME",1)+".ini";
	if(FindFirst("INI\\*.ini",faAnyFile,Sr)==0)
	{
		do
		{
			FileList->Add(Sr.Name);
			if(Sr.Name!="Config.ini")
				cbDevName->Items->Add(Sr.Name.SubString(1,Sr.Name.Length()-4));
		}
		while(FindNext(Sr)==0);
		FindClose(Sr);
	}
	for(int i =0;i<=cbDevName->Items->Count;i++)
	{
		if(FILE_DUT_SET_INI.Pos(cbDevName->Items->Strings[i]))
		   cbDevName->ItemIndex = i;
	}
	if(FileList->Count==0)
	{
		delete FileList;
		return false;
	}
	else
	{
		cbDevNameChange(NULL);
		delete FileList;
		return true;
	}
}
void __fastcall TUSBHIDForm::cbDevNameChange(TObject *Sender)
{
	AnsiString FILE_DUT_SET_INI = "INI\\"+cbDevName->Text+".ini";
	AnsiString SS;
	if(FileExists(FILE_DUT_SET_INI))
	{
		if (FileExists(ChangeFileExt("INI\\"+cbDevName->Text, ".ini")))
		{
			TIniFile *ini;			ini = new TIniFile(ChangeFileExt("INI\\"+cbDevName->Text, ".ini"));			Tool_Ver = ini->ReadString(_T("BURN_SETTING"), _T("TOOL_VER"), _T("20200214"));			//			MAC_FILE_PATH  	= "RealtekMAC\\"+Tool_Ver+"\\";			MAC_LOG_PATH  	= "RealtekMAC\\"+Tool_Ver+"\\pgtool.log";			if(cbDevName->Text.Pos("IA-UED6"))			{				plIA_UEA6->Width  = 295;				USBHIDForm->Width = 1468;				dwRight = plDebug->Width + plIA_UEA6->Width+2;			}			else			{				plIA_UEA6->Width  = 1;
				USBHIDForm->Width = 1175;
				dwRight = plDebug->Width;
			}			//BURN_SETTING			//ckbBurnMAC->Checked = ini->ReadBool("BURN_SETTING", "BURN_MAC", false);			//ckbAutoIncrease->Checked = ini->ReadBool("BURN_SETTING", "AUTO", false);			//ckbBarcode->Checked = ini->ReadBool("BURN_SETTING", "BARCODE", false);			//ckbBarcodeClick(NULL);			//ckbBarcodeVerify->Checked = ini->ReadBool("BURN_SETTING", "BARCODE_VERIFY", false);			//ckbBarcodeVerifyClick(NULL);			//BURN_CONTENT			mskedBurntMacPre->Text = ini->ReadString(_T("BURN_CONTENT"), _T("MAC_PREFIX"), _T("0"));			mskedtBurnMac->Text = ini->ReadString(_T("BURN_CONTENT"), _T("MAC_END"), _T("0"));			//Realtek			mskedtBurnSn->Text    = ini->ReadString(_T("Realtek"), _T("SN"), _T("000001"));			ckbRealtekDocking->Checked   = ini->ReadBool(_T("Realtek"), _T("DOCKING"), _T(false));			edtSPI_FLASH_EN->Text        = ini->ReadString(_T("Realtek"), _T("EFUSE_SPI_FLASH_EN"), _T("01"));			edtEEPROM_EN->Text           = ini->ReadString(_T("Realtek"), _T("EFUSE_EEPROM_EN"), _T("00"));			edtLED_SEL_CFG->Text         = ini->ReadString(_T("Realtek"), _T("EFUSE_LED_SEL_CFG"), _T("87F0"));			edtDocking->Text         	 = ini->ReadString(_T("Realtek"), _T("EFUSE_DOCKING"), _T("01"));			//			SS = ini->ReadString(_T("EFuse Setting"), _T("EFUSE_SPI_FLASH_EN"), _T("01"));			ckbSPI_FLASH_EN->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("EEPROM_EN"), _T("00"));			ckbEEPROM_EN->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("NO_REMOTE_WAKEUP"), _T("00"));			ckbNO_REMOTE_WAKEUP->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("ECM_FLOW_CONTROL"), _T("00"));			ckbECM_FLOW_CONTROL->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("LAN_DIS"), _T("00"));			ckbLAN_DIS->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("LINK_OK"), _T("00"));			ckbLINK_OK->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("GPHY_FOLW_CONTROL"), _T("01"));			ckbGPHY_FLOW_CONTROL->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("LED_SEL_CFG"), _T("787C"));			edtSetLED_SEL_CFG->Text = SS;			SS = ini->ReadString(_T("EFuse Setting"), _T("NWAY_LINK_CAPA"), _T("06"));			edtSetNWAY_LINK_CAPA->Text = SS;			SS = ini->ReadString(_T("EFuse Setting"), _T("VID"), _T("056E"));			edtSetIDVendor->Text = SS;			SS = ini->ReadString(_T("EFuse Setting"), _T("PID"), _T("4013"));			edtSetIDProduct->Text = SS;			SS = ini->ReadString(_T("EFuse Setting"), _T("BCD_DEVICE"), _T("3105"));			edtSetIDbcdDevice->Text = SS;			SS = ini->ReadString(_T("EFuse Setting"), _T("MANFCTURE"), _T("Realtek"));			edtSetManfcture->Text = SS;			SS = ini->ReadString(_T("EFuse Setting"), _T("PRODUCT"), _T("EDC-G01"));			edtSetProduct->Text = SS;			SS = ini->ReadString(_T("EFuse Setting"), _T("CTAPSHORT"), _T("01"));			ckbCTAPSHORT->Caption = SS=="01" ? "√" : "";			SS = ini->ReadString(_T("EFuse Setting"), _T("LAN_WAKE_EN"), _T("01"));			ckbLAN_WAKE_EN->Caption = SS=="01" ? "√" : "";			SS = ckbSPI_FLASH_EN->Caption.Pos("√") ? "01":"00";
			ini->WriteString(_T("EFuse Setting"), _T("EFUSE_SPI_FLASH_EN"), SS);
			SS = ckbEEPROM_EN->Caption.Pos("√") ? "01":"00";
			ini->WriteString(_T("EFuse Setting"), _T("EEPROM_EN"), SS);
			SS = ckbNO_REMOTE_WAKEUP->Caption.Pos("√") ? "01":"00";
			ini->WriteString(_T("EFuse Setting"), _T("NO_REMOTE_WAKEUP"), SS);
			SS = ckbECM_FLOW_CONTROL->Caption.Pos("√") ? "01":"00";
			ini->WriteString(_T("EFuse Setting"), _T("ECM_FLOW_CONTROL"), SS);
			SS = ckbLAN_DIS->Caption.Pos("√") ? "01":"00";
			ini->WriteString(_T("EFuse Setting"), _T("LAN_DIS"), SS);
			SS = ckbLINK_OK->Caption.Pos("√") ? "01":"00";
			ini->WriteString(_T("EFuse Setting"), _T("LINK_OK"), SS);
			SS = ckbGPHY_FLOW_CONTROL->Caption.Pos("√") ? "01":"00";
			ini->WriteString(_T("EFuse Setting"), _T("GPHY_FOLW_CONTROL"), SS);
			SS = edtLED_SEL_CFG->Text;
			ini->WriteString(_T("EFuse Setting"), _T("LED_SEL_CFG"), SS);			//			delete ini;
			//
			ini = new TIniFile(ChangeFileExt("Realtek MACView", ".ini"));
			//WorkOrder
			edtWorkOrderNumber->Text = ini->ReadString(_T("WorkOrder"), _T("WorkOrderNumber"), _T("000-00000000000"));
			edtEmployeeID->Text = ini->ReadString(_T("WorkOrder"), _T("EmployeeID"), _T("T00000"));			mskedBurntMacStart->Text = ini->ReadString(_T("WorkOrder"), _T("BurntMacStart"), _T("000000000000"));			mskedBurntMacLast->Text = ini->ReadString(_T("WorkOrder"), _T("BurntMacLast"), _T("FFFFFFFFFFFF"));
			delete ini;
		}
	}
}
bool TUSBHIDForm::SetIniFile()
{
	TIniFile *ini;
	ini = new TIniFile(ChangeFileExt("INI\\"+cbDevName->Text, ".ini"));

	//Thired step. 儲存所有設定
	AnsiString SS;
	//BURN_SETTING
	ini->WriteBool("BURN_SETTING", "BURN_MAC", ckbBurnMAC->Checked);
	ini->WriteBool("BURN_SETTING", "AUTO", ckbAutoIncrease->Checked);
	ini->WriteBool("BURN_SETTING", "BARCODE", ckbBarcode->Checked);
	ini->WriteBool("BURN_SETTING", "BARCODE_VERIFY", ckbBarcodeVerify->Checked);
	//BURN_CONTENT
	ini->WriteString(_T("BURN_CONTENT"), _T("MAC_PREFIX"), mskedBurntMacPre->Text);
	ini->WriteString(_T("BURN_CONTENT"), _T("MAC_END"), mskedtBurnMac->Text);
	// Realtek
	ini->WriteString(_T("Realtek"), _T("SN"), mskedtBurnSn->Text);
	ini->WriteBool("Realtek", "DOCKING", ckbRealtekDocking->Checked);
	ini->WriteString(_T("Realtek"), _T("EFUSE_SPI_FLASH_EN"), edtSPI_FLASH_EN->Text);
	ini->WriteString(_T("Realtek"), _T("EFUSE_EEPROM_EN"), edtEEPROM_EN->Text);
	ini->WriteString(_T("Realtek"), _T("EFUSE_LED_SEL_CFG"), edtLED_SEL_CFG->Text);
	ini->WriteString(_T("Realtek"), _T("EFUSE_DOCKING"), edtDocking->Text);
	//
	SS = ckbSPI_FLASH_EN->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("EFUSE_SPI_FLASH_EN"), SS);
	SS = ckbEEPROM_EN->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("EEPROM_EN"), SS);
	SS = ckbNO_REMOTE_WAKEUP->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("NO_REMOTE_WAKEUP"), SS);
	SS = ckbECM_FLOW_CONTROL->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("ECM_FLOW_CONTROL"), SS);
	SS = ckbLAN_DIS->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("LAN_DIS"), SS);
	SS = ckbLINK_OK->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("LINK_OK"), SS);
	SS = ckbGPHY_FLOW_CONTROL->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("GPHY_FOLW_CONTROL"), SS);
	SS = edtLED_SEL_CFG->Text;
	ini->WriteString(_T("EFuse Setting"), _T("LED_SEL_CFG"), SS);
	SS = ckbLAN_WAKE_EN->Caption.Pos("√") ? "01":"00";
	ini->WriteString(_T("EFuse Setting"), _T("LAN_WAKE_EN"), SS);
	delete ini;

	ini = new TIniFile(ChangeFileExt("Realtek MACView", ".ini"));
	//WorkOrder
	ini->WriteString(_T("WorkOrder"), _T("WorkOrderNumber"), edtWorkOrderNumber->Text);
	ini->WriteString(_T("WorkOrder"), _T("EmployeeID"), edtEmployeeID->Text);
	ini->WriteString(_T("WorkOrder"), _T("BurntMacStart"), mskedBurntMacStart->Text);
	ini->WriteString(_T("WorkOrder"), _T("BurntMacLast"), mskedBurntMacLast->Text);
	delete ini;

	return true;
}
//---------------------------------------------------------------------------
void TUSBHIDForm::ShowSettingForm()
{
	if(frmMsg==NULL)  frmMsg = new TfrmMsg(Application);

	frmMsg->edtSetWorkOrderNumber->Text = edtWorkOrderNumber->Text;
	frmMsg->mskedBurntMacStart->Text = mskedBurntMacStart->Text;
	frmMsg->mskedBurntMacLast->Text = mskedBurntMacLast->Text;
	frmMsg->edtSetEmployeeID->Text = edtEmployeeID->Text.Trim();
	if(ckbBarcode->Checked)
		frmMsg->plckbBarcode->Caption = "√";
	else
		frmMsg->plckbBarcode->Caption = "";

	if(plTestMode->Caption.Pos("一般"))
	{
		frmMsg->plNormalMode->Color = clLime;
		frmMsg->plReworkMode->Color = clCream;
	}
	else
	{
		frmMsg->plNormalMode->Color = clCream;
		frmMsg->plReworkMode->Color = clLime;
	}

	if(frmMsg->ShowModal()== mrOk)
	{
		edtWorkOrderNumber->Text = frmMsg->edtSetWorkOrderNumber->Text.Trim();
		edtEmployeeID->Text = frmMsg->edtSetEmployeeID->Text.Trim();
		mskedBurntMacStart->Text = frmMsg->mskedBurntMacStart->Text;
		mskedBurntMacLast->Text = frmMsg->mskedBurntMacLast->Text;
		//
		TIniFile *ini;
		ini = new TIniFile(ChangeFileExt("Realtek MACView", ".ini"));
		//WorkOrder
		ini->WriteString(_T("WorkOrder"), _T("WorkOrderNumber"), edtWorkOrderNumber->Text);
		ini->WriteString(_T("WorkOrder"), _T("EmployeeID"), edtEmployeeID->Text);
		ini->WriteString(_T("WorkOrder"), _T("BurntMacStart"), mskedBurntMacStart->Text);
		ini->WriteString(_T("WorkOrder"), _T("BurntMacLast"), mskedBurntMacLast->Text);
		delete ini;
		//
		//取得LOGFilePath、LOGDiskPath路徑
		LOGFilePath = "C:\\ASMP\\log\\"+edtWorkOrderNumber->Text+"\\RealtekMACView";
		CheckDiskName();
		//
		if(frmMsg->plckbBarcode->Caption.Pos("√"))
		{
			bReWork = false;
			ckbAutoIncrease->Checked = false;
			ckbBurnMAC->Checked = true;
			ckbBarcode->Checked = true;
			gbBurnContent->Enabled = false;
			plTestMode->Caption = frmMsg->plNormalMode->Color == clLime ? "一般模式":"重工模式";
			if(frmMsg->plNormalMode->Color != clLime)
			{   //重工模式
				bReWork = true;
				LOGFilePath += "\\ReWork";
				if(LOGDiskPath != "")
					LOGDiskPath += "\\ReWork";
			}
		}
		else
		{
			ckbAutoIncrease->Checked = false;
			ckbBurnMAC->Checked = false;
			ckbBarcode->Enabled = false;
			ckbBarcode->Checked = false;
			gbBurnContent->Enabled = true;
		}
		FindLogFile();
		delete frmMsg;
		frmMsg = NULL;
	}
	else
	{
		delete frmMsg;
		frmMsg = NULL;
		Close();
	}
}
void TUSBHIDForm::FindLogFile()
{
	//
	NewFilePath(LOGFilePath);
	NewFilePath(LOGDiskPath);
	//
	TSearchRec Sr;
	AnsiString time = FormatDateTime("yyyymmdd-hhnnss", Now());
	if(DirectoryExists(LOGDiskPath.c_str()))
	{
		ImgDisk->Visible = true;
		if(FindFirst(LOGDiskPath+"\\*.csv",faAnyFile,Sr)==0)
		{
			do
			{
				if(Sr.Name.Pos("RD"))
				{
					if(Sr.Name.SubString(3,8) == time.SubString(1,8))
					{
						RDLOGDiskPath = LOGDiskPath + "\\"+Sr.Name;
						LOGDiskPath += "\\"+Sr.Name.SubString(3,19);
						//
						RDLOGFilePath = LOGFilePath + "\\"+Sr.Name;
						LOGFilePath += "\\"+Sr.Name.SubString(3,19);
						break;
					}
				}
				else
				{
					if(Sr.Name.SubString(1,8) == time.SubString(1,8))
					{
						RDLOGDiskPath = LOGDiskPath +"\\RD"+Sr.Name;
						LOGDiskPath += "\\"+Sr.Name;
						//
						RDLOGFilePath = LOGFilePath +"\\RD"+Sr.Name;
						LOGFilePath += "\\"+Sr.Name;
						break;
					}
				}
			}
			while(FindNext(Sr)==0);
			FindClose(Sr);
		}

		if(!LOGDiskPath.Pos(".csv"))
		{
			RDLOGDiskPath = LOGDiskPath +"\\RD"+time+".csv";
			LOGDiskPath += "\\"+time+".csv";
			RDLOGFilePath = LOGFilePath +"\\RD"+time+".csv";
			LOGFilePath += "\\"+time+".csv";
		}
		else//檔案已存在於USBDisk
		{
			if(!FileExists(LOGFilePath))
			{
				CopyFile(LOGDiskPath.c_str(),LOGFilePath.c_str(),true);
				CopyFile(RDLOGDiskPath.c_str(),RDLOGFilePath.c_str(),true);
			}
		}
	}
	else
	{
		ImgDisk->Visible = false;
		if(DirectoryExists(LOGFilePath.c_str()))
		{
			if(FindFirst(LOGFilePath+"\\*.csv",faAnyFile,Sr)==0)
			{
				do
				{
					if(Sr.Name.Pos("RD"))
					{
						if(Sr.Name.SubString(3,8) == time.SubString(1,8))
						{
							RDLOGFilePath = LOGFilePath + "\\"+Sr.Name;
							LOGFilePath += "\\"+Sr.Name.SubString(3,19);
							break;
						}
					}
					else
					{
						if(Sr.Name.SubString(1,8) == time.SubString(1,8))
						{
							RDLOGFilePath = LOGFilePath +"\\RD"+Sr.Name;
							LOGFilePath += "\\"+Sr.Name;
							break;
						}
					}
				}
				while(FindNext(Sr)==0);
				FindClose(Sr);
			}
		}
		if(!LOGFilePath.Pos(".csv"))
		{
			RDLOGFilePath = LOGFilePath +"\\RD"+time+".csv";
			LOGFilePath += "\\"+time+".csv";
		}
	}
	//
	dwTestCount = CheckMAC("");
	plCount->Caption = dwTestCount;
}
DWORD TUSBHIDForm::CheckMAC(AnsiString MAC)
{   //換人員重計數 //不綁mac範圍 //
	if(bReWork) return 0;
	AnsiString Temp_Start,Temp_Last,Temp;
	if(MAC != "")
	{
	//範圍
		Temp_Start = HexToInt(mskedBurntMacStart->Text.SubString(1,6));
		Temp_Last  = HexToInt(mskedBurntMacLast->Text.SubString(1,6));
		Temp  	   = HexToInt(MAC.SubString(1,6));
		if(Temp.ToInt() <= Temp_Last.ToInt() && Temp.ToInt() >= Temp_Start.ToInt())
		{
			Temp_Start = HexToInt(mskedBurntMacStart->Text.SubString(7,6));
			Temp_Last  = HexToInt(mskedBurntMacLast->Text.SubString(7,6));
			Temp  	   = HexToInt(MAC.SubString(7,6));
			if(Temp.ToInt() > Temp_Last.ToInt() || Temp.ToInt() < Temp_Start.ToInt())
				return ERROR_MAC_OUTRANGE;
		}
		else return ERROR_MAC_OUTRANGE;
	//重複
		Temp = LOGFilePath;
		AnsiString TempRepeat,TempRepeat1;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS")
					)
					{
						TempRepeat  = "[MAC]"+MAC;
						TempRepeat1 = "[MAC2]"+MAC;
						if(strstr(filemsg.c_str(), TempRepeat.c_str())
						|| strstr(filemsg.c_str(), TempRepeat1.c_str()))
						{
							lanfile.close();
							return ERROR_MAC_REPEAT;
						}
					}
				}
				lanfile.close();
			}
		}
		Temp = LOGDiskPath;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS")
					)
					{
						TempRepeat  = "[MAC]"+MAC;
						TempRepeat1 = "[MAC2]"+MAC;
						if(strstr(filemsg.c_str(), TempRepeat.c_str())
						|| strstr(filemsg.c_str(), TempRepeat1.c_str()))
						{
							lanfile.close();
							return ERROR_MAC_REPEAT;
						}
					}
				}
				lanfile.close();
			}
		}
		return VERIFTY_MAC_PASS;
	}
	else
	{   //計數
		DWORD dwCount = 0,dwCount1 = 0;
		Temp = LOGFilePath;
		AnsiString EmployeeID = "[EmployeeID]"+edtEmployeeID->Text;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS"))
					{   //換工號重新計數  不綁mac範圍
						if(!AnsiString(filemsg.c_str()).Pos(EmployeeID))
							dwCount = 0;
						else
							dwCount++;
					}
				}
				lanfile.close();
			}
		}
		Temp = LOGDiskPath;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS"))
					{   //換工號重新計數  不綁mac範圍
						if(!AnsiString(filemsg.c_str()).Pos(EmployeeID))
							dwCount1 = 0;
						else
							dwCount1++;
					}
				}
				lanfile.close();
				return dwCount1;
			}
		}
		return dwCount;
	}
	return 0;
}
void TUSBHIDForm::NewFilePath(AnsiString Path)
{
	Path+="\\";
	if(!DirectoryExists(Path.c_str()))
	{
		AnsiString SS,Temp = "";
		while(true)
		{
			SS = Path.SubString(1,Path.Pos("\\"));
			Path.Delete(1,Path.Pos("\\"));
			Temp+=SS;
			_mkdir( Temp.c_str());
			if(Path == "")
				break;
		}
	}
}
bool TUSBHIDForm::CheckDiskName()
{
	DWORD DiskInfo = GetLogicalDrives();
	AnsiString DiskName,DiskVerifty = "DEVICE_LOG",SS;
	char Disk;
	for(int i =3;i<=25;i++)
	{
		if((DiskInfo&(0x01<<i))!=0)
		{
			char Volumelabel[20];
			DWORD SerialNumber;
			DWORD MaxCLength;
			DWORD FileSysFlag;
			char FileSysName[10];
			Disk = 0x41+i;
			DiskName = AnsiString(Disk)+":\\";
			GetVolumeInformation(DiskName.c_str(),Volumelabel,255,&SerialNumber,&MaxCLength,&FileSysFlag,FileSysName,255);
			if(!strcmp(Volumelabel,DiskVerifty.c_str()))
			{
				LOGDiskPath = DiskName+"\\ASMP\\log\\"+edtWorkOrderNumber->Text+"\\RealtekMACView";
				if(!DirectoryExists(LOGDiskPath.c_str())){
					_mkdir( DiskName.c_str());
					SS = DiskName + "\\ASMP";
					_mkdir( SS.c_str());
					SS = DiskName + "\\ASMP\\log";
					_mkdir( SS.c_str());
					SS = DiskName + "\\ASMP\\log\\"+edtWorkOrderNumber->Text;
					_mkdir(SS.c_str());
					_mkdir( LOGDiskPath.c_str());
				}
				return true;
			}
		}
	}
	return false;
}

