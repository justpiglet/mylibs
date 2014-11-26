#include "tools.h"
#include "time.h"
#define CaGetTickCout GetTickCount64

	//##Get a randoom number. param@min and max must between 0 to 0x7fff  ##

bool _CANNP_NAME::randoom::m_isInit=false;
int32 _CANNP_NAME::randoom::GetRandoom(int32 min /*= 0*/, int32 max /*= 0x7fff*/)
{
	if (!m_isInit)
	{
		unsigned int uiseed = (unsigned int)CaGetTickCout();
		uiseed = uiseed + (unsigned)time(NULL);
		uiseed = uiseed & 0x000007ff;
		uiseed = uiseed*uiseed;
		srand(uiseed);
		m_isInit = true;
	}

	if (min < 0)min = 0;
	if (max > 0x7fff)max = 0x7fff;
			
			
	if(max <= min)return min;
	else if (min == 0 && max == 0x7fff)return rand();
	else return min + (int32)(rand() % (max - min + 1));
};

uint8 _CANNP_NAME::randoom::GetIndex(const uint16* pArr, const uint8& count)
{
	uint8 index(0);
	uint32 totalval(0);
	for (int i = 0; i < count; ++i)
		totalval = totalval + pArr[i];
	int randValue = GetRandoom(0,totalval);
	int totalTemp(0);
	for (int i = 0; i < count; ++i)
	{
		totalTemp += pArr[i];
		if (randValue < totalTemp || i == count - 1){index = i;break;}	
	}

	return index;
};