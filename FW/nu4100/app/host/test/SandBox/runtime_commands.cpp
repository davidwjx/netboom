#include <time.h>
#include "SandBoxFWLib/SandBoxLib.h"
#include "Sandbox.h"
#include <string.h>
#include <assert.h>
#include "utils.h"
#include "inttypes.h"

#if (defined _WIN32) || (defined _WIN64)
#include <thread>
#else
#include <unistd.h>
#include <pthread.h>
#endif

#define SPACE_DELIMITER " "
#define COMMA_DELIMITER ","

#if (defined _WIN32) || (defined _WIN64)
typedef HANDLE THREAD_HANDLE;
#else
typedef void* THREAD_HANDLE;
#endif

static ERRG_codeE DumpDebugRegisters(inu_deviceH deviceH, const char *debugFilePath, const char *debugOutputFilePath)
{
    ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
    FILE* fp;
    FILE* fp_out;
    char lineBuf[1024];
    char* regName;
    char* regAddrStr;
    char* regDescStr;
    UINT32 regCount;
    INT32 bufLen;
    FWLib_accessRegisterT accessReg;
    UINT32 regStart;
    UINT32 regEnd;

    if (debugFilePath == NULL || debugOutputFilePath == NULL)
    {
        return ret;
    }

    fp = fopen(debugFilePath, "r");
    if (fp == NULL)
    {
        printf("Failed to open file at %s\n", debugFilePath);
        return ret;
    }

    fp_out = fopen(debugOutputFilePath, "a+");
    if (fp_out == NULL)
    {
        printf("Failed to open output file: %s\n", debugOutputFilePath);
        fclose(fp);
        return ret;
    }

    fprintf(fp_out, "=============Debug Registers Dump================\n");

    while (!feof(fp))
    {
        memset(lineBuf, 0, 1024);
        fgets(lineBuf, 1024, fp);
        bufLen = strlen(lineBuf);
        if (strncmp(lineBuf, "BLOCK:", strlen("BLOCK:")) == 0)
        {
            fprintf(fp_out, "\n%s", lineBuf);
            if (strstr(lineBuf, "SENSOR") != NULL)
            {
                accessReg.regType = FW_LIB__REG_I2C_E;
            }
            else if (strstr(lineBuf, "DPHY") != NULL) {
                accessReg.regType = FW_LIB__REG_DPHY_E;
            }
            else
            {
                accessReg.regType = FW_LIB__REG_SOC_E;
            }

            printf("regType: %d\n", accessReg.regType);
        }
        else
        {
            if (lineBuf[0] == '[' && (strchr(lineBuf, ']') != NULL))
            {
                regName = strtok(lineBuf + 1, COMMA_DELIMITER);
                if (accessReg.regType == FW_LIB__REG_SOC_E)
                {
                    //[RegName,Address,Count,Description]
                    regAddrStr = strtok(NULL, COMMA_DELIMITER);
                    accessReg.socReg.addr = strtol(regAddrStr, NULL, 0);
                    regCount = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    regDescStr = strtok(NULL, COMMA_DELIMITER);
                    *strchr(regDescStr, ']') = '\0';
                    fprintf(fp_out, "[%s - 0x%08X]: %s\n", regName, accessReg.socReg.addr, regDescStr);
                    for (int i = 0; i < regCount; i++) {
                        ret = FwLib_read_register(deviceH, &accessReg);
                        fprintf(fp_out, "\t[0x%08X]:0x%08X\n",accessReg.socReg.addr , accessReg.socReg.val);
                        accessReg.socReg.addr += 0x4;
                    }
                }

                if (accessReg.regType == FW_LIB__REG_DPHY_E) {
                    //[RegName, DphyNum,Address,Count,Description]
                    accessReg.dphyReg.dphyNum = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    accessReg.dphyReg.addr = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    regCount = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    regDescStr = strtok(NULL, COMMA_DELIMITER);
                    *strchr(regDescStr, ']') = '\0';
                    fprintf(fp_out, "[%s - DPHY%d:0x%08X]: %s\n", regName, accessReg.dphyReg.dphyNum, accessReg.dphyReg.addr, regDescStr);
                    for (int i = 0; i < regCount; i++) {
                        ret = FwLib_read_register(deviceH, &accessReg);
                        fprintf(fp_out, "\t[0x%08X]:0x%08X\n", accessReg.dphyReg.addr, accessReg.dphyReg.val);
                        accessReg.dphyReg.addr++;
                    }
                }

                if (accessReg.regType == FW_LIB__REG_I2C_E)
                {
                    //[RegName,I2CBusNum,I2CDeviceAddr,AccessSize,RegStartAddr,RegEndAddr,Description]
                    accessReg.i2cReg.i2cNum = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    accessReg.i2cReg.tarAddress = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    accessReg.i2cReg.accessSize = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    regStart = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    regEnd = strtol(strtok(NULL, COMMA_DELIMITER), NULL, 0);
                    accessReg.i2cReg.regWidth = 1;

                    printf("I2C: %d - 0x%X - %d - 0x%X:0x%X\n"
                        , accessReg.i2cReg.i2cNum, accessReg.i2cReg.tarAddress, accessReg.i2cReg.accessSize
                        , regStart, regEnd);
                    regDescStr = strtok(NULL, COMMA_DELIMITER);
                    *strchr(regDescStr, ']') = '\0';
                    fprintf(fp_out, "[%s - 0x%08X]: %s\n", regName, regStart, regDescStr);
                    while (regStart <= regEnd)
                    {
                        accessReg.i2cReg.regAddress = regStart;
                        ret = FwLib_read_register(deviceH, &accessReg);
                        fprintf(fp_out, "\t[0x%08X]:%02X\n", accessReg.i2cReg.regAddress, accessReg.i2cReg.i2cData);
                        regStart++;
                    }
                }
            }
        }
    }

    fclose(fp);
    fclose(fp_out);
    return ret;
}

