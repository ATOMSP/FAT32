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
 * 簇首所在扇区地址，从首目录算起
*/
#define CLUSTER_TO_SECTOR(pSysInfo,cluster)     (((pSysInfo)->firstdir_start_sector_addr) + (((cluster) - 2) * ((pSysInfo)->sector_num_of_cluster)))


/**
 * @description: SFAT获取FAT32文件系统参数
 * @param {MEDIUM_TYPES_T } dev
 * @return {*}
 */
SFAT_RES SFAT_ReadSysInfo(MEDIUM_TYPES_T dev);
/**
 * @description: SFAT查找对应文件名的文件，返回其文件信息
 * @param {const char* } filename
 * @param {file_Info *} fi
 * @return {*}
 */
SFAT_RES SFAT_FindFile(const char* filename,struct file_Info * fi);

void SFAT_ShowFDIInfo(struct file_Info * fi);


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