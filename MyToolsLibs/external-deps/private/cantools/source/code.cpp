#include "tools.h"

wchar_t* _CANNP_NAME::code::AsciiToUnicode(const char* pSrc, const uint16& len, uint16&wcsLen)
{
	if (!pSrc || len < 1)
		return nullptr;

	wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, pSrc, len, NULL, 0);
	if (wcsLen > 0)
	{
		wchar_t* wszString = new wchar_t[wcsLen + 1];
		::MultiByteToWideChar(CP_ACP, NULL, pSrc, len, wszString, wcsLen);
		wszString[wcsLen] = '\0';
		return wszString;
	}
	else
		return nullptr;
}

char* _CANNP_NAME::code::UnicodeToAscii(const wchar_t* pwSrc, const uint16& wLen, uint16& len)
{
	if (!pwSrc || wLen < 1)
		return nullptr;

	len = ::WideCharToMultiByte(CP_ACP, NULL, pwSrc, wLen, NULL, 0,NULL,NULL);
	if (len > 0)
	{
		char* szString = new char[len + 1];
		::WideCharToMultiByte(CP_ACP, NULL, pwSrc, wLen, szString, len, NULL, NULL);
		szString[len] = '\0';
		return szString;
	}
	else
		return nullptr;
}

wchar_t* _CANNP_NAME::code::Utf8ToUnicode(const char* pSrc, const uint16& len, uint16& wcsLen)
{
	if(!pSrc || len < 1)
		return nullptr;

	wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, pSrc, len, NULL, 0);
	if (wcsLen > 0)
	{
		wchar_t* wszString = new wchar_t[wcsLen + 1];
		::MultiByteToWideChar(CP_UTF8, NULL, pSrc, len, wszString, wcsLen);
		wszString[wcsLen] = '\0';
		return wszString;
	}
	else
		return nullptr;
}

char* _CANNP_NAME::code::UnicodeToUtf8(const wchar_t* pwSrc, const uint16& wLen, uint16&len)
{
	if (!pwSrc || wLen < 1)
		return nullptr;

	len = ::WideCharToMultiByte(CP_UTF8, NULL, pwSrc, wLen, NULL, 0, NULL, NULL);
	if (len > 0)
	{
		char* szString = new char[len + 1];
		::WideCharToMultiByte(CP_UTF8, NULL, pwSrc, wLen, szString, len, NULL, NULL);
		szString[len] = '\0';
		return szString;
	}
	else
		return nullptr;
}