THREAD_HANDLE thrdH;

static ERRG_codeE focusAr2020Sensor(inu_deviceH deviceH,UINT32 hOff, UINT32 vOff)
{
    ERRG_codeE ret;
    FWLib_accessRegisterT accessReg;
    UINT32 i2cNum;
    INT32 x_len, y_len, x, y;
    INT32 x_b, y_b, x_e, y_e, x_t, y_t;

    accessReg.regType = FW_LIB__REG_I2C_E;
    accessReg.i2cReg.i2cNum = 1;
    accessReg.i2cReg.tarAddress = 0x6c;
    accessReg.i2cReg.accessSize = 2;
    accessReg.i2cReg.regWidth = 2;
    accessReg.i2cReg.regAddress = 0x0016;

    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        if(accessReg.i2cReg.i2cData == 0x0653)
            i2cNum = 1;
        else
            i2cNum = 2;
    }

    accessReg.i2cReg.i2cNum = i2cNum;

    accessReg.i2cReg.regAddress = 0x0344;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        x_b = accessReg.i2cReg.i2cData;
    }
    accessReg.i2cReg.regAddress = 0x0348;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        x_e = accessReg.i2cReg.i2cData;
    }
    x_t = x_e - x_b;

    accessReg.i2cReg.regAddress = 0x0346;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        y_b = accessReg.i2cReg.i2cData;
    }
    accessReg.i2cReg.regAddress = 0x034a;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        y_e = accessReg.i2cReg.i2cData;
    }
    y_t = y_e - y_b;

    accessReg.i2cReg.regAddress = 0x3c6c;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        x_len = accessReg.i2cReg.i2cData;
    }
    accessReg.i2cReg.regAddress = 0x3c72;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        y_len = accessReg.i2cReg.i2cData;
    }

    accessReg.i2cReg.regAddress = 0x3c6a;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        x = accessReg.i2cReg.i2cData;
    }
    accessReg.i2cReg.regAddress = 0x3c70;
    ret = FwLib_read_register(deviceH, &accessReg);
    if (ERRG_SUCCEEDED(ret))
    {
        y = accessReg.i2cReg.i2cData;
    }
    //printf("configure info: xlen :%d - ylen: %d xtotal : %d - ytotal: %d\n", x_len,y_len,x_t,y_t);
    //printf("old focus x:%d - y: %d", x,y);
    x = x + hOff;
    y = y + vOff;
    if(x <= 0)
        x = 0;
    if(y <= 0)
        y = 0;

    if(x + x_len >= x_t)
        x = x_t - x_len;
    if(y + y_len >= y_t)
        y = y_t - y_len;

    //printf(" ===> new focus x:%d - y: %d\n", x,y);
    accessReg.i2cReg.regAddress = 0x3c6a;
    accessReg.i2cReg.i2cData = x;
    ret = FwLib_write_register(deviceH, &accessReg);

    accessReg.i2cReg.regAddress = 0x3c70;
    accessReg.i2cReg.i2cData = y;
    ret = FwLib_write_register(deviceH, &accessReg);

    return ret;
}

static ERRG_codeE dumpI2CRegs(inu_deviceH deviceH, UINT32 i2cNum, UINT32 tarAddress, UINT32 accessSize, UINT32 regStart, UINT32 regEnd)
{
    ERRG_codeE ret;
    FWLib_accessRegisterT accessReg;

    accessReg.regType = FW_LIB__REG_I2C_E;
    accessReg.i2cReg.i2cNum = i2cNum;
    accessReg.i2cReg.tarAddress = tarAddress;
    accessReg.i2cReg.accessSize = accessSize;
    accessReg.i2cReg.regWidth = 1;

    while (regStart <= regEnd) {
        accessReg.i2cReg.regAddress = regStart;
        ret = FwLib_read_register(deviceH, &accessReg);
        if (ERRG_SUCCEEDED(ret))
        {
            printf("[0x%08x]: 0x%x\n",
                accessReg.i2cReg.regAddress,
                accessReg.i2cReg.i2cData);
            regStart++;
        }
        else
        {
            printf("Failed to access I2C register: %d 0x%x, %d, 0x%x, %d = 0x%x\n",
                accessReg.i2cReg.i2cNum, accessReg.i2cReg.tarAddress, accessReg.i2cReg.accessSize, accessReg.i2cReg.regAddress,
                accessReg.i2cReg.regWidth, accessReg.i2cReg.i2cData);
            break;
        }
    }

    return ret;
}

