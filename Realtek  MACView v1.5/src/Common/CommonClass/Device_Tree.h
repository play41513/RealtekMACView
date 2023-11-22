// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef Device_TreeH
#define Device_TreeH

#include "tinyxml2.h"
using namespace tinyxml2;
//---------------------------------------------------------------------------
class Device_Tree : public tinyxml2::XMLDocument
{
private:
protected:
public:
	void Clear();
public:
	Device_Tree();
	~Device_Tree();
};
//---------------------------------------------------------------------------
#endif
