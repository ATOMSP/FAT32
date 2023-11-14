#include "sfat.h"
#include "sfat_macro.h"

/**
 * 设备系统参数(目前支持一个设备)
*/
static struct sfat_system_param sysInfo;

/**
 * static function
*/
static SFAT_RES SFAT_Analysis_DPT(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo);
static SFAT_RES SFAT_Analysis_BPB(MEDIUM_TYPES_T dev,struct sfat_system_param * sysInfo);
static SFAT_RES SFAT_Analysis_PDI(struct PDI_Typedef * pdi,struct file_Info * info);
static void SFAT_ShowAllInfo(struct sfat_system_param * sysInfo);
static SFAT_RES SearchFile(const char * host,const char * ext,const char* filename);


/**
 * @description: SFAT获取FAT32文件系统参数（DPI / BPB ）
 * @param {MEDIUM_TYPES_T } dev
 * @param {sfat_system_param *} sysInfo
 * @return {*}
 */
SFAT_RES SFAT_ReadSysInfo(MEDIUM_TYPES_T dev)
{
    SFAT_RES res = RUN_OK;
    res = SFAT_Analysis_DPT(dev,&sysInfo);
    if(res > 0){
       res = SFAT_Analysis_BPB(dev,&sysInfo);
       if(res > 0) SFAT_ShowAllInfo(&sysInfo);
       sysInfo.dev_owner = dev;
       return res;
    }
    return res;
}

/**
 * @description: SFAT查找对应文件名的文件，返回其文件信息
 * @param {const char* } filename
 * @param {file_Info *} fi
 * @return {*}
 */
SFAT_RES SFAT_FindFile(const char* filename,struct file_Info * fi)
{
    if(filename == NULL || fi == NULL) return PARAM_ERROR;
    SFAT_RES res = RUN_OK;
    struct PDOS_Typedef * pdos;
    UBYTE r_buf[SECTOR_SIZE];
    UINT cur_cluster = FISRT_DIR_CLUSTER;
    /* 获取对应簇首的扇区地址 */ 
    UINT sec = CLUSTER_TO_SECTOR(&sysInfo,cur_cluster);
    /* 遍历一个首目录簇 */
    for (int i = 0; i < sysInfo.sector_num_of_cluster; i++)
    {
        /* 读取扇区数据 */
        res = Read_dev_port(sysInfo.dev_owner,r_buf,sec + i,1);
        if(res > 0){
            pdos = (struct PDOS_Typedef *)r_buf;
            /* 遍历每个扇区的文件目录项 */
            for (int j = 0; j < PDI_NUM_OF_SECTOR; j++)
            {
                res = SearchFile(pdos->pdi[j].file_name,pdos->pdi[j].ext_file_name,filename);
                if(res > 0)
                    /* 写入文件目录项 */
                    return SFAT_Analysis_PDI(&(pdos->pdi[j]),fi);
            }
        }else{
            return res;
        }
    }
    return FILE_NO_FIND;
}
/**
 * @description: 匹配文件名(8.3\支持大写)
 * @param {const char  *} host 
 * @param {const char  *} ext
 * @param {const char  *} filename 
 * @return {*}
 */
static SFAT_RES SearchFile(const char * host,const char * ext,const char* filename)
{
    char name[11];
    UBYTE i = 0,j = 0;
    if(host == NULL || ext == NULL || filename == NULL){
        return PARAM_ERROR;
    }
    for (int i = 0; i < 11; i++)
        name[i] = ' ';
    while (*(filename + j) != '\0'){
        if(filename[j] == '.'){
            i = 8;
        }else{
            name[i] = filename[j];
            i += 1;
        }
        j += 1;
    }
    if(j >= FILE_NAME_BLEN ) return ERROR_FILE_NAME;
    for (int m = 0; m < 3; m++){
        if(ext[m] != name[8 + m]){
            return FILENAME_UNEQU;
        }
    }
    for (int n = 0; n < 8; n++){
        if(host[n] != name[n]){
            return FILENAME_UNEQU;
        }
    }
    return RUN_OK;
} 

