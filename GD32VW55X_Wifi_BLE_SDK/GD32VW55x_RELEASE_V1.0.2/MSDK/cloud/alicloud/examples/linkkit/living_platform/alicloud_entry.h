#ifndef __LIVING_PLATFORM_ENTRY_H__
#define __LIVING_PLATFORM_ENTRY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//#define LOG_LEVEL_DEBUG

typedef struct
{
    int argc;
    char **argv;
} living_platform_main_params_t;

extern void user_event_monitor(int event);
extern void cmd_alicloud_linkkit(int argc, char **argv);


#endif
