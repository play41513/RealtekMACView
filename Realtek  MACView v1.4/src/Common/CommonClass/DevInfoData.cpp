//---------------------------------------------------------------------------

#pragma hdrstop

#include "DevInfoData.h"
#include <tchar.h>
#include <strsafe.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
DevInfoData::DevInfoData()
{
	ZeroMemory(this, sizeof(SP_DEVINFO_DATA));
	this->cbSize = sizeof(SP_DEVINFO_DATA);
}
//---------------------------------------------------------------------------
DevInfoData::~DevInfoData(){
}
//---------------------------------------------------------------------------
