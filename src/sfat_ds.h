#ifndef __SFAT_DS_H
#define __SFAT_DS_H




#ifdef __cplusplus
extern "C"{
#endif

#include "sfat_type.h"
#include "sfat_macro.h"
/**
 * DPT->DPT.ITEM 字段数据结构
*/
struct __attribute__((__packed__)) DPT_Item{
    UBYTE is_active;                    // 引导指示符
    UBYTE head;                         // 开始磁头
    UBYTE start_sechead_cylider[2];     // 开始扇区和柱面
    UBYTE id;                           // 分区类型
    UBYTE tail;                         // 结束磁头
    UBYTE end_sechead_cylider[2];       // 结束扇区和柱面
    UBYTE part_start_sector_addr[4];    // 分区起始扇区
    UBYTE part_total_sectors[4];        // 总扇区数
};

/**
 * MBR->DPT 字段数据结构
*/
struct __attribute__((__packed__)) MBR_Typedef{
    UBYTE boot_code[BOOT_CODE_SIZE];
    struct DPT_Item dpt[DPT_ITEM_NUM];
    UBYTE magic_num[2];
};
/**
 * BPB 字段数据结构
*/
struct __attribute__((__packed__)) BPB_Typedef{
    UBYTE sector_bytes[2];          // 扇区字节数
    UBYTE sector_num_of_cluster;    // 每簇扇区数
    UBYTE sector_num_of_rsvd[2];    // 保留扇区数
    UBYTE fat_num;                  // fat表个数
    UBYTE unused1[15];              // 未用区1
    UBYTE sector_total_num[4];      // 总扇区数
    UBYTE sector_num_of_fat[4];     // 每个fat表的扇区数
    UBYTE unused2[50];              // 未用区2
};
/**
 * DBR 字段数据结构
*/
struct __attribute__((__packed__)) DBR_Typedef{
    UBYTE unused_code[UNUSED_CODE_SIZE];
    struct BPB_Typedef bpb;
    UBYTE boot_code[BOOT_CODE2_SIZE];
    UBYTE magic_num[2];
};

/**
 * 文件目录项结构
*/
struct __attribute__((__packed__)) PDI_Typedef{
    UBYTE file_name[HOST_FILE_NAME_BLEN];    // 主文件名
    UBYTE ext_file_name[EXT_FILE_NAME_BLEN]; // 拓展文件名
    UBYTE file_attribute;                   // 文件属性
    UBYTE unused;                           // 系统保留
    UBYTE tim_10ms;                         // 10ms精确值
    UBYTE create_file_time[CTIME_BLEN];      // 创建时间
    UBYTE create_file_date[CDATE_BLEN];      // 创建日期
    UBYTE lasted_access_file_date[ADATE_BLEN];// 访问日期
    UBYTE start_cluster_high16[2];          // 起始簇高16位
    UBYTE modify_file_time[MTIME_BLEN];      // 修改时间
    UBYTE modify_file_date[MDATE_BLEN];      // 修改日期    
    UBYTE start_cluster_low16[2];          // 起始簇低16位
    UBYTE file_data_len[FILE_DATA_BLEN];     // 文件数据长度(byte)
};


/**
 * sfat 文件系统参数表
*/
struct sfat_system_param{
    UBYTE sys_id;                   // 分区类型
    UINT  part_start_sector_addr;   // 分区起始扇区地址
    UINT  part_total_sectors;       // 分区总扇区数
    USHORT bytes_of_sector;         // 扇区字节数
    UBYTE sector_num_of_cluster;    // 每簇扇区数
    USHORT sector_num_of_rsvd;      // 保留扇区数    
    UBYTE fat_num;                  // fat表个数
    UINT sector_num_of_fat;         // 每个fat表的扇区数  
    UINT sector_total_num;          // 总扇区数    
    UINT fat1_start_sector_addr;    // fat1起始扇区
    UINT firstdir_start_sector_addr;// 首目录起始扇区
};

/**
 * 日期和时间数据结构
*/
struct Time_Date{
    USHORT year;   // 年月日
    UBYTE  month;
    UBYTE  day;
    UBYTE  hour;   // 时分秒
    UBYTE  min;
    UBYTE  sec; 
};

/**
 * 文件信息
*/
struct file_Info{
    UBYTE filename[FILE_NAME_BLEN];     // 文件名
    UBYTE file_attr;                    // 文件属性
    struct Time_Date ctim;               // 创建时间日期
    struct Time_Date atim;               // 访问时间日期
    struct Time_Date mtim;               // 修改时间日期
    UINT  file_start_cluster;           // 文件起始簇
    UINT  file_size;                    // 文件大小
};

#ifdef __cplusplus
}
#endif




#endif










