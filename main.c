#include "sfat.h"
#include <stdio.h>



int main(){
    SFAT_RES res;
    /* 读取系统参数 */
    SFAT_ReadSysInfo(DEV_SD);

    /* 读取文件目录项 */
    struct file_Info f;
    res = SFAT_FindFile("DEMO.TXT",&f);
    if(res > 0){
        SFAT_ShowFDIInfo(&f);
    }
    else{
        printf("Error Code : [%d]",res);
    }
    return 0;
}










