// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef USB_Hub_TreeH
#define USB_Hub_TreeH
#include "USB_Tree.h"
//#include "USB_Hub.h"

//---------------------------------------------------------------------------
class USB_Hub_Tree : public Device_Tree
{
private:
//	bool ProcessUsbHub(USB_Hub &usbHub);
//	bool EnumerateUsbHub(tinyxml2::XMLElement *trvParent, int port, TCHAR *symbolLink, int cb);
//	void AddNode(char *element, TCHAR *text, int szCch, int attributeValue, int attributeValue2);
//	void AddHostNode(TCHAR *usbHost_Symbol_Link, size_t szCch);
//	void AddHostRootNode(TCHAR *usbHostRoot_Symbol_Link, size_t szCch);
public:
	void InitXml();
	bool Refresh();
public:
	USB_Hub_Tree();
	~USB_Hub_Tree();
};
//---------------------------------------------------------------------------
#endif
