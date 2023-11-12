#include "sfat.h"

/**
 * win api
*/
#include <windows.h>
#include <stdio.h>


SFAT_RES Read_dev_port(
    MEDIUM_TYPES_T pdev,
    UBYTE * buff,
    ULONG sector,
    UINT  cnt
)
{
    HANDLE device = NULL;
    DWORD bytesRead;
    device = CreateFile("\\\\.\\PhysicalDrive1", 
                        GENERIC_READ, 
                        FILE_SHARE_READ | FILE_SHARE_WRITE, 
                        NULL, 
                        OPEN_EXISTING, 
                        0, 
                        NULL);
    if (device == INVALID_HANDLE_VALUE) {
        printf("Failed to open device.\n");
        return -5;
    }
    SetFilePointer(device, 512 * sector, NULL, FILE_BEGIN); 
    if (!ReadFile(device, buff, 512, &bytesRead, NULL)) {
        printf("Failed to read from device.\n");
        CloseHandle(device);
        return -5;
    }
    CloseHandle(device);
    return 1;
}



SFAT_RES Write_dev_port(
    MEDIUM_TYPES_T pdev,
    const UBYTE * buff,
    ULONG sector,
    UINT  cnt
){
    return 1;
}



SFAT_RES Ctrl_dev_port(
    MEDIUM_TYPES_T pdev,
    UINT cmd,
    UBYTE * buff
){
    return 1;
}
























