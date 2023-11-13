#include "sfat.h"
#include <stdio.h>



int main(){
    struct sfat_system_param sysInfo;
    SFAT_ReadSysInfo(DEV_SD,&sysInfo);
    return 0;
}










