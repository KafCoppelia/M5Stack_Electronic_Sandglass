#ifndef _USER_H_
#define _USER_H_

#include "sandglass.h"

void User_Setup(void);
void User_Loop(void);
void led_heartbeat(void);

extern Sandglass sandglass;

#endif