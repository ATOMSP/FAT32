#ifndef __SFAT_MACRO_H
#define __SFAT_MACRO_H




#ifdef __cplusplus
extern "C"{
#endif

#include "sfat_type.h"

#ifndef NULL
    #define NULL ((void*)0)
#endif

/**
 * dev ID
*/
#define DEV_SD  (MEDIUM_TYPES_T)0

/**
 * MBR
*/
#define BOOT_CODE_SIZE          446
#define DPT_ITEM_NUM            4
#define SECTOR_MBR_ADDR         (ULONG)0
#define SECTOR_MBR_SIZE         512

#define FILE_SYSTEM_TYPE_NONE   0x00
#define FILE_SYSTEM_TYPE_FAT32  0x0B
#define FILE_SYSTEM_TYPE_FAT32_C   0x0C

/**
 * DBR
*/
#define DBR_MARK {0xEB,0x58,0x90}
#define BOOT_CODE2_SIZE 420
#define UNUSED_CODE_SIZE 11
#define SECTOR_DBR_SIZE 512

/**
 * PDI
*/
#define HOST_FILE_NAME_BLEN 8
#define EXT_FILE_NAME_BLEN  3
#define CTIME_BLEN          2
#define CDATE_BLEN          2
#define ADATE_BLEN          2
#define MTIME_BLEN          2
#define MDATE_BLEN          2
#define FILE_DATA_BLEN      4

/**
 * file info
*/
#define FILE_NAME_BLEN        13
#define DATE_BASE_YEAR        1980    // 解析日期掩码
#define FILE_DATE_YEAR_MASK   0xFE00
#define FILE_DATE_YEAR_TB     9   
#define FILE_DATE_MON_MASK    0x01E0
#define FILE_DATE_MON_TB      5  
#define FILE_DATE_DAY_MASK    0x001F
#define FILE_TIME_HOUR_MASK   0xF800
#define FILE_TIME_HOUR_TB     11
#define FILE_TIME_MIN_MASK    0x07E0
#define FILE_TIME_MIN_TB      5
#define FILE_TIME_SEC_MASK    0x001F


/**
 * sfat error code
*/
#define RUN_OK                  1
#define UNKNOWN_FS              -1
#define NOT_MBR                 -2
#define NOT_DBR                 -3
#define NO_FAT_SYS              -4
#define READ_DISK_ERROR         -5
#define PART_SECTOR_NUM_UNEQU   -6
#define PARAM_ERROR             -7
#ifdef __cplusplus
}
#endif




#endif














