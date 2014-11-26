#include "tools.h"
#include <Iphlpapi.h>
#include < conio.h > 
#pragma comment(lib,"Iphlpapi.lib") 

void _CANNP_NAME::encrypt::CPUID2(std::string& strOut)
{
	strOut.clear();
	char szCPUID[16+1]="";
	unsigned long s1 = 0,s2=0;  
	_asm       
	{       
		mov     eax,01h       
		xor     edx,edx       
		cpuid       
		mov     s1,edx       
		mov     s2,eax       
	}  

	sprintf_s(szCPUID,sizeof(szCPUID), "%08X%08X", s1, s2);   
	strOut.append(szCPUID);
}
bool _CANNP_NAME::encrypt::MAC_ETHERNET(std::string& strOut)
{
	strOut.clear();
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	
	if (ERROR_SUCCESS == nRel)
	{
		char szMac[32]="";
		while (pIpAdapterInfo)
		{
			if (pIpAdapterInfo->Type==MIB_IF_TYPE_ETHERNET)
			{
				for (uint8 i=0;i<pIpAdapterInfo->AddressLength;++i)
					sprintf_s(szMac+2*i,2+1, "%02X", pIpAdapterInfo->Address[i]);
				break;
			}
		}
		strOut.append(szMac);
	}
	
	return strOut.size() > 0;
}