static ERRG_codeE dumpGzhtEeprom(inu_deviceH deviceH, UINT32 i2cNum, UINT32 tarAddress, UINT8 gzHt, UINT32 regStart, UINT32 regEnd)
{
    ERRG_codeE ret = (ERRG_codeE)ERR_INVALID_ARGS;
    FWLib_accessRegisterT accessReg;
    UINT8 Config[5] = {0x01,0x00, 0x00, 0x02, 0x01}; //STOP_STREAM, 0x00, 0x00, NVM_READ, START_STREAM
    int loop;
    UINT16 regAddr, regOffset;

    if(gzHt == 0) {
        regAddr = 0x78; // regAddr of GAZE
        regOffset = 0x06b8;
    }else if(gzHt == 1) {
        regAddr = 0x1cc; // regAddr of Track
        regOffset = 0x080c;
    }else {
        printf("Invalid argument!\n");
        return ret;
    }
    accessReg.regType = FW_LIB__REG_I2C_E;
    accessReg.i2cReg.i2cNum = i2cNum;
    accessReg.i2cReg.tarAddress = tarAddress;
    accessReg.i2cReg.accessSize = 2;
    accessReg.i2cReg.regWidth = 1;

    accessReg.i2cReg.regAddress = 0x0202;
    accessReg.i2cReg.i2cData  = Config[0];
    ret = FwLib_write_register(deviceH, &accessReg);

    // Set NVM_BURST_MODE
    accessReg.i2cReg.regAddress = 0x0228;
    accessReg.i2cReg.i2cData  = Config[1];
    ret = FwLib_write_register(deviceH, &accessReg);

    // Set NVM_ECC_ON
    accessReg.i2cReg.regAddress = 0x0228;
    accessReg.i2cReg.i2cData  = Config[2];
    ret = FwLib_write_register(deviceH, &accessReg);

    // Set NVM_NB_OF_WORDS
    accessReg.i2cReg.regAddress = 0x0229;
    if(gzHt == 0)
        accessReg.i2cReg.i2cData  = 124 / 4;
    else
        accessReg.i2cReg.i2cData  = 44 / 4;
    ret = FwLib_write_register(deviceH, &accessReg);

    // NVM address to write (word 0: address 0x0, word 1: address 0x4), CTM_AREA_0 :
    accessReg.i2cReg.regAddress = 0x022a;
    accessReg.i2cReg.i2cData  = regAddr & 0xff;
    ret = FwLib_write_register(deviceH, &accessReg);

    accessReg.i2cReg.regAddress = 0x022b;
    accessReg.i2cReg.i2cData  = regAddr >> 8;
    ret = FwLib_write_register(deviceH, &accessReg);

    // Send NVM_READ command
    accessReg.i2cReg.regAddress = 0x0201;
    accessReg.i2cReg.i2cData  = Config[3];
    ret = FwLib_write_register(deviceH, &accessReg);

    for(loop = regStart; loop < regEnd; loop++) {
        accessReg.i2cReg.regAddress = regOffset + loop;
        ret = FwLib_read_register(deviceH, &accessReg);
        if (ERRG_SUCCEEDED(ret))
        {
            printf("[0x%08x]: 0x%x\n",
                   loop,
                   accessReg.i2cReg.i2cData);
        }
        else
        {
            printf("Failed to access gz|ht eerpom : %d 0x%x, %d, 0x%x, %d = 0x%x\n",
                accessReg.i2cReg.i2cNum, accessReg.i2cReg.tarAddress, 1, accessReg.i2cReg.regAddress,
                accessReg.i2cReg.regWidth, accessReg.i2cReg.i2cData);
            break;
        }
    }

    // Send START_STREAM command
    accessReg.i2cReg.regAddress = 0x0201;
    accessReg.i2cReg.i2cData  = Config[4];
    ret = FwLib_write_register(deviceH, &accessReg);

    return ret;
}

static void exeRuntimeCmdPrintHelp()
{
      printf("Usage:\n\n");
      printf("Read  register: r address\n");
      printf("                read address\n");
      printf("Write register: w address value\n");
      printf("                write address value\n");
      printf("Read  i2c register: R i2cNum deviceAddress accessSize registerAddress regWidth\n");
      printf("                    read_i2c i2cNum deviceAddress accessSize registerAddress regWidth\n");
      printf("Write i2c register: W i2cNum deviceAddress accessSize registerAddress regWidth i2cData\n");
      printf("                    write_i2c i2cNum deviceAddress accessSize registerAddress regWidth i2cData\n");
      printf("Read  Attiny register: a address\n");
      printf("                       read_att address\n");
      printf("Write Attiny register: b address value\n");
      printf("                       write_att address value\n");
      printf("Modify sensorGroup exposure by sensorGroup id: e sensorGroupId exposureTimeUs analogGain\n");
      printf("                                               exposure sensorGroupId exposureTimeUs analogGain\n");
      printf("Reset frame count: s streamerName\n");
      printf("                   reset_frame_count streamerName\n");
      printf("Set user parameters: u streamerName userParam1 userParam2\n");
      printf("                     set_user_params streamerName userParam1 userParam2\n");
      printf("SW reset: k\n");
      printf("          sw_reset\n");
      printf("Set network id: n cdnn_func_name network_id <configuration_name>\n");
      printf("                set_network_id cdnn_func_name network_id <configuration_name>\n");
      printf("                (configuration_name is optional. Default value is \"main\")\n");
      printf("Reload Calibration LUT files: l calib_path\n");
      printf("                              lut_reload calib_path\n");
      printf("Change sensorGroup's frame rate: f sensorGroupId frameRate\n");
      printf("                                 fps sensorGroupId frameRate\n");
      printf("Dump I2C registers: d i2cnum deviceAddress accessSize registerStartAddr registerEndAddr\n");
      printf("                    dump_i2c i2cnum deviceAddress accessSize registerStartAddr registerEndAddr\n");
      printf("Dump gz&ht eeprom registers: E i2cnum deviceAddress gzOrht(gz:0, ht:1) registerStartAddr registerEndAddr\n");
      printf("                    dump_gh i2cnum deviceAddress gzOrht(gz:0, ht:1) registerStartAddr registerEndAddr\n");
      printf("Focus AR2020 sensor: x Hoffset Voffset\n");
      printf("                    focux_ar2020 Hoffset Voffset\n");
      printf("Read sensor temperature: t sensorId\n");
      printf("                         temperature sensorId(Caution: not sensor group ID)\n");
      printf("Get temperature infor: T temperatureId\n");
      printf("                         get_temp temperatureId\n");
      printf("Dump debug registers: D debugCfgFile debugOutputFile\n");
      printf("                      debug_registers debugCfgFile debugOutputFile\n");
      printf("Modify focus by sensor id: f sensorGroupId mode value\n");
      printf("                           focus sensorGroupId mode value\n");
      printf("                           for mode > 3, do only focus params reading\n");
      printf("Modify crop position:      crop | c sensor_id startX startY  \n");
      printf("Write a file into target file system: load_file | lf  host_file_path target_file_name\n");
      printf("Read a file from target file system: read_file | rf  host_file_name target_file_name\n");
      printf("Send ISP Commands:\n");
      printf("                 Set Exposure: isp_set_exp | i0  channel integration_time sensor_gain isp_gain\n");
      printf("                 Get Exposure: isp_get_exp | i1  channel \n");
      printf("                 Set WB      : isp_set_wb  | i2  channel  wbGain.Blue wbGain.GreenB wbGain.GreenR wbGain.Red ccOffset.Blue ccOffset.Green ccOffset.Red ccMatrix.coeff[0] .. ccMatrix.coeff[8]\n");
      printf("                 Get WB      : isp_get_wb  | i3  channel \n");
      printf("                 Set LSC     : isp_set_lsc | i4  channel set_by_file <0/1> file_name \n");
      printf("                 Get LSC     : isp_get_lsc | i5  channel set_by_file <0/1>\n");
      printf("Record Enable: RE number_of_frames[0 to disable] streamName1 streamName2 ...\n");
      printf("               record_enable number_of_frames[0 to disable] streamName1 streamName2 ...\n");
}

