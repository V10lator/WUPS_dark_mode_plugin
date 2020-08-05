#include <wups.h>
#include <nsysnet/socket.h>
#include <utils/logger.h>
#include <coreinit/title.h>

#define WII_U_MENU_TITLE_ID_JAP (0x0005001010040000)
#define WII_U_MENU_TITLE_ID_EUR (0x0005001010040100)
#define WII_U_MENU_TITLE_ID_USA (0x0005001010040200)

// Mandatory plugin information.
WUPS_PLUGIN_NAME("Dark Mode");
WUPS_PLUGIN_DESCRIPTION("Wii U Menu dark mode");
WUPS_PLUGIN_VERSION("v0.2");
WUPS_PLUGIN_AUTHOR("JacquesCedric & V10lator");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_ALLOW_KERNEL()

// Gets called once the loader exists.
ON_APPLICATION_START(args){
    if(!args.kernel_access)
        return;

    uint64_t titleID = OSGetTitleID();
    if(titleID != WII_U_MENU_TITLE_ID_JAP &&
       titleID != WII_U_MENU_TITLE_ID_USA &&
       titleID != WII_U_MENU_TITLE_ID_EUR)
        return;

    socket_lib_init();
    log_init();
    int *address = (int *)0x105DD0A8;
    WUPS_KernelWrite(address, 0x3C800000);  
}