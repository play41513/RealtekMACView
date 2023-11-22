//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MSGBOX.h"
#include "Uint1.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
__fastcall TfrmMsg::TfrmMsg(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::btnID_okClick(TObject *Sender)
{
	if(edtSetWorkOrderNumber->Text=="")
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
	else if(edtSetModel->Text=="")
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
	else if(edtSetEmployeeID->Text=="")
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
	else
		ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::plSwitchClick(TObject *Sender)
{
	if(plSwitch->Caption=="V")
	{
		plSwitch->Caption = "^";
		frmMsg->Height = 452;
	}
	else
	{
		plSwitch->Caption = "V";
		frmMsg->Height = 269;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if(edtBarcodeMAC->Text.Length()==12&&Key==13)
	{
		USBHIDForm->mskedBurntMacPre->Text = edtBarcodeMAC->Text.SubString(1,6);
		USBHIDForm->mskedtBurnMac->Text = edtBarcodeMAC->Text.SubString(7,6);
		ModalResult = mrOk;
	}
	else if(edtBarcodeMAC->Text.Length()<12&&Key==13)
		edtBarcodeMAC->Text = "";
	else if(Key == 8)
		edtBarcodeMAC->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::BitBtn2Click(TObject *Sender)
{
	edtBarcodeMAC->Text = "";
	edtBarcodeMAC->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::BitBtn1Click(TObject *Sender)
{
	ModalResult = mrNo;
}
//---------------------------------------------------------------------------



void __fastcall TfrmMsg::edtSetEmployeeIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
		btnID_ok->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetWorkOrderNumberKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift)
{
	if(Key==13)
		edtWorkStation->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetModelKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
		edtSetEmployeeID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtBarcodeMACChange(TObject *Sender)
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



void __fastcall TfrmMsg::FormShow(TObject *Sender)
{
	if(frmMsg->Tag)
	{
		edtBarcodeMAC->Text = "";
		frmMsg->plWOInfo->Enabled = false;
		frmMsg->plWOInfo->Height  = 0;
		frmMsg->plSwitch->Enabled = false;
		frmMsg->plSwitch->Height  = 0;
		frmMsg->TouchKeyboard->Top = 182;
		frmMsg->Height = 160;
		edtBarcodeMAC->SetFocus();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtWorkStationKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
		edtSetModel->SetFocus();
}
//---------------------------------------------------------------------------

