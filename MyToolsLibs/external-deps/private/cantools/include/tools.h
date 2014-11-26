#ifndef _FHKJEF_FDS_CAN_TOOLS
#define _FHKJEF_FDS_CAN_TOOLS

#include <windows.h>
#include "macro.h"
#include "rapidjson/document.h"
#include "jsoncpp/json/reader.h"

//#include <math.h>

_CANNP_BEGIN

//////////////////////////////////////////compare begin
template<typename T>T _max(T a, T b){ return (a > b) ? a : b; }
template<typename T>T _min(T a, T b){ return (a < b) ? a : b; }
template<typename T>T _abs(T a){ return (a < 0.0f) ? -a : a; }
template<typename T>T _sqrt(T a){ return sqrt(a); }
template<typename T>bool IsEqualFloat(T a, T b, DECIMALS eps = EPS){ return _abs(a - b) < eps; }
template<typename T>bool IsZero(T a){ return a < EPS && a > -EPS; }
//////////////////////////////////////////compare end


//////////////////////////////////////////lineIntersection begin
class lineIntersection
{
public:
	//�ж����߶��Ƿ��ཻ fast##
	static bool IsIntersection(const Point& aa, const Point& bb, const Point& cc, const Point& dd);
	//##//���߶�ab��cd���㣬 �������p ����ֵ��0��ʾ���ཻ�� 1��ʾ�淶�ཻ�� 2��ʾ�ǹ淶�ཻ##
	static uint8 IntersectionPos(const Point &a, const Point &b, const Point &c, const Point &d, Point &p);

	static uint8 LineInterCirclePos(const Point& ptStart, const Point& ptEnd, const Point& ptCenter, const DECIMALS Radius, Point& ptInter1, Point& ptInter2);
	static bool isLineInterCircle(const Point& ptStart, const Point& ptEnd, const Point& ptCenter, const DECIMALS Radius);

	static DECIMALS DistanceTwoPoints(const Point& a,const Point& b);
	static bool IsVaildDistance2Points(const Point& a, const Point& b,const DECIMALS& vdis );
private:
	////�ж�pk �Ƿ����߶�pipj��  check�Ƿ�������һ��##
	static bool IsInLinePart(const Point& pi, const Point& pj, const Point& pk, bool check = false);
	////�ж���ƽ���߶�pkpl��pipj�Ƿ����ص����� ##
	static bool AreLinesOverlap(const Point& pi, const Point& pj, const Point& pk, const Point& pl);
	// // ����ʽ  ##
	static DECIMALS Determinant(DECIMALS v1, DECIMALS v2, DECIMALS v3, DECIMALS v4);

	//## �жϸ�����  ����ֵ1��������0���㣻-1������ ##
	static int8 JudgeDecimals(DECIMALS d);
	//##//������������ˣ�AB * AC###
	static DECIMALS CrossProduct(const Point &A, const Point &B, const Point &C);
};

//////////////////////////////////////////lineIntersection end


//////////////////////////////////////////code type begin
class code
{
public:
	//##need delete ##
	static wchar_t* AsciiToUnicode(const char*, const uint16&, uint16&);
	//##need delete ##
	static char* UnicodeToAscii(const wchar_t*, const uint16&, uint16&);
	//##need delete ##
	static wchar_t* Utf8ToUnicode(const char*, const uint16&, uint16&);
	//##need delete ##
	static char* UnicodeToUtf8(const wchar_t*, const uint16&, uint16&);
};

//////////////////////////////////////////code type end

//////////////////////////////////////////random begin
class randoom
{
public:
	//##Get a randoom number. param@min and max must between 0 to 0x7fff  ##
	static int32 GetRandoom(int32 min = 0, int32 max = 0x7fff);
	//##Get a arrary index by arrary value prencent. param@1 arrary,@2count  ##
	static uint8 GetIndex(const uint16* pArr, const uint8& count);
private:
	static bool m_isInit;
};

/////////////////////////////////////////////////////////////////////////////random end


//////////////////////////////////////////encrypt begin
class encrypt
{
public:
	static void EncryptMD5(const char* src, uint8 len, std::string&);

	//encrypt only for pwd 0-128bit
	static bool EncryptAES(const char* src, const char* pwd, std::string&);
	static bool DecryptAES(const char* ciphertext, const char* pwd, std::string&);

	static void CPUID2(std::string&);
	static bool MAC_ETHERNET(std::string&);

	//encrypt files
	static void EncryptBuffer(const char* src,const uint32&, const char* pwd, std::string&);
	static void DecryptBuffer(const char* src, const uint32&, const char* pwd, std::string&);
	
};
/////////////////////////////////////////////////////////////////////////////encrypt end

class files
{
public:
	static char* ReadFile(const char* filePath);
	static bool WriteFile(const char* filePath, const char* pbuffer);
	static bool WriteFile(const char* filePath, const wchar_t* pbuffer);

	//////////////////////////////////////////Json begin
	static bool ReadFileRapidJson(const char* filePath, rapidjson::Document& document);
	static bool WriteFileRapidJson(const char* filePath, rapidjson::Document& document);
	static bool ReadFileJsonCpp(const char* filePath, Json::Value& value);
	static bool WriteFileJsonCpp(const char* filePath, Json::Value& value);

	/////////////////////////////////////////////////////////////////////////////Json end
};

_CANNP_END

//#include "random.hpp"
//#include "md5.hpp"
//#include "aes.hpp"
//#include "hardwareinfo.hpp"
//#include "rapidjson.hpp"
//#include "jsoncpp.hpp"
//#include "tools.hpp"
#endif