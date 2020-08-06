#include <wups.h>
#include <coreinit/memory.h>
#include <coreinit/title.h>

#ifdef __LOGGING__
	#include <nsysnet/socket.h>
	#include <utils/logger.h>
#else
	#define DEBUG_FUNCTION_LINE
#endif

#define WII_U_MENU_TITLE_ID_JAP (0x0005001010040000)
#define WII_U_MENU_TITLE_ID_EUR (0x0005001010040200)
#define WII_U_MENU_TITLE_ID_USA (0x0005001010040100)

// Mandatory plugin information.
WUPS_PLUGIN_NAME("Dark Mode");
WUPS_PLUGIN_DESCRIPTION("Wii U Menu dark mode");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("JacquesCedric & V10lator");
WUPS_PLUGIN_LICENSE("GPLv3");

static inline void darkenU()
{
#ifdef __LOGGING__
	DEBUG_FUNCTION_LINE("Searching address...\n");
#endif
	
	uint32_t *addy = (uint32_t *)0x105DD000;
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
#ifdef __LOGGING__
		DEBUG_FUNCTION_LINE("Not found!\n");
#endif
		return;
	}
	
#ifdef __LOGGING__
	DEBUG_FUNCTION_LINE("Patching!\n");
#endif
	OSBlockSet(addy, 0x3C, 1);
}

// Gets called once the loader exists.
ON_APPLICATION_START(args)  // TODO: The example plugin doesn't use args and compiles fine, why do we need args?
{
#ifdef __LOGGING__
	socket_lib_init();
	log_init();
	DEBUG_FUNCTION_LINE("ON_APPLICATION_START()!\n");
#endif
	
	switch(OSGetTitleID())
	{
		case WII_U_MENU_TITLE_ID_EUR:
		case WII_U_MENU_TITLE_ID_USA:
		case WII_U_MENU_TITLE_ID_JAP:
			darkenU();
	}
}
