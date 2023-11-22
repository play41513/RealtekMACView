// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#pragma hdrstop
#include "Windows.h"
#include <dbt.h>

#include "USB_Hub_Tree.h"
#include "USB_Host.h"
#include "UsbHostInfoSet.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
USB_Hub_Tree::USB_Hub_Tree():
	Device_Tree()
{
	InitXml();
}
//---------------------------------------------------------------------------
USB_Hub_Tree::~USB_Hub_Tree(){

}

void USB_Hub_Tree::InitXml(){
	tinyxml2::XMLElement *root;
	tinyxml2::XMLDeclaration *dec;

	this->Clear();
	dec = this->NewDeclaration();
	this->LinkEndChild(dec);
	root = this->NewElement("USB_HUB_TREE");
	root->SetAttribute("ver", "1.0");
	this->LinkEndChild(root);
}
//---------------------------------------------------------------------------
bool USB_Hub_Tree::Refresh(){
	bool bl;
	GUID guid = GUID_DEVINTERFACE_USB_HOST_CONTROLLER;
	UsbHostInfoSet *pUsbHostInfoSet;


	InitXml();


	pUsbHostInfoSet = new UsbHostInfoSet(&guid);

	if(pUsbHostInfoSet){
		bl = pUsbHostInfoSet->GetUsbHubTree(this);
	}else{
    	bl = false;
	}

	delete pUsbHostInfoSet;

	return bl;
}
//---------------------------------------------------------------------------
