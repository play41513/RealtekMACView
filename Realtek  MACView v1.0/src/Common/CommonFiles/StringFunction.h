// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.2
//---------------------------------------------------------------------------
#ifndef StringFunctionH
#define StringFunctionH                            file:///D:/CODE/CODING/COMMON/inc/StringFunction.h
//---------------------------------------------------------------------------
//#include <ctype.h>
#include <cctype.h>
#include <string>
#include <algorithm>
#include <stdlib.h>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
class Ws2Cs
{
private:
	char *cs;
public:
	char *c_str(){
    	return cs;
	}
public:
	Ws2Cs(wchar_t* ws){
		size_t cb;
		const size_t sz = (wcslen(ws)+1) * sizeof(wchar_t);

		setlocale(LC_ALL,"");
		cs = new char[sz];

		if(cs != NULL){
		int x = wcstombs(cs, ws, sz);
			if(wcstombs(cs, ws, sz) > 1){
				cs[sz] = '\0';
			}else{
				SAFE_ARRAY_DELETE(cs);
			}
		}
	}
	~Ws2Cs(){
		SAFE_ARRAY_DELETE(cs);
	}
};
//---------------------------------------------------------------------------
//分割字串
template <typename T>
std::vector<T>::size_type Split(const T& input, const T& delims, std::vector<T>& output)
{

	typename T::size_type beg_index , end_index;

	beg_index = input.find_first_not_of(delims);

	while(beg_index != T::npos)  // or just T::npos on old compilers
	{
		end_index = input.find_first_of(delims,beg_index);
		if (end_index == std::string::npos) end_index = input.length();

		output.push_back( input.substr(beg_index,end_index-beg_index) );

		beg_index = input.find_first_not_of(delims,end_index);
	}

	return output.size();
}

typedef std::basic_string<_TCHAR> _tstring;
std::vector<_tstring>::size_type Split(
	const _TCHAR* str,
	const _TCHAR* delim,
	std::vector<_tstring>& results,
	bool empties = true)
{
	const _TCHAR* pstr = str;
	const _TCHAR* r = _tcsstr(pstr, delim);
	std::size_t dlen = _tcslen(delim);
	while( r )
	{
		if( empties || r > pstr )
		{
			results.push_back(_tstring(pstr, _tstring::size_type(r - pstr)));
		}
		pstr = r + dlen;
		r = _tcsstr(pstr, delim);
	}
	if( empties || _tcslen(pstr) > 0)
	{
		results.push_back(_tstring(pstr));
	}
	return results.size();
}
//---------------------------------------------------------------------------
std::wstring StringToWstring(const std::string &str)
{
	std::wstring wstr;

    LPCSTR pszSrc = str.c_str();
    int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
    if (nLen == 0)
		return NULL;

    wchar_t* pwszDst = new wchar_t[nLen];
    if (!pwszDst)
        return NULL;

    MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
	wstr.append(pwszDst);
    delete[] pwszDst;

    return wstr;
}
//---------------------------------------------------------------------------
std::string WstringToString(const std::wstring& wstr)
{
	std::string str;
	bool bl;

	LPCWSTR pwszSrc = wstr.c_str();
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
		return NULL;

	char* pszDst = new char[nLen + 1];
	if (!pszDst)
		return NULL;

	if(WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL) != 0){
		pszDst[nLen] = '\0';
		str.append(pszDst);
		bl = true;
	}else{
		bl = false;
	}

	delete[] pszDst;

	if(bl){
		return str;
	}else{
    	return NULL;
	}
}
//---------------------------------------------------------------------------
TCHAR * upper(TCHAR str[]){
	for(unsigned int i=0; i<_tcslen(str); i++){
		str[i] = toupper(str[i]);
	}

	return str;
}

TCHAR * lower(TCHAR str[]){
	for(unsigned int i=0; i<_tcslen(str); i++){
		str[i] = tolower(str[i]);
	}

	return str;
}

void ToUpper(TCHAR *str){
	int i = -1;

	while(str[++i]){
		str[i] = toupper(str[i]);
	}
}

void ToLower(TCHAR *str){
	int i = -1;

	while(str[++i]){
		str[i] = tolower(str[i]);
	}
}

void ToUpper(string &str){
	transform(str.begin(), str.end(), str.begin(), toupper);
}

void ToLower(string &str){
	transform(str.begin(), str.end(), str.begin(), tolower);
}

int HexToInt(const TCHAR *HexStr) {
	int iIndex, iHex, totalChar;
	TCHAR HexChar[8];

	_tcscpy_s(HexChar, 8, HexStr);
//	strcpy(HexChar, HexStr.c_str()); // 把字串轉成字元陣列

//	HexStr.UpperCase(); // 把字串轉成全大寫
	ToUpper(HexChar);

	iHex = 0;
//	totalChar = HexStr.Length(); // 取得字串的長度
	totalChar = _tcslen(HexChar); // 取得字串的長度

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
#endif
