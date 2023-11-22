// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#ifndef HidInfoSetH
#define HidInfoSetH
//---------------------------------------------------------------------------
#include "DevInfoSet.h"
//---------------------------------------------------------------------------
class HidInfoSet : public DevInfoSet
{
private:
	GUID _HidGlassGuid;
private:
	GUID* GetHidGuid();
public:
	HidInfoSet(const PCTSTR enumerator = NULL, HWND hwnd = NULL);
	~HidInfoSet();
};
//---------------------------------------------------------------------------
#endif
