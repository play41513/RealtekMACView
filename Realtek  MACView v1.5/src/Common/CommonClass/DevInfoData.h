//---------------------------------------------------------------------------

#ifndef DevInfoDataH
#define DevInfoDataH
//---------------------------------------------------------------------------
#include <windows.h>
#ifdef __cplusplus
	extern "C" {
#endif
#include <setupapi.h> //HDEVINFO
#ifdef __cplusplus                                                                               d
	}
#endif
//---------------------------------------------------------------------------
class DevInfoData : SP_DEVINFO_DATA
{
private:
public:
	DevInfoData();
	~DevInfoData();
};
//---------------------------------------------------------------------------
#endif
