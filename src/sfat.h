#ifndef __SFAT_H
#define __SFAT_H




#ifdef __cplusplus
extern "C"{
#endif

/**
 * 头文件
*/
#include "sfat_type.h"
#include "sfat_ds.h"


/**
 * 转换小端模式
*/
static inline UINT SHIFT_LITTLE_ENDIAN(UBYTE * ptr,UINT len){
    UINT res = 0;
    if(len >= 1) res |= (UINT)(ptr[0]);
    if(len >= 2) res |= ((UINT)(ptr[1])) << 8;
    if(len >= 3) res |= ((UINT)(ptr[2])) << 16;
    if(len >= 4) res |= ((UINT)(ptr[3])) << 24;
    return res;
}


/**
 * @description: SFAT获取FAT32文件系统参数
 * @param {MEDIUM_TYPES_T } dev
 * @param {sfat_system_param *} sysInfo
 * @return {*}
 */
SFAT_RES SFAT_ReadSysInfo(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo);

/**
 * 磁盘驱动移植接口
*/
SFAT_RES Read_dev_port(
    MEDIUM_TYPES_T pdev,
    UBYTE * buff,
    ULONG sector,
    UINT  cnt
);
SFAT_RES Write_dev_port(
    MEDIUM_TYPES_T pdev,
    const UBYTE * buff,
    ULONG sector,
    UINT  cnt
);
SFAT_RES Ctrl_dev_port(
    MEDIUM_TYPES_T pdev,
    UINT cmd,
    UBYTE * buff
);

#ifdef __cplusplus
}
#endif




#endif