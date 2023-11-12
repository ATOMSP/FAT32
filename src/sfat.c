#include "sfat.h"
#include "sfat_macro.h"

/**
 * static function
*/
static SFAT_RES SFAT_Analysis_DPT(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo);
static SFAT_RES SFAT_Analysis_BPB(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo);
static SFAT_RES SFAT_Analysis_PDI(struct PDI_Typedef * pdi,struct file_Info * info);


/**
 * @description: SFAT获取FAT32文件系统参数
 * @param {MEDIUM_TYPES_T } dev
 * @param {sfat_system_param *} sysInfo
 * @return {*}
 */
SFAT_RES SFAT_GetSysInfo(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo)
{
    SFAT_RES res = RUN_OK;
    res = SFAT_Analysis_DPT(dev,sysInfo);
    if(res > 0){
       SFAT_Analysis_BPB(dev,sysInfo);
    }
    return res;
}


/**
 * @description: 解析DPT字段
 * @param {MEDIUM_TYPES_T } dev
 * @param {sfat_system_param *} sysInfo
 * @return {*}
 */
static SFAT_RES SFAT_Analysis_DPT(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo)
{
    UBYTE r_buf[SECTOR_MBR_SIZE];
    SFAT_RES res = RUN_OK;
    struct MBR_Typedef * ptr;
    UBYTE flag[3] = DBR_MARK;
    int i;
    res = Read_dev_port(dev,r_buf,SECTOR_MBR_ADDR,1);
    if(res > 0){
        ptr = (struct MBR_Typedef *)r_buf;
        if(ptr->magic_num[0] == 0x55 && ptr->magic_num[1] == 0xAA){
            if(r_buf[0] == flag[0] && r_buf[1] == flag[1] && r_buf[2] == flag[2]){ // if no MBR
                sysInfo->part_start_sector_addr = SECTOR_MBR_ADDR;
                return res;
            }else{
                for (i = 0; i < DPT_ITEM_NUM; i++){
                    if(ptr->dpt[i].id == FILE_SYSTEM_TYPE_NONE){
                        continue;
                    }else if(ptr->dpt[i].id == FILE_SYSTEM_TYPE_FAT32){
                        sysInfo->sys_id = FILE_SYSTEM_TYPE_FAT32;
                        break;
                    }else if(ptr->dpt[i].id == FILE_SYSTEM_TYPE_FAT32_C){
                        sysInfo->sys_id = FILE_SYSTEM_TYPE_FAT32_C;
                        break;
                    }else{
                        res = UNKNOWN_FS;
                        return res;
                    }
                }
                if(i == DPT_ITEM_NUM){
                    res = NO_FAT_SYS;
                    return res;                
                }
                sysInfo->part_start_sector_addr = SHIFT_LITTLE_ENDIAN(ptr->dpt[i].part_start_sector_addr,4);
                sysInfo->part_total_sectors = SHIFT_LITTLE_ENDIAN(ptr->dpt[i].part_total_sectors,4);
            }
        }else{
            res = NOT_MBR;           
        }
    }else{
        res = READ_DISK_ERROR;
    }
    return res;
}


/**
 * @description: 解析BPB字段
 * @param {MEDIUM_TYPES_T } dev
 * @param {sfat_system_param *} sysInfo
 * @return {*}
 */
static SFAT_RES SFAT_Analysis_BPB(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo)
{
    UBYTE r_buf[SECTOR_DBR_SIZE];
    SFAT_RES res = RUN_OK;
    struct DBR_Typedef * ptr;
    int i;
    res = Read_dev_port(dev,r_buf,sysInfo->part_start_sector_addr,1);
    if(res > 0){
        ptr = (struct DBR_Typedef *)r_buf;
        if(ptr->magic_num[0] == 0x55 && ptr->magic_num[1] == 0xAA){
            sysInfo->bytes_of_sector = (USHORT)SHIFT_LITTLE_ENDIAN(ptr->bpb.sector_bytes,2);
            sysInfo->sector_total_num = SHIFT_LITTLE_ENDIAN(ptr->bpb.sector_total_num,4);
            if(sysInfo->part_start_sector_addr != SECTOR_MBR_ADDR && 
               sysInfo->sector_total_num != sysInfo->part_total_sectors){
                res = PART_SECTOR_NUM_UNEQU;
                return res;
            }
            sysInfo->sector_num_of_cluster = ptr->bpb.sector_num_of_cluster;
            sysInfo->sector_num_of_rsvd = (USHORT)SHIFT_LITTLE_ENDIAN(ptr->bpb.sector_num_of_rsvd,2);
            sysInfo->fat_num = ptr->bpb.fat_num;
            sysInfo->sector_num_of_fat = SHIFT_LITTLE_ENDIAN(ptr->bpb.sector_num_of_fat,4);
            sysInfo->fat1_start_sector_addr = sysInfo->part_start_sector_addr 
                                            + sysInfo->sector_num_of_rsvd;
            sysInfo->firstdir_start_sector_addr = sysInfo->fat1_start_sector_addr 
                                                + sysInfo->sector_num_of_fat * sysInfo->fat_num;
        }else{
            res = NOT_DBR;           
        }
    }else{
        res = READ_DISK_ERROR;
    }    
    return res;
}

