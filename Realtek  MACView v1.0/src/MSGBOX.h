//---------------------------------------------------------------------------

#ifndef MSGBOXH
#define MSGBOXH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Keyboard.hpp>
//---------------------------------------------------------------------------
class TfrmMsg : public TForm
{
__published:	// IDE-managed Components
	TPanel *plWOInfo;
	TLabel *Label2;
	TPanel *Panel39;
	TPanel *Panel40;
	TEdit *edtSetWorkOrderNumber;
	TPanel *Panel5;
	TEdit *edtSetEmployeeID;
	TPanel *Panel41;
	TBitBtn *btnID_ok;
	TTouchKeyboard *TouchKeyboard;
	TPanel *plSwitch;
	TPanel *Panel2;
	TPanel *Panel3;
	TEdit *edtSetModel;
	TPanel *plBarcode;
	TLabel *lbBarcode;
	TPanel *Panel4;
	TBitBtn *BitBtn1;
	TEdit *edtBarcodeMAC;
	TBitBtn *BitBtn2;
	TPanel *Panel1;
	TPanel *Panel6;
	TEdit *edtWorkStation;
	void __fastcall btnID_okClick(TObject *Sender);
	void __fastcall plSwitchClick(TObject *Sender);
	void __fastcall edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall BitBtn2Click(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall edtSetEmployeeIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtSetWorkOrderNumberKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtSetModelKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtBarcodeMACChange(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall edtWorkStationKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);






private:	// User declarations
public:		// User declarations
	__fastcall TfrmMsg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
#endif