/**
 * @description: 打印文件目录项信息
 * @param {file_Info *} fi
 * @return {*}
 */
void SFAT_ShowFDIInfo(struct file_Info * fi)
{
    SFAT_log("\r\n======= SFAT File InfoTable ========\r\n");
    SFAT_log("-> FileName------------[%s] \r\n",fi->filename);
    SFAT_log("-> FileAttribute-------[0x%x] \r\n",fi->file_attr);
    SFAT_log("-> FileCreateDate------[%d-%d-%d-%d-%d-%d ] \r\n",
    fi->ctim.year,fi->ctim.month,fi->ctim.day,fi->ctim.hour,fi->ctim.min,fi->ctim.sec);
    SFAT_log("-> FileModifyDate------[%d-%d-%d-%d-%d-%d ] \r\n",
    fi->mtim.year,fi->mtim.month,fi->mtim.day,fi->mtim.hour,fi->mtim.min,fi->mtim.sec);
    SFAT_log("-> FileAccessDate------[%d-%d-%d ] \r\n",
    fi->atim.year,fi->atim.month,fi->atim.day);
    SFAT_log("-> FileStartCluster----[%d] \r\n",fi->file_start_cluster);
    SFAT_log("-> FileSize------------[%d] \r\n",fi->file_size);
    SFAT_log("======= End File InfoTable  ========\r\n");
}

/**
 * @description: 打印系统参数
 * @param {sfat_system_param *} sysInfo
 * @return {*}
 */
static void SFAT_ShowAllInfo(struct sfat_system_param * sysInfo)
{
    SFAT_log("\r\n======= SFAT System InfoTable ========\r\n");
    SFAT_log("-> FileSystemType------[0x%x] \r\n",sysInfo->sys_id);
    SFAT_log("-> PartSartSectorAddr--[0x%x] \r\n",sysInfo->part_start_sector_addr);
    SFAT_log("-> DiskSize------------[%d MB] \r\n",sysInfo->disk_size);
    SFAT_log("-> SectorBytes---------[%d B] \r\n",sysInfo->bytes_of_sector);
    SFAT_log("-> SectorsOfCluster----[%d] \r\n",sysInfo->sector_num_of_cluster);
    SFAT_log("-> FatTableNum---------[%d] \r\n",sysInfo->fat_num);
    SFAT_log("-> SectorsOfFatTable---[%d] \r\n",sysInfo->sector_num_of_fat);
    SFAT_log("-> FatTableNum---------[%d] \r\n",sysInfo->fat_num);
    SFAT_log("-> Fat1StartSectorAddr-[0x%x] \r\n",sysInfo->fat1_start_sector_addr);
    SFAT_log("-> FirstDirSectorAddr--[0x%x] \r\n",sysInfo->firstdir_start_sector_addr);
    SFAT_log("======= End System InfoTable  ========\r\n");
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
                /* find Fat32 file system */
                for (i = 0; i < DPT_ITEM_NUM; i++){
                    if(ptr->dpt[i].id == FILE_SYSTEM_TYPE_NONE){
                        continue;
                    }else if(ptr->dpt[i].id == FILE_SYSTEM_TYPE_FAT32_2M){
                        sysInfo->sys_id = FILE_SYSTEM_TYPE_FAT32_2M;
                        break;
                    }else if(ptr->dpt[i].id == FILE_SYSTEM_TYPE_FAT32_2G){
                        sysInfo->sys_id = FILE_SYSTEM_TYPE_FAT32_2G;
                        break;
                    }else if(ptr->dpt[i].id == FILE_SYSTEM_TYPE_FAT32_2T){
                        sysInfo->sys_id = FILE_SYSTEM_TYPE_FAT32_2T;
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
            if(sysInfo->bytes_of_sector != SECTOR_SIZE){
                res = SECTOR_SIZE_NOT_512;
                return res;                
            }
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
            sysInfo->disk_size = (UINT)((float)sysInfo->sector_total_num * (float)((float)sysInfo->bytes_of_sector / 1048576.0f ));
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
