static ERRG_codeE exeRuntimeCmdReadRegister(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   FWLib_accessRegisterT accessReg = {};
   char * cmd_param;

  // memset((void*)&accessReg, 0, sizeof(accessReg));

   accessReg.regType = FW_LIB__REG_SOC_E;
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.socReg.addr = (int)strtol(cmd_param, NULL, 0);
   ret = FwLib_read_register(deviceH, &accessReg);
   if (ERRG_SUCCEEDED(ret))
   {
      printf("read register: 0x%x = 0x%x\n", accessReg.socReg.addr, accessReg.socReg.val);
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdWriteRegister(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   FWLib_accessRegisterT accessReg = {};
   char* cmd_param;

   accessReg.regType = FW_LIB__REG_SOC_E;
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.socReg.addr = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.socReg.val = (int)strtol(cmd_param, NULL, 0);
   printf("write register: 0x%x = 0x%x\n", accessReg.socReg.addr, accessReg.socReg.val);
   ret = FwLib_write_register(deviceH, &accessReg);

   return ret;
}

static ERRG_codeE exeRuntimeCmdReadI2cRegister(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   FWLib_accessRegisterT accessReg = {};
   char* cmd_param;

   accessReg.regType = FW_LIB__REG_I2C_E;
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.i2cNum = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.tarAddress = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.accessSize = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.regAddress = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.regWidth = (int)strtol(cmd_param, NULL, 0);
   ret = FwLib_read_register(deviceH, &accessReg);
   if (ERRG_SUCCEEDED(ret))
   {
      printf("read i2c register: %d 0x%x, %d, 0x%x, %d = 0x%x\n",
         accessReg.i2cReg.i2cNum, accessReg.i2cReg.tarAddress, accessReg.i2cReg.accessSize, accessReg.i2cReg.regAddress,
         accessReg.i2cReg.regWidth, accessReg.i2cReg.i2cData);
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdWriteI2cRegister(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   FWLib_accessRegisterT accessReg = {};
   char* cmd_param;

   accessReg.regType = FW_LIB__REG_I2C_E;
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.i2cNum = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.tarAddress = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.accessSize = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.regAddress = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.regWidth = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.i2cReg.i2cData = (int)strtol(cmd_param, NULL, 0);
   ret = FwLib_write_register(deviceH, &accessReg);
   printf("write i2c register: %d 0x%x, %d, 0x%x, %d = 0x%x\n",
      accessReg.i2cReg.i2cNum, accessReg.i2cReg.tarAddress, accessReg.i2cReg.accessSize, accessReg.i2cReg.regAddress,
      accessReg.i2cReg.regWidth, accessReg.i2cReg.i2cData);

   return ret;
}

static ERRG_codeE exeRuntimeCmdReadAttRegister(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   FWLib_accessRegisterT accessReg = {};
   char* cmd_param;

   accessReg.regType = FW_LIB__REG_ATT_E;
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.attReg.addr = (UINT8)strtol(cmd_param, NULL, 0);
   ret = FwLib_read_register(deviceH, &accessReg);
   if (ERRG_SUCCEEDED(ret))
   {
      printf("read Attiny register: 0x%x = 0x%x\n", accessReg.attReg.addr, accessReg.attReg.val);
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdWriteAttRegister(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   FWLib_accessRegisterT accessReg = {};
   char* cmd_param;

   accessReg.regType = FW_LIB__REG_ATT_E;
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.attReg.addr = (UINT8)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   accessReg.attReg.val = (UINT8)strtol(cmd_param, NULL, 0);
   printf("write Attiny register: 0x%x = 0x%x\n", accessReg.attReg.addr, accessReg.attReg.val);
   ret = FwLib_write_register(deviceH, &accessReg);

   return ret;
}

static ERRG_codeE exeRuntimeCmdSetExposure(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   char* cmd_param;

   UINT32 exposureUs, newExposureUs, sensorGroupId;
   float digGain, anaGain, newAnaGain;
   uint32_t fps = 0;

   cmd_param = strtok(NULL, SPACE_DELIMITER);
   sensorGroupId = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   newExposureUs = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   newAnaGain = strtof(cmd_param, NULL);
   ret = FwLib_get_sensorGroup_params(deviceH, sensorGroupId, &exposureUs, &anaGain, &digGain,&fps);
   if (ERRG_SUCCEEDED(ret) && (newExposureUs != 0) && (newAnaGain != 0))
   {
      ret = FwLib_set_sensorGroup_params(deviceH, sensorGroupId, newExposureUs, newAnaGain, digGain,fps);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("Old exposure %d[us], changed to %d[us]\n", exposureUs, newExposureUs);
         printf("Old analog gain %f, changed to %f, (digital gain = %f)\n", anaGain, newAnaGain, digGain);
      }
   }
   else
   {
      printf("Current exposure %d[us], analog gain %f\n", exposureUs, anaGain);
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdResetFrameCount(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   char* cmd_param;
   inu_nodeH streamerH = NULL;

   cmd_param = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, cmd_param, &streamerH);
   if (ERRG_SUCCEEDED(ret)) {
      ret = FwLib_reset_channel_frame_count((inu_refH)streamerH);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("reset channel frame count on %s: Passed\n", cmd_param);
      }
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdSetUserParams(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   char* cmd_param;
   inu_nodeH streamerH = NULL;
   char* channel_name;

   channel_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, channel_name, &streamerH);
   if (ERRG_SUCCEEDED(ret)) {
      UINT32 userParam1, userParam2;
      cmd_param = strtok(NULL, SPACE_DELIMITER);
      userParam1 = (UINT32)strtol(cmd_param, NULL, 0);
      cmd_param = strtok(NULL, SPACE_DELIMITER);
      userParam2 = (UINT32)strtol(cmd_param, NULL, 0);
      ret = FwLib_set_channel_user_info((inu_refH)streamerH, userParam1, userParam2);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("channel user info was set on %s: user1 = %d, user2 = %d\n", channel_name, userParam1, userParam2);
      }
   }

   return ret;
}


static ERRG_codeE exeRuntimeCmdWriteFile(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   inu_nodeH streamerH = NULL;
   char* file_path, * file_name;

   file_path = strtok(NULL, SPACE_DELIMITER);
   file_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__writeFileToGp(deviceH, file_path, file_name);

   return ret;
}

static ERRG_codeE exeRuntimeCmdReadFile(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   inu_nodeH streamerH = NULL;
   char* hostFile, * targetFile;

   hostFile = strtok(NULL, SPACE_DELIMITER);
   targetFile = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__readFileFromGp(deviceH, targetFile, hostFile);

   return ret;
}

static ERRG_codeE exeRuntimeCmdSwREset(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   FwLib_sw_reset(deviceH);
   if (ERRG_SUCCEEDED(ret))
   {
      exit(0);
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdSetNetworkId(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   char* cmd_param;
   void* configH;
   char* cdnn_func_name;
   char* configuration_name;
   UINT32 network_id;

   cdnn_func_name = strtok(NULL, SPACE_DELIMITER);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   network_id = (UINT32)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);

   if (cmd_param == NULL)
   {
      configuration_name = strdup("main");
   }
   else
   {
      configuration_name = strdup(cmd_param);
   }

   ret = findConfigInTable(configuration_name, &configH);

   if (ERRG_SUCCEEDED(ret))
   {
      FwLib_set_network_to_node(deviceH, configH, cdnn_func_name, network_id);
      printf("set network id [%d] on cdnn_function [%s]: Passed\n", network_id, cdnn_func_name);
   }
   else
   {
      printf("Configuration [%s] is not valid. Please choose a valid configuration name\n", configuration_name);
   }

   free(configuration_name);

   return ret;
}

static ERRG_codeE exeRuntimeCmdReloadLutFiles(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;

   cmd_param = strtok(NULL, COMMA_DELIMITER);

   FwLib_reload_lut_files(deviceH, cmd_param);

   return ret;
}

static ERRG_codeE exeRuntimeCmdFocus(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;
   INT32 currentDac, min, max, newDac;
   UINT32 sensorId, chipId, mode;

   cmd_param = strtok(NULL, SPACE_DELIMITER);
   sensorId = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   mode = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   newDac = (int)strtol(cmd_param, NULL, 0);
   ret = FwLib_getSensorFocusParams(deviceH, sensorId, &currentDac, &min, &max, &chipId);
   if (ERRG_SUCCEEDED(ret))
   {
      printf("Current dac %d (min %d, max %d), chip id = 0x%x, change to %d (mode %d)\n", currentDac, min, max, chipId, newDac, mode);
      if (mode < 3) // if mode is illegal, do only get focus params
      {
         ret = FwLib_setSensorFocusParams(deviceH, sensorId, newDac, mode);
         if (ERRG_SUCCEEDED(ret))
         {
            printf(" focus %d (min %d, max %d), chip id = 0x%x, change to %d (mode %d)\n", currentDac, min, max, chipId, newDac, mode);
            ret = FwLib_getSensorFocusParams(deviceH, sensorId, &currentDac, &min, &max, &chipId);
            if (ERRG_SUCCEEDED(ret))
            {
               printf("Changed dac to %d\n", currentDac);
            }
         }
         else
         {
            printf("failed to change\n");
         }
      }
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdIspSetExposure(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;
   inu_nodeH streamerH = NULL;
   char *channel_name;
   float int_time_usec;
   float an_gain;
   float isp_gain;

   channel_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, channel_name, &streamerH);
   if (ERRG_SUCCEEDED(ret))
   {
      cmd_param = strtok(NULL, SPACE_DELIMITER);
      int_time_usec = (float)strtod(cmd_param, NULL);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      an_gain = (float)strtod(cmd_param, NULL);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      isp_gain = (float)strtod(cmd_param, NULL);

      ret = FwLib_ispCommand((inu_refH)streamerH, INU_ISP_COMMAND_SET_EXPOSURE_E, int_time_usec, an_gain, isp_gain);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("ISP channel %s: command = %d, OK\n", channel_name, INU_ISP_COMMAND_SET_EXPOSURE_E);
      }
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdIspGetExposure(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   inu_nodeH streamerH = NULL;
   char* channel_name;

   channel_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, channel_name, &streamerH);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = FwLib_ispCommand((inu_refH)streamerH, INU_ISP_COMMAND_GET_EXPOSURE_E);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("ISP channel %s: command = %d, OK\n", channel_name, INU_ISP_COMMAND_GET_EXPOSURE_E);
      }
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdIspSetWb(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;
   inu_nodeH streamerH = NULL;
   char* channel_name;
   float gain1, gain2, gain3, gain4;
   int off1, off2, off3;
   float matx[9U];
   unsigned int i = 0;

   channel_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, channel_name, &streamerH);
   if (ERRG_SUCCEEDED(ret))
   {

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      gain1 = (float)strtod(cmd_param, NULL);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      gain2 = (float)strtod(cmd_param, NULL);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      gain3 = (float)strtod(cmd_param, NULL);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      gain4 = (float)strtod(cmd_param, NULL);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      off1 = (UINT32)strtol(cmd_param, NULL, 0);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      off2 = (UINT32)strtol(cmd_param, NULL, 0);

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      off3 = (UINT32)strtol(cmd_param, NULL, 0);

      for (i = 0; i < 9; i++)
      {
         cmd_param = strtok(NULL, SPACE_DELIMITER);
         matx[i] = (float)strtod(cmd_param, NULL);
      }

      ret = FwLib_ispCommand((inu_refH)streamerH, INU_ISP_COMMAND_SET_WB_E, gain1, gain2, gain3, gain4, off1, off2, off3, matx[0], matx[1], matx[2], matx[3], matx[4], matx[5], matx[6], matx[7], matx[8]);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("ISP channel %s: command = %d, OK\n", channel_name, INU_ISP_COMMAND_SET_WB_E);
      }
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdIspGetWb(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   inu_nodeH streamerH = NULL;
   char* channel_name;

   channel_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, channel_name, &streamerH);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = FwLib_ispCommand((inu_refH)streamerH, INU_ISP_COMMAND_GET_WB_E);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("ISP channel %s: command = %d, OK\n", channel_name, INU_ISP_COMMAND_GET_WB_E);
      }
   }

   return ret;
}



static ERRG_codeE exeRuntimeCmdIspSetLsc(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;
   inu_nodeH streamerH = NULL;
   char* channel_name;
   unsigned int set_by_file = 0;
   char* file_name_p;
   char file_name[32];

   channel_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, channel_name, &streamerH);
   if (ERRG_SUCCEEDED(ret))
   {
      cmd_param = strtok(NULL, SPACE_DELIMITER);
      set_by_file = (unsigned int)strtod(cmd_param, NULL);
      file_name_p = strtok(NULL, SPACE_DELIMITER);
      strcpy(file_name, file_name_p);
      //printf("ISP channel %s: command = %d,x=%d,file=%s\n", channel_name, INU_ISP_COMMAND_SET_LSC_E,set_by_file,file_name);
      ret = FwLib_ispCommand((inu_refH)streamerH, INU_ISP_COMMAND_SET_LSC_E, set_by_file, file_name);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("ISP channel %s: command = %d, OK\n", channel_name, INU_ISP_COMMAND_SET_LSC_E);
      }
   }

   return ret;
}


static ERRG_codeE exeRuntimeCmdIspGetLsc(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;
   inu_nodeH streamerH = NULL;
   char* channel_name;
   unsigned int set_by_file = 0;

   channel_name = strtok(NULL, SPACE_DELIMITER);
   ret = inu_device__findNode(deviceH, channel_name, &streamerH);
   if (ERRG_SUCCEEDED(ret))
   {
      cmd_param = strtok(NULL, SPACE_DELIMITER);
      set_by_file = (unsigned int)strtod(cmd_param, NULL);
      ret = FwLib_ispCommand((inu_refH)streamerH, INU_ISP_COMMAND_GET_LSC_E, set_by_file);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("ISP channel %s: command = %d, OK\n", channel_name, INU_ISP_COMMAND_GET_LSC_E);
      }
   }

   return ret;
}

static ERRG_codeE exeRuntimeCmdSetCrop(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;
   unsigned int startX = 0, startY = 0;
   unsigned int curruntStartX=0, curruntStartY=0;
   UINT32 sensorId, chipId, mode;

   cmd_param = strtok(NULL, SPACE_DELIMITER);
   sensorId = (int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   startX = (unsigned int)strtol(cmd_param, NULL, 0);
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   startY = (unsigned int)strtol(cmd_param, NULL, 0);
   if (ERRG_SUCCEEDED(ret))
   {

      ret = FwLib_setSensorCropParams(deviceH, sensorId, startX, startY);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("set crop to sensor_id%d: startX=%d,startY=%d\n",sensorId,startX,startY);
      }
      else
      {
         printf("failed to change\n");
      }
   }
   return ret;
}

static ERRG_codeE exeRuntimeCmdSetExposureMode(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* cmd_param;
   UINT32 sensor_control_id, exposure_time;
   float analog_gain, digital_gain;

   cmd_param         = strtok(NULL, SPACE_DELIMITER);
   sensor_control_id = (UINT32)strtol(cmd_param, NULL, 0);
   cmd_param      = strtok(NULL, SPACE_DELIMITER);
   exposure_time = (UINT32)strtol(cmd_param, NULL, 0);
   cmd_param      = strtok(NULL, SPACE_DELIMITER);
   analog_gain    = (float)strtol(cmd_param, NULL, 0);
   cmd_param      = strtok(NULL, SPACE_DELIMITER);
   digital_gain   = (float)strtol(cmd_param, NULL, 0);

   FwLib_set_manual_exposure_params(deviceH, sensor_control_id, exposure_time, analog_gain, digital_gain);

   return ret;
}

static ERRG_codeE exeRuntimeCmdRecordEnable(inu_deviceH deviceH)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   char* stream_name;
   char* cmd_param;
   UINT32 nFrames = 0;

   //get frame count
   cmd_param = strtok(NULL, SPACE_DELIMITER);
   if (cmd_param)
   {
       nFrames = (UINT32)strtol(cmd_param, NULL, 0);
       //get stream name
       stream_name = strtok(NULL, SPACE_DELIMITER);
       while (stream_name)
       {
           ret = setStreamRecordOutputFrames(stream_name, nFrames);

           if (ERRG_SUCCEEDED(ret))
           {
               printf("Record [%d] frames on stream [%s]: Passed\n", nFrames, stream_name);
           }
           else
           {
               printf("stream name [%s] is not valid. Please choose a valid stream name\n", stream_name);
           }
           //get next stream name
           stream_name = strtok(NULL, SPACE_DELIMITER);
       }
   }
   else
   {
       printf("must anter number of frames to record and a list os stream names\n");
       ret = (ERRG_codeE)ERR_INVALID_ARGS;
   }

   return ret;
}

