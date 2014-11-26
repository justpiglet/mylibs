#ifndef _FHKJEF_FDS_CAN_MACRO
#define _FHKJEF_FDS_CAN_MACRO

#ifdef BUILD_DLL
#define DLL_EX_OR_IM dllexport
#else
#define DLL_EX_OR_IM dllimport
#endif // BUILD_DLL

#define _CANNP_NAME canspace
#define _CANNP_BEGIN	namespace _CANNP_NAME {
#define _CANNP_END		}
#define _USE_CANNP using namespace _CANNP_NAME;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

#if defined(_MSC_VER) && !defined(__MWRKS__) && defined(_M_IX86)
typedef __int64							int64;
typedef unsigned __int64				uint64;
#else
typedef signed	long long				int64;
typedef unsigned long long				uint64;
#endif

typedef uint32 SCORE;
typedef int32  W_VAL;
typedef uint64 PROFIT;
typedef float RATE;
typedef float DECIMALS;

#ifdef _UNICODE
typedef wchar_t TCHAR;
#define __T(x) L##x
#define _T(x) __T(x)
#else
typedef char TCHAR;
#define __T(x) x
#define _T(x) __T(x)
#endif // _UNICODE

#define EPS (1e-5)
#define EPS_LOW (5e-3)

typedef struct Point
{
	DECIMALS x, y;
	Point(DECIMALS x = 0.0f, DECIMALS y = 0.0f) :x(x), y(y){}


	Point& operator + (Point& other)
	{
		this->x = this->x + other.x;
		this->y = this->y + other.y;

		return *this;
	}

	Point& operator / (DECIMALS div)
	{
		this->x = this->x / div;
		this->y = this->y / div;

		return *this;
	}

}*PPoint;
#endif