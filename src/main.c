#include <wups.h>
#include <coreinit/title.h>

#ifdef WUPS_DM_DEBUG
    #include <nsysnet/socket.h>
    #include <utils/logger.h>
#endif

#define WII_U_MENU_TITLE_ID_JAP (0x0005001010040000)
#define WII_U_MENU_TITLE_ID_EUR (0x0005001010040200)
#define WII_U_MENU_TITLE_ID_USA (0x0005001010040100)

// Mandatory plugin information.
WUPS_PLUGIN_NAME("Dark Mode");
WUPS_PLUGIN_DESCRIPTION("Wii U Menu dark mode");
WUPS_PLUGIN_VERSION("v0.2");
WUPS_PLUGIN_AUTHOR("JacquesCedric & V10lator");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_ALLOW_KERNEL()

static inline void darkenU()
{
#ifdef WUPS_DM_DEBUG
	DEBUG_FUNCTION_LINE("Searching address...\n");
#endif
	
	uint32_t addy = 0x105DD000;
	uint32_t a;
	bool found = false;
	for(uint32_t i = addy; i < 0x10600000; i += 0x00000004)
	{
		a = WUPS_KernelRead((void *)i);
		if(a != 0x3F800000)
			continue;
		
		a = WUPS_KernelRead((void *)(i - 0x00000004));
		if(a != 0x00000000)
			continue;
		
		a = WUPS_KernelRead((void *)(i + 0x00000004));
		if(a != 0x40000000)
			continue;
		
		addy = i;
		found = true;
		break;
	}
	
	if(!found)
	{
#ifdef WUPS_DM_DEBUG
		DEBUG_FUNCTION_LINE("Not found!\n");
#endif
		return;
	}
	
#ifdef WUPS_DM_DEBUG
	DEBUG_FUNCTION_LINE("Patching!\n");
#endif
	WUPS_KernelWrite((void *)addy, 0x3C800000);
}

// Gets called once the loader exists.
ON_APPLICATION_START(args){
#ifdef WUPS_DM_DEBUG
    socket_lib_init();
    log_init();
    DEBUG_FUNCTION_LINE("ON_APPLICATION_START()!\n");
#endif

    if(!args.kernel_access)
        return;

#ifdef WUPS_DM_DEBUG
    DEBUG_FUNCTION_LINE("Kernel access!\n");
#endif

    switch(OSGetTitleID())
    {
        case WII_U_MENU_TITLE_ID_EUR:
        case WII_U_MENU_TITLE_ID_USA:
        case WII_U_MENU_TITLE_ID_JAP:
            darkenU();
    }
}