static void exeRuntimeCmd(inu_deviceH deviceH, char *userInput)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   FWLib_accessRegisterT accessReg;
   char *cmd_param, *cmd;

   memset((void*)&accessReg, 0, sizeof(accessReg));
   // Remove \n character from end of string.
   userInput[strlen(userInput) - 1] = 0;
   cmd = strdup(userInput);

   cmd_param = strtok(userInput, SPACE_DELIMITER);

   if (cmd_param == NULL)
   {
      printf("Command can't be empty. Aborting.\n");
      return;
   }

   if ((strcmp(cmd_param, "help") == 0) || (strcmp(cmd_param, "h") == 0))
   {
      exeRuntimeCmdPrintHelp();
   }
   else if ((strcmp(cmd_param, "read") == 0) || (strcmp(cmd_param, "r") == 0))
   {
      ret = exeRuntimeCmdReadRegister(deviceH);
   }
   else if ((strcmp(cmd_param, "write") == 0) || (strcmp(cmd_param, "w") == 0))
   {
      ret = exeRuntimeCmdWriteRegister(deviceH);
   }
   else if ((strcmp(cmd_param, "read_i2c") == 0) || (strcmp(cmd_param, "R") == 0))
   {
      ret = exeRuntimeCmdReadI2cRegister(deviceH);
   }
   else if ((strcmp(cmd_param, "write_i2c") == 0) || (strcmp(cmd_param, "W") == 0))
   {
      ret = exeRuntimeCmdWriteI2cRegister(deviceH);
   }
   else if ((strcmp(cmd_param, "read_att") == 0) || (strcmp(cmd_param, "a") == 0))
   {
      ret = exeRuntimeCmdReadAttRegister(deviceH);
   }
   else if ((strcmp(cmd_param, "write_att") == 0) || (strcmp(cmd_param, "b") == 0))
   {
      ret = exeRuntimeCmdWriteAttRegister(deviceH);
   }
   else if ((strcmp(cmd_param, "exposure") == 0) || (strcmp(cmd_param, "e") == 0))
   {
      ret = exeRuntimeCmdSetExposure(deviceH);
   }
   else if ((strcmp(cmd_param, "reset_frame_count") == 0) || (strcmp(cmd_param, "s") == 0))
   {
      ret = exeRuntimeCmdResetFrameCount(deviceH);
   }
   else if ((strcmp(cmd_param, "set_user_params") == 0) || (strcmp(cmd_param, "u") == 0))
   {
      ret = exeRuntimeCmdSetUserParams(deviceH);
   }
   else if ((strcmp(cmd_param, "crop") == 0) || (strcmp(cmd_param, "c") == 0))
   {
      ret = exeRuntimeCmdSetCrop(deviceH);
   }
   else if ((strcmp(cmd_param, "load_file") == 0) || (strcmp(cmd_param, "lf") == 0))
   {
      ret = exeRuntimeCmdWriteFile(deviceH);
   }
   else if ((strcmp(cmd_param, "read_file") == 0) || (strcmp(cmd_param, "rf") == 0))
   {
      ret = exeRuntimeCmdReadFile(deviceH);
   }
   else if ((strcmp(cmd_param, "isp_get_exp") == 0) || (strcmp(cmd_param, "i0") == 0))
   {
      ret = exeRuntimeCmdIspSetExposure(deviceH);
   }
   else if ((strcmp(cmd_param, "isp_get_exp") == 0) || (strcmp(cmd_param, "i1") == 0))
   {
      ret = exeRuntimeCmdIspGetExposure(deviceH);
   }
   else if ((strcmp(cmd_param, "isp_set_wb") == 0) || (strcmp(cmd_param, "i2") == 0))
   {
      ret = exeRuntimeCmdIspSetWb(deviceH);
   }
   else if ((strcmp(cmd_param, "isp_get_wb") == 0) || (strcmp(cmd_param, "i3") == 0))
   {
      ret = exeRuntimeCmdIspGetWb(deviceH);
   }
   else if ((strcmp(cmd_param, "isp_set_lsc") == 0) || (strcmp(cmd_param, "i4") == 0))
   {
      ret = exeRuntimeCmdIspSetLsc(deviceH);
   }
   else if ((strcmp(cmd_param, "isp_get_lsc") == 0) || (strcmp(cmd_param, "i5") == 0))
   {
      ret = exeRuntimeCmdIspGetLsc(deviceH);
   }
   else if ((strcmp(cmd_param, "sw_reset") == 0) || (strcmp(cmd_param, "k") == 0))
   {
      ret = exeRuntimeCmdSwREset(deviceH);
   }
   else if ((strcmp(cmd_param, "set_network_id") == 0) || (strcmp(cmd_param, "n") == 0))
   {
      ret = exeRuntimeCmdSetNetworkId(deviceH);
   }
   else if ((strcmp(cmd_param, "lut_reload") == 0) || (strcmp(cmd_param, "l") == 0))
   {
      ret = exeRuntimeCmdReloadLutFiles(deviceH);
   }
   else if ((strcmp(cmd_param, "focus") == 0) || (strcmp(cmd_param, "f") == 0))
   {
      ret = exeRuntimeCmdFocus(deviceH);
   }
   else if ((strcmp(cmd_param, "set_exposure_mode") == 0) || (strcmp(cmd_param, "m") == 0))
   {
      ret = exeRuntimeCmdSetExposureMode(deviceH);
   }
   else if ((strcmp(cmd_param, "record_enable") == 0) || (strcmp(cmd_param, "RE") == 0))
   {
       ret = exeRuntimeCmdRecordEnable(deviceH);
        }
    else if ((strcmp(cmd_param, "fps") == 0) || (strcmp(cmd_param, "f") == 0))
   {
      UINT32 exposureUs, sensorGroupId, fps;
      float digGain, anaGain;
      UINT32 newFps;
      cmd_param = strtok(NULL, SPACE_DELIMITER);
      sensorGroupId = (int)strtol(cmd_param, NULL, 0);
      cmd_param = strtok(NULL, SPACE_DELIMITER);
      newFps = (UINT32)strtol(cmd_param, NULL, 0);

      ret = FwLib_get_sensorGroup_params(deviceH, sensorGroupId, &exposureUs, &anaGain, &digGain, &fps);
      if (ERRG_SUCCEEDED(ret))
      {
          ret = FwLib_set_sensorGroup_params(deviceH, sensorGroupId, exposureUs, anaGain, digGain, newFps);
          if (ERRG_SUCCEEDED(ret))
          {
             printf("Old FPS %d, changed to %d\n", fps, newFps);
          }
      }
   }
   else if ((strcmp(cmd_param, "temperature") == 0) || (strcmp(cmd_param, "t") == 0))
   {
      INT32 temperature = -5000;
      UINT32 exposureUs, sensorGroupId, fps;
      float digGain, anaGain;
      UINT32 sensorId;

      cmd_param = strtok(NULL, SPACE_DELIMITER);
      sensorId = (int)strtol(cmd_param, NULL, 0);

      ret = FwLib_get_sensor_params(deviceH, sensorId, &exposureUs, &anaGain, &digGain, &fps, &temperature);
      if (ERRG_SUCCEEDED(ret))
      {
         printf("[Sensor%d's temperature]: %d\n", sensorId, temperature);
      }
      else
      {
         printf("Error reading Sensor%d's temperature: 0x%x\n", sensorId, ret);
      }
   }
   else if ((strcmp(cmd_param, "debug_registers") == 0) || (strcmp(cmd_param, "D") == 0))
   {
       char *debugCfgFilePath = strtok(NULL, SPACE_DELIMITER);
       char* debugOutputFilePath = strtok(NULL, SPACE_DELIMITER);
       ret = DumpDebugRegisters(deviceH, debugCfgFilePath, debugOutputFilePath);
   }
   else
   {
      ret = (ERRG_codeE)ERR_INVALID_ARGS;
   }

   if (ERRG_FAILED(ret))
   {
      printf("Failed to run cmd [%s] on (0x%x)\n", cmd, ret);
   }

   free(cmd);
}

static void runtimeInputListener(void *argP)
{
   volatile inu_deviceH *deviceH = (inu_deviceH*)argP;
   char userInput[256];

   //Wait until device is initialized
   while ((*deviceH) == NULL)
   {

   }

   while (*deviceH)
   {
      printf("Enter Command: ");
      fgets(userInput, 256, stdin);
      printf("Execute %s\n", userInput);
      exeRuntimeCmd(*deviceH, userInput);
   }
   printf("Runtime cmd listenr exit\n");
}

void createRuntimeCmdsListener(inu_deviceH *deviceH)
{
#if (defined _WIN32) || (defined _WIN64)
   DWORD threadID;
   thrdH = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runtimeInputListener, deviceH, 0, &threadID);
#else
   typedef void *(*PTHREAD_ROUTINE) (void *);
   if (pthread_create((pthread_t *)&thrdH, NULL, (PTHREAD_ROUTINE)runtimeInputListener, deviceH))
   {
      assert(0);
   }
#endif
}
