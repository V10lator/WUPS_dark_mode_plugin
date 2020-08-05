#include <wups.h>
#include <coreinit/title.h>

#ifdef WUPS_DM_DEBUG
    #include <nsysnet/socket.h>
    #include <utils/logger.h>
#endif

#define WII_U_MENU_TITLE_ID_JAP (0x0005001010040000)
#define WII_U_MENU_TITLE_ID_EUR (0x0005001010040200)
#define WII_U_MENU_TITLE_ID_USA (0x0005001010040100)

#define WII_U_ADDRESS (void *)0x105DD2A8

// Mandatory plugin information.
WUPS_PLUGIN_NAME("Dark Mode");
WUPS_PLUGIN_DESCRIPTION("Wii U Menu dark mode");
WUPS_PLUGIN_VERSION("v0.2");
WUPS_PLUGIN_AUTHOR("JacquesCedric & V10lator");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_ALLOW_KERNEL()

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
#ifdef WUPS_DM_DEBUG
            DEBUG_FUNCTION_LINE("Patching!\n");
#endif
            WUPS_KernelWrite(WII_U_ADDRESS, 0x3C800000);  
    }
}