/**
 * @description: SFAT解析文件目录项
 * @param {PDI_Typedef *} pdi
 * @param {file_Info *} info
 * @return {*}
 */
static SFAT_RES SFAT_Analysis_PDI(struct PDI_Typedef * pdi,struct file_Info * info)
{   
    UINT _date;
    UINT _time;
    SFAT_RES res = RUN_OK;
    if(pdi == NULL || info == NULL) return PARAM_ERROR;
    // 解析文件名
    for (int i = 0; i < FILE_NAME_BLEN; i++)
    {
        if(i < HOST_FILE_NAME_BLEN){
            info->filename[i] = pdi->file_name[i];
        }else if(i > HOST_FILE_NAME_BLEN && i < FILE_NAME_BLEN - 1){
            info->filename[i] = pdi->ext_file_name[i - HOST_FILE_NAME_BLEN - 1];
        }else{
            info->filename[i] = '.';
        }        
    }
    info->filename[FILE_NAME_BLEN - 1] = '\0';
    // 解析文件位置大小
    info->file_start_cluster = SHIFT_LITTLE_ENDIAN(pdi->start_cluster_low16,2) | 
                               (SHIFT_LITTLE_ENDIAN(pdi->start_cluster_high16,2) << 16);
    info->file_size = SHIFT_LITTLE_ENDIAN(pdi->file_data_len,FILE_DATA_BLEN);                           
    // 解析文件属性
    info->file_attr = pdi->file_attribute;
    // 解析创建文件日期时间
    _date = SHIFT_LITTLE_ENDIAN(pdi->create_file_date,CDATE_BLEN);
    info->ctim.year = (USHORT)(((_date & FILE_DATE_YEAR_MASK) >> FILE_DATE_YEAR_TB) + DATE_BASE_YEAR);
    info->ctim.month = (UBYTE)((_date & FILE_DATE_MON_MASK) >> FILE_DATE_MON_TB);
    info->ctim.day = (UBYTE)(_date & FILE_DATE_DAY_MASK);
    _time = SHIFT_LITTLE_ENDIAN(pdi->create_file_time,CTIME_BLEN);
    info->ctim.hour = (UBYTE)((_time & FILE_TIME_HOUR_MASK) >> FILE_TIME_HOUR_TB);
    info->ctim.min = (UBYTE)((_time & FILE_TIME_MIN_MASK) >> FILE_TIME_MIN_TB);
    info->ctim.sec = (UBYTE)((_time & FILE_TIME_SEC_MASK) * 2) + (UBYTE)(pdi->tim_10ms * 0.01f);

    // 解析修改文件日期时间
    _date = SHIFT_LITTLE_ENDIAN(pdi->modify_file_date,MDATE_BLEN);
    info->mtim.year = (USHORT)(((_date & FILE_DATE_YEAR_MASK) >> FILE_DATE_YEAR_TB) + DATE_BASE_YEAR);
    info->mtim.month = (UBYTE)((_date & FILE_DATE_MON_MASK) >> FILE_DATE_MON_TB);
    info->mtim.day = (UBYTE)(_date & FILE_DATE_DAY_MASK);
    _time = SHIFT_LITTLE_ENDIAN(pdi->modify_file_time,MTIME_BLEN);
    info->mtim.hour = (UBYTE)((_time & FILE_TIME_HOUR_MASK) >> FILE_TIME_HOUR_TB);
    info->mtim.min = (UBYTE)((_time & FILE_TIME_MIN_MASK) >> FILE_TIME_MIN_TB);
    info->mtim.sec = (UBYTE)((_time & FILE_TIME_SEC_MASK) * 2);
    // 解析访问文件日期
    _date = SHIFT_LITTLE_ENDIAN(pdi->lasted_access_file_date,ADATE_BLEN);
    info->atim.year = (USHORT)(((_date & FILE_DATE_YEAR_MASK) >> FILE_DATE_YEAR_TB) + DATE_BASE_YEAR);
    info->atim.month = (UBYTE)((_date & FILE_DATE_MON_MASK) >> FILE_DATE_MON_TB);
    info->atim.day = (UBYTE)(_date & FILE_DATE_DAY_MASK);
    return res;
}
























