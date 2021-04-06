#include <wups.h>
#include <coreinit/memory.h>
#include <coreinit/title.h>

#ifdef __LOGGING__
	#include <whb/log.h>
	#include <whb/log_udp.h>
#else
	#define WHBLogPrint(...)
	#define WHBLogPrintf(...)
#endif

#define WII_U_MENU_TITLE_ID_JAP (0x0005001010040000)
#define WII_U_MENU_TITLE_ID_EUR (0x0005001010040200)
#define WII_U_MENU_TITLE_ID_USA (0x0005001010040100)

#define EUR_ADDY (uint32_t *)0x105DD2A8
#define USA_ADDY (uint32_t *)0x105DD0A4
#define JAP_ADDY USA_ADDY

// Mandatory plugin information.
WUPS_PLUGIN_NAME("Dark Mode");
WUPS_PLUGIN_DESCRIPTION("Wii U Menu dark mode");
WUPS_PLUGIN_VERSION("v2.0");
WUPS_PLUGIN_AUTHOR("JacquesCedric & V10lator");
WUPS_PLUGIN_LICENSE("GPLv3");

static inline void darkenU(uint32_t *addy)
{
	if(*(uint64_t *)addy != 0x3F80000040000000 || *(addy - 1) != 0x00000000)
	{
		WHBLogPrint("Pattern not found at hardcoded memory address!\n");
		WHBLogPrint("Searching the pattern in memory...\n");
		
		addy = (uint32_t *)0x105DD000;
		uint32_t a;
		bool found = false;
		while(addy < (uint32_t *)0x10600000)
		{
			a = *addy++;
			if(a != 0x00000000)
				continue;
			
			a = *addy;
			if(a != 0x3F800000)
				continue;
			
			a = *(addy + 1);
			if(a != 0x40000000)
				continue;
			
			found = true;
			break;
		}
		
		if(!found)
		{
			WHBLogPrint("Not found!\n");
			return;
		}
	}
	
	WHBLogPrintf("Patching at 0x%08X!\n", addy);
	*(uint8_t *)addy = 0x3C;
}

// Gets called once the loader exists.
ON_APPLICATION_START()
{
#ifdef __LOGGING__
	WHBLogUdpInit();
	WHBLogPrint("ON_APPLICATION_START()!\n");
#endif
	
	uint32_t *addy;
	switch(OSGetTitleID())
	{
		case WII_U_MENU_TITLE_ID_EUR:
			addy = EUR_ADDY;
			break;
		case WII_U_MENU_TITLE_ID_USA:
			addy = USA_ADDY;
			break;
		case WII_U_MENU_TITLE_ID_JAP:
			addy = JAP_ADDY;
			break;
		default:
			return;
	}
	
	darkenU(addy);
}

ON_APPLICATION_REQUESTS_EXIT()
{
	WHBLogPrint("ON_APPLICATION_REQUESTS_EXIT()!\n");
	WHBLogUdpDeinit();
}
