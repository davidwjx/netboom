/****************************************************************************
 *
 *   FileName: err.c
 *
 *   Author:  Benny V.
 *
 *   Date: 
 *
 *   Description: error handling
 *
 ****************************************************************************/

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_common.h"
#include "inu_str.h"
#include <string.h>

/****************************************************************************
 ***************      L O C A L       D E F N I T I O N S     ***************
 ****************************************************************************/
#define INU_STRP_MAX_STRING_LENGTH   (50)

/****************************************************************************
 ***************      L O C A L       T Y P E D E F S         ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L    D A T A                     **************
****************************************************************************/

/****************************************************************************
 ***************      G L O B A L     D A T A                  **************
 ****************************************************************************/
 
/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S      **************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static char *INU_STRP_getProcessorString(ERRG_codeE err, char *stringP);
static char *INU_STRP_getModuleString(ERRG_codeE err, char *stringP);
static char *INU_STRP_getErrString(ERRG_codeE err, char *stringP);

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: INU_STRP_getProcessorString
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns:
*
*  Context: error handling
*
****************************************************************************/
static char *INU_STRP_getProcessorString(ERRG_codeE err, char *stringP)
{
   INU_DEFSG_pidE pid;
   pid = (INU_DEFSG_pidE)ERRG_GET_PID(err);
   strcpy(stringP, "PID: ");
   switch(pid)
   {
      case HOST_ID:
         strcat(stringP, "HOST");
         break;
      case GP_ID:
         strcat(stringP, "GP");
         break;
      case CEVA_ID:
         strcat(stringP, "CEVA");
         break;
      case IAF_ID:
         strcat(stringP, "IAF");
         break;
      default:
         strcat(stringP, "INVALID");
         break;
   }
   strcat (stringP, ", ");
   return(stringP);
}

/****************************************************************************
*
*  Function Name: INU_STRP_getModuleString
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns:
*
*  Context: error handling
*
****************************************************************************/
static char *INU_STRP_getModuleString(ERRG_codeE err, char *stringP)
{
   INU_DEFSG_moduleIdE mid;
   mid = (INU_DEFSG_moduleIdE)ERRG_GET_MID(err);
   strcpy(stringP, "MID: ");
   switch(mid)
   {

      case SVC_MNGR:
         strcat(stringP, "SVC_MNGR");
         break;
      case SVC_COM:
         strcat(stringP, "SVC_COM");
         break;
      case IAE:
         strcat(stringP, "IAE");
         break;
      case DPE:
         strcat(stringP, "DPE");
         break;
      case CDE:
         strcat(stringP, "CDE");
         break;
      case CONN_LYR:
         strcat(stringP, "CONN_LYR");
         break;
      case CONFIG:
         strcat(stringP, "CONFIG");
         break;
      case INU_FDK:
         strcat(stringP, "INU_FDK");
         break;
      case INU_COMM:
         strcat(stringP, "INU_COMM");
         break;
      case INU_LM:
         strcat(stringP, "INU_LM");
         break;
      case SENSOR:
         strcat(stringP, "SENSOR");
         break;
      case HW_MNGR:
         strcat(stringP, "HW_MNGR");
         break;
      case PAL:
         strcat(stringP, "PAL");
         break;
      case INIT:
         strcat(stringP, "INIT");
         break;
      case MEM_POOL:
         strcat(stringP, "MEM_POOL");
         break;
      case INU_LOGGER:
         strcat(stringP, "INU_LOGGER");
         break;
      case I2C:
         strcat(stringP, "I2C");
         break;
      case DATA_BASE:
         strcat(stringP, "DATA_BASE");
         break;
      case MEM_MAP:
         strcat(stringP, "MEM_MAP");
         break;
      case OS_LYR:
         strcat(stringP, "OS_LYR");
         break;
      case HW_REGS:
         strcat(stringP, "HW_REGS");
         break;
      case MSG_BOX:
         strcat(stringP, "MSG_BOX");
         break;
      case SERIAL:
         strcat(stringP, "SERIAL");
         break;
      case QUEUE_LYR:
         strcat(stringP, "QUEUE_LYR");
         break;
      case TCP_COM:
         strcat(stringP, "TCP_COM");
         break;
      case UDP_COM:
         strcat(stringP, "UDP_COM");
         break;
      case UART_COM:
         strcat(stringP, "UART_COM");
         break;
      case MSGBOX_COM:
         strcat(stringP, "MSGBOX_COM");
         break;
      case GME:
         strcat(stringP, "GME");
         break;
      case CEVA_BOOT:
         strcat(stringP, "CEVA_BOOT");
         break;
      case USB_CTRL_COM:
         strcat(stringP, "USB_CTRL_COM");
         break;
      case SYSTEM:
         strcat(stringP, "SYSTEM");
         break;
      case GPIO:
         strcat(stringP, "GPIO");
         break;
      case SPI:
         strcat(stringP, "SPI");
         break;
      case SPI_FLASH:
         strcat(stringP, "SPI_FLASH");
         break;
      case IPC_CTRL_COM:
         strcat(stringP, "IPC_CTRL_COM");
         break;

      default:
         strcat(stringP, "INVALID");
         break;
   }

   strcat (stringP, ", ");
   return(stringP);
}

/****************************************************************************
*
*  Function Name: INU_STRP_getErrString
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns:
*
*  Context: error handling
*
****************************************************************************/
static char *INU_STRP_getErrString(ERRG_codeE err, char *stringP)
{
   UINT16   errCode        = ERRG_GET_ERR_CODE(err);;
   UINT32   moduleErrCode  = ERRG_GET_ERR_CODE_AND_MID(err);

#if ((DEFSG_PROCESSOR == DEFSG_GP) || (DEFSG_PROCESSOR == DEFSG_HOST))

   strcpy(stringP, "ERR string: ");
   if(errCode <= ERR_LAST_COMMON)
   {
      switch(errCode)
      {
         case RET_SUCCESS:
            strcat(stringP, "RET_SUCCESS");
            break;
         case ERR_UNEXPECTED:
            strcat(stringP, "ERR_UNEXPECTED");
            break;
         case ERR_INVALID_ARGS:
            strcat(stringP, "ERR_INVALID_ARGS");
            break;
         case ERR_OUT_OF_MEM:
            strcat(stringP, "ERR_OUT_OF_MEM");
            break;
         case ERR_OUT_OF_RSRCS:
            strcat(stringP, "ERR_OUT_OF_RSRCS");
            break;
         case ERR_NULL_PTR:
            strcat(stringP, "ERR_NULL_PTR");
            break;
         case ERR_TIMEOUT:
            strcat(stringP, "ERR_TIMEOUT");
            break;
         case ERR_IO_ERROR:
            strcat(stringP, "ERR_IO_ERROR");
            break;
         case ERR_NOT_SUPPORTED:
            strcat(stringP, "ERR_NOT_SUPPORTED");
            break;
         case ERR_ILLEGAL_STATE:
            strcat(stringP, "ERR_ILLEGAL_STATE");
            break;
         case ERR_INVALID_STREAM_ID:
            strcat(stringP, "ERR_INVALID_STREAM_ID");
            break;
      }
   }
   else
   {
      switch(moduleErrCode | ERRG_PID_MASK(CURR_PID))
      {

// SVC_MNGR module errors
         case SVC_MNGR__ERR_CREATE_GP_HOST_GEN_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_GEN_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_HOST_SVC_MNGR_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_SVC_MNGR_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_HOST_VIDEO_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_VIDEO_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_HOST_DEPTH_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_DEPTH_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_HOST_HEAD_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_GAZE_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_HOST_CAMERA_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_CAMERA_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_HOST_LOGGER_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_LOGGER_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_CEVA_GEN_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_CEVA_GEN_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_CEVA_VIDEO_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_CEVA_VIDEO_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_CEVA_DEPTH_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_CEVA_DEPTH_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_CEVA_HEAD_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_CEVA_GAZE_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_CEVA_CAMERA_MEM_POOL_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_CEVA_CAMERA_MEM_POOL_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_HOST_CONNECTION_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_HOST_CONNECTION_FAIL");
            break;
         case SVC_MNGR__ERR_CREATE_GP_CEVA_CONNECTION_FAIL:
            strcat(stringP, "SVC_MNGR__ERR_CREATE_GP_CEVA_CONNECTION_FAIL");
            break;

// SVC module errors
         case SVC_COM__ERR_OPEN:
            strcat(stringP, "SVC_COM__ERR_OPEN");
            break;
         case SVC_COM__ERR_SEND:
            strcat(stringP, "SVC_COM__ERR_SEND");
            break;
         case SVC_COM__ERR_INVALID_CHANEL_NUM:
            strcat(stringP, "SVC_COM__ERR_INVALID_CHANEL_NUM");
            break;

// IAE module errors
         case IAE__ERR_INIT_FAIL_WRONG_PAL_HANDLE:
            strcat(stringP, "IAE__ERR_INIT_FAIL_WRONG_PAL_HANDLE");
            break;
         case IAE__ERR_OPEN_FAIL_DRV_NOT_CLOSED:
            strcat(stringP, "IAE__ERR_OPEN_FAIL_DRV_NOT_CLOSED");
            break;
         case IAE__ERR_OPEN_FAIL_NULL_PARAMS:
            strcat(stringP, "IAE__ERR_OPEN_FAIL_NULL_PARAMS");
            break;
         case IAE__ERR_OPEN_FAIL_NULL_REGS_ADDRESS:
            strcat(stringP, "IAE__ERR_OPEN_FAIL_NULL_REGS_ADDRESS");
            break;
         case IAE__ERR_IOCTL_FAIL_DRV_NOT_OPEN:
            strcat(stringP, "IAE__ERR_IOCTL_FAIL_DRV_NOT_OPEN");
            break;
         case IAE__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE:
            strcat(stringP, "IAE__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE");
            break;
         case IAE__ERR_IOCTL_FAIL_WRONG_HANDLE:
            strcat(stringP, "IAE__ERR_IOCTL_FAIL_WRONG_HANDLE");
            break;
         case IAE__ERR_ALN_CONTROL_FAIL:
            strcat(stringP, "IAE__ERR_ALN_CONTROL_FAIL");
            break;
         case IAE__ERR_BYPASS_FAIL_WRONG_MODULE:
            strcat(stringP, "IAE__ERR_BYPASS_FAIL_WRONG_MODULE");
            break;
         case IAE__ERR_ENABLE_FAIL_WRONG_MODULE:
            strcat(stringP, "IAE__ERR_ENABLE_FAIL_WRONG_MODULE");
            break;
         case IAE__ERR_IIM_ALN_FAIL_WRONG_MODULE:
            strcat(stringP, "IAE__ERR_IIM_ALN_FAIL_WRONG_MODULE");
            break;
         case IAE__ERR_FRAME_GEN_FAIL_WRONG_MODULE:
            strcat(stringP, "IAE__ERR_FRAME_GEN_FAIL_WRONG_MODULE");
            break;
         case IAE__ERR_PORCH_CONFIG_FAIL_WRONG_CAMERA:
            strcat(stringP, "IAE__ERR_PORCH_CONFIG_FAIL_WRONG_CAMERA");
            break;
         case IAE__ERR_IAU_FAIL_WRONG_MODULE:
            strcat(stringP, "IAE__ERR_IAU_FAIL_WRONG_MODULE");
            break;
         case IAE__ERR_HISTOGRAM_CFG_FAIL_WRONG_IAU:
            strcat(stringP, "IAE__ERR_HISTOGRAM_CFG_FAIL_WRONG_IAU");
            break;
         case IAE__ERR_IB_CFG_FAIL_WRONG_IAU:
            strcat(stringP, "IAE__ERR_IB_CFG_FAIL_WRONG_IAU");
            break;
         case IAE__ERR_WB_CFG_FAIL_WRONG_IAU:
            strcat(stringP, "IAE__ERR_WB_CFG_FAIL_WRONG_IAU");
            break;
          case IAE__ERR_CSC_CFG_FAIL_WRONG_IAU:
             strcat(stringP, "IAE__ERR_CSC_CFG_FAIL_WRONG_IAU");
             break;
          case IAE__ERR_DATA_FORMAT_FAIL_WRONG_IAU:
             strcat(stringP, "IAE__ERR_DATA_FORMAT_FAIL_WRONG_IAU");
             break;
          case IAE__ERR_FRR_FAIL_WRONG_IAU:
             strcat(stringP, "IAE__ERR_FRR_FAIL_WRONG_IAU");
             break;
          case IAE__ERR_SLU_CTRL_FAIL_WRONG_IAU:
             strcat(stringP, "IAE__ERR_SLU_CTRL_FAIL_WRONG_IAU");
             break;
          case IAE__ERR_SLU_CROP_FAIL:
             strcat(stringP, "IAE__ERR_SLU_CROP_FAIL");
             break;
          case IAE__ERR_SET_ISR_PARAMS_FAIL:
             strcat(stringP, "IAE__ERR_SET_ISR_PARAMS_FAIL");
             break;
          case IAE__ERR_ENABLE_ISR_FAIL:
             strcat(stringP, "IAE__ERR_ENABLE_ISR_FAIL");
             break;

// DPE module errors
         case DPE__ERR_INIT_FAIL_WRONG_PAL_HANDLE:
            strcat(stringP, "DPE__ERR_INIT_FAIL_WRONG_PAL_HANDLE");
            break;
         case DPE__ERR_OPEN_FAIL_DRV_NOT_CLOSED:
            strcat(stringP, "DPE__ERR_OPEN_FAIL_DRV_NOT_CLOSED");
            break;
         case DPE__ERR_OPEN_FAIL_NULL_PARAMS:
            strcat(stringP, "DPE__ERR_OPEN_FAIL_NULL_PARAMS");
            break;
         case DPE__ERR_OPEN_FAIL_NULL_REGS_ADDRESS:
            strcat(stringP, "DPE__ERR_OPEN_FAIL_NULL_REGS_ADDRESS");
            break;
         case DPE__ERR_IOCTL_FAIL_DRV_NOT_OPEN:
            strcat(stringP, "DPE__ERR_IOCTL_FAIL_DRV_NOT_OPEN");
            break;
         case DPE__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE:
            strcat(stringP, "DPE__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE");
            break;
         case DPE__ERR_IOCTL_FAIL_WRONG_HANDLE:
            strcat(stringP, "DPE__ERR_IOCTL_FAIL_WRONG_HANDLE");
            break;
         case DPE__ERR_SET_DISP2DEPTH_REFINE_FAIL_WRONG_BLOCK_ID:
            strcat(stringP, "DPE__ERR_SET_DISP2DEPTH_REFINE_FAIL_WRONG_BLOCK_ID");
            break;
         case DPE__ERR_SET_MASK_COND_FAIL_WRONG_COND_TYPE:
            strcat(stringP, "DPE__ERR_SET_MASK_COND_FAIL_WRONG_COND_TYPE");
            break;
         case DPE__ERR_SET_IMG_BLOCKS_FAIL_WRONG_NUM_BLOCKS:
            strcat(stringP, "DPE__ERR_SET_IMG_BLOCKS_FAIL_WRONG_NUM_BLOCKS");
            break;
         case DPE__ERR_INTEERUPT_CLR_FAIL_WRONG_NUM:
            strcat(stringP, "DPE__ERR_INTEERUPT_CLR_FAIL_WRONG_NUM");
            break;
         case DPE__ERR_SET_MUX_OUTPUT_FAIL_WRONG_PARAM:
            strcat(stringP, "DPE__ERR_SET_MUX_OUTPUT_FAIL_WRONG_PARAM");
            break;
         case DPE__ERR_GET_INTEERUPT_FAIL_WRONG_NUM:
            strcat(stringP, "DPE__ERR_GET_INTEERUPT_FAIL_WRONG_NUM");
            break;
         case DPE__ERR_INTEERUPT_CFG_FAIL_WRONG_NUM:
            strcat(stringP, "DPE__ERR_INTEERUPT_CFG_FAIL_WRONG_NUM");
            break;
         case DPE__ERR_SET_ISR_PARAMS_FAIL:
            strcat(stringP, "DPE__ERR_SET_ISR_PARAMS_FAIL");
            break;
         case DPE__ERR_ENABLE_ISR_FAIL:
            strcat(stringP, "DPE__ERR_ENABLE_ISR_FAIL");
            break;
         case DPE__ERR_SET_DISP_RANGE_BAD_RANGE:
            strcat(stringP, "DPE__ERR_SET_DISP_RANGE_BAD_RANGE");
            break;

// CDE module errors
         case CDE__ERR_INIT_FAIL_MNGR_FAIL:
            strcat(stringP, "CDE__ERR_INIT_FAIL_MNGR_FAIL");
            break;
         case CDE__ERR_INIT_FAIL_DRV_FAIL:
            strcat(stringP, "CDE__ERR_INIT_FAIL_DRV_FAIL");
            break;
         case CDE__ERR_OPEN_FAIL_MNGR_NOT_CLOSED:
            strcat(stringP, "CDE__ERR_OPEN_FAIL_MNGR_NOT_CLOSED");
            break;
         case CDE__ERR_OPEN_FAIL_DRV_NOT_CLOSED:
            strcat(stringP, "CDE__ERR_OPEN_FAIL_DRV_NOT_CLOSED");
            break;
         case CDE__ERR_OPEN_FAIL_NULL_PARAMS:
            strcat(stringP, "CDE__ERR_OPEN_FAIL_NULL_PARAMS");
            break;
         case CDE__ERR_OPEN_FAIL_DRV_NULL_REGS_ADDRESS:
            strcat(stringP, "CDE__ERR_OPEN_FAIL_DRV_NULL_REGS_ADDRESS");
            break;
         case CDE__ERR_OPEN_FAIL_DRV_DESCRIPTORS_INIT_FAIL:
            strcat(stringP, "CDE__ERR_OPEN_FAIL_DRV_DESCRIPTORS_INIT_FAIL");
            break;
         case CDE__ERR_IOCTL_FAIL_MNGR_NOT_OPEN:
            strcat(stringP, "CDE__ERR_IOCTL_FAIL_MNGR_NOT_OPEN");
            break;
         case CDE__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE:
            strcat(stringP, "CDE__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE");
            break;
         case CDE__ERR_CH_OPEN_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_OPEN_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_OPEN_IOCTL_CH_NOT_CLOSED:
            strcat(stringP, "CDE__ERR_CH_OPEN_IOCTL_CH_NOT_CLOSED");
            break;
         case CDE__ERR_CH_OPEN_IOCTL_OPEN_DRV_FAIL:
            strcat(stringP, "CDE__ERR_CH_OPEN_IOCTL_OPEN_DRV_FAIL");
            break;
         case CDE__ERR_CH_OPEN_IOCTL_SET_DEF_PARAMS_FAIL:
            strcat(stringP, "CDE__ERR_CH_OPEN_IOCTL_SET_DEF_PARAMS_FAIL");
            break;
         case CDE__ERR_DRV_CH_CFG_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_DRV_CH_CFG_NULL_HANDLE");
            break;
         case CDE__ERR_DRV_CH_CFG_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_DRV_CH_CFG_CH_NOT_OPEN");
            break;
         case CDE__ERR_DRV_CH_CFG_DESC_CFG_FAIL:
            strcat(stringP, "CDE__ERR_DRV_CH_CFG_DESC_CFG_FAIL");
            break;
         case CDE__ERR_DRV_CH_CFG_REGS_CFG_FAIL:
            strcat(stringP, "CDE__ERR_DRV_CH_CFG_REGS_CFG_FAIL");
            break;
         case CDE__ERR_CH_SET_PARAMS_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_SET_PARAMS_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_SET_PARAMS_IOCTL_WRONG_CH:
            strcat(stringP, "CDE__ERR_CH_SET_PARAMS_IOCTL_WRONG_CH");
            break;
         case CDE__ERR_CH_SET_PARAMS_IOCTL_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_CH_SET_PARAMS_IOCTL_CH_NOT_OPEN");
            break;
         case CDE__ERR_CH_GET_PARAMS_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_GET_PARAMS_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_GET_PARAMS_IOCTL_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_CH_GET_PARAMS_IOCTL_CH_NOT_OPEN");
            break;
         case CDE__ERR_GP_CH_SET_PARAMS_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_GP_CH_SET_PARAMS_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_GP_CH_SET_PARAMS_IOCTL_WRONG_CH:
            strcat(stringP, "CDE__ERR_GP_CH_SET_PARAMS_IOCTL_WRONG_CH");
            break;
         case CDE__ERR_GP_CH_SET_PARAMS_IOCTL_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_GP_CH_SET_PARAMS_IOCTL_CH_NOT_OPEN");
            break;
         case CDE__ERR_CH_SET_DEF_PARAMS_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_SET_DEF_PARAMS_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_SET_DEF_PARAMS_IOCTL_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_CH_SET_DEF_PARAMS_IOCTL_CH_NOT_OPEN");
            break;
         case CDE__ERR_CH_CLOSE_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_CLOSE_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_CLOSE_IOCTL_CH_IS_CLOSED:
            strcat(stringP, "CDE__ERR_CH_CLOSE_IOCTL_CH_IS_CLOSED");
            break;
         case CDE__ERR_CH_CLOSE_IOCTL_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_CH_CLOSE_IOCTL_CH_NOT_OPEN");
            break;
         case CDE__ERR_CH_CLOSE_IOCTL_INIT_REGS_FAIL:
            strcat(stringP, "CDE__ERR_CH_CLOSE_IOCTL_INIT_REGS_FAIL");
            break;
         case CDE__ERR_CH_CLOSE_IOCTL_REMOVE_DESCR_FAIL:
            strcat(stringP, "CDE__ERR_CH_CLOSE_IOCTL_REMOVE_DESCR_FAIL");
            break;
         case CDE__ERR_CH_ACTIVATE_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_ACTIVATE_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_ACTIVATE_IOCTL_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_CH_ACTIVATE_IOCTL_CH_NOT_OPEN");
            break;
         case CDE__ERR_DRV_CH_ACTIVATE_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_DRV_CH_ACTIVATE_NULL_HANDLE");
            break;
         case CDE__ERR_DRV_CH_ACTIVATE_CH_NOT_OPEN:
            strcat(stringP, "CDE__ERR_DRV_CH_ACTIVATE_CH_NOT_OPEN");
            break;
         case CDE__ERR_CH_DEACTIVATE_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_DEACTIVATE_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_DEACTIVATE_IOCTL_CH_NOT_ACTIVE:
            strcat(stringP, "CDE__ERR_CH_DEACTIVATE_IOCTL_CH_NOT_ACTIVE");
            break;
         case CDE__ERR_DRV_CH_DEACTIVATE_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_DRV_CH_DEACTIVATE_NULL_HANDLE");
            break;
         case CDE__ERR_DRV_CH_DEACTIVATE_CH_NOT_ACTIVE:
            strcat(stringP, "CDE__ERR_DRV_CH_DEACTIVATE_CH_NOT_ACTIVE");
            break;
         case CDE__ERR_DRV_CH_DEACTIVATE_CLEAR_LEFT_CMD:
            strcat(stringP, "CDE__ERR_DRV_CH_DEACTIVATE_CLEAR_LEFT_CMD");
            break;
         case CDE__ERR_CH_POLLING_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_POLLING_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_POLLING_IOCTL_NO_FRAMES:
            strcat(stringP, "CDE__ERR_CH_POLLING_IOCTL_NO_FRAMES");
            break;
         case CDE__ERR_DRV_CH_POLLING_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_DRV_CH_POLLING_NULL_HANDLE");
            break;
         case CDE__ERR_DRV_CH_POLLING_CH_NOT_ACTIVE:
            strcat(stringP, "CDE__ERR_DRV_CH_POLLING_CH_NOT_ACTIVE");
            break;
         case CDE__ERR_CH_PRINT_DEBUG_IOCTL_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_CH_PRINT_DEBUG_IOCTL_NULL_HANDLE");
            break;
         case CDE__ERR_CH_PRINT_DEBUG_IOCTL_CH_CLOSE:
            strcat(stringP, "CDE__ERR_CH_PRINT_DEBUG_IOCTL_CH_CLOSE");
            break;
         case CDE__ERR_DRV_CH_PRINT_DEBUG_NULL_HANDLE:
            strcat(stringP, "CDE__ERR_DRV_CH_PRINT_DEBUG_NULL_HANDLE");
            break;
         case CDE__ERR_DRV_CH_PRINT_DEBUG_CH_CLOSE:
            strcat(stringP, "CDE__ERR_DRV_CH_PRINT_DEBUG_CH_CLOSE");
            break;
         case CDE__ERR_SET_ISR_PARAMS_FAIL:
            strcat(stringP, "CDE__ERR_SET_ISR_PARAMS_FAIL");
            break;

// CONN_LYR module errors
         case CONN_LYR__ERR_NO_FREE_CONNECTIONS:
            strcat(stringP, "CONN_LYR__ERR_NO_FREE_CONNECTIONS");
            break;
         case CONN_LYR__ERR_CONNECTION_IS_DOWN:
            strcat(stringP, "CONN_LYR__ERR_CONNECTION_IS_DOWN");
            break;

// Config module errors
         case CONFIG__ERR_MISSING_OPT:
            strcat(stringP, "CONFIG__ERR_MISSING_OPT");
            break;

// Sensors module errors
         case SENSOR__ERR_CONFIGUTATION_FAILED:
            strcat(stringP, "SENSOR__ERR_CONFIGUTATION_FAILED");
            break;
         case SENSOR__ERR_EXP_TIME_FAILED:
            strcat(stringP, "SENSOR__ERR_EXP_TIME_FAILED");
            break;
         case SENSOR__ERR_AUTO_EXP_NOT_LOCKED_FAIL:
            strcat(stringP, "SENSOR__ERR_AUTO_EXP_NOT_LOCKED_FAIL");
            break;
         case SENSOR__ERR_AUTO_EXP_MODE_SET_FAILED:
            strcat(stringP, "SENSOR__ERR_AUTO_EXP_MODE_SET_FAILED");
            break;
         case SENSOR__ERR_BAD_SENSOR_FORMAT:
            strcat(stringP, "SENSOR__ERR_BAD_SENSOR_FORMAT");
            break;
         case SENSOR__ERR_USR_INIT_FAILED:
            strcat(stringP, "SENSOR__ERR_USR_INIT_FAILED");
            break;
         case SENSOR__ERR_BAD_INIT_TABLE:
            strcat(stringP, "SENSOR__ERR_BAD_INIT_TABLE");
            break;
         case SENSOR__ERR_WRONG_POWER_FREQUENCY:
            strcat(stringP, "SENSOR__ERR_WRONG_POWER_FREQUENCY");
            break;
         case SENSOR__ERR_START_SENSOR_FAIL_WRONG_PARAM:
            strcat(stringP, "SENSOR__ERR_START_SENSOR_FAIL_WRONG_PARAM");
            break;
         case SENSOR__ERR_OPEN_DEVICE_FAIL:
            strcat(stringP, "SENSOR__ERR_OPEN_DEVICE_FAIL");
            break;
         case SENSOR__ERR_INIT_SENSOR_FAIL_WRONG_MODEL:
            strcat(stringP, "SENSOR__ERR_INIT_SENSOR_FAIL_WRONG_MODEL");
            break;
         case SENSOR__ERR_SET_OUT_FORMAT_FAIL_WRONG_FORMAT:
            strcat(stringP, "SENSOR__ERR_SET_OUT_FORMAT_FAIL_WRONG_FORMAT");
            break;

// HW_MNGR module errors
         case HW_MNGR__ERR_CREATE_THREAD_FAIL:
            strcat(stringP, "HW_MNGR__ERR_CREATE_THREAD_FAIL");
            break;
         case HW_MNGR__LOAD_CONFIG_FAIL:
            strcat(stringP, "HW_MNGR__LOAD_CONFIG_FAIL");
            break;
         case HW_MNGR__ERR_POLL_INACTIVE_USER:
            strcat(stringP, "HW_MNGR__ERR_POLL_INACTIVE_USER");
            break;
         case HW_MNGR__ERR_LOAD_IAE_LUT_FAIL:
            strcat(stringP, "HW_MNGR__ERR_LOAD_IAE_LUT_FAIL");
            break;
         case HW_MNGR__ERR_POLL_TIME_OUT:
            strcat(stringP, "HW_MNGR__ERR_POLL_TIME_OUT");
            break;
         case HW_MNGR__ERR_OUT_CHAN_FREE_TIMEOUT:
            strcat(stringP, "HW_MNGR__ERR_OUT_CHAN_FREE_TIMEOUT");
            break;
         case HW_REGS__ERR_WRITE_SOC_REG_FAIL:
            strcat(stringP, "HW_REGS__ERR_WRITE_SOC_REG_FAIL");
            break;
         case HW_REGS__ERR_READ_SOC_REG_FAIL:
            strcat(stringP, "HW_REGS__ERR_READ_SOC_REG_FAIL");
            break;

// PAL module errors
         case PAL__ERR_INIT_FAIL:
            strcat(stringP, "PAL__ERR_INIT_FAIL");
            break;
         case PAL__ERR_INVALID_DEVICE_ID:
            strcat(stringP, "PAL__ERR_INVALID_DEVICE_ID");
            break;
         case PAL__ERR_INVALID_DEVICE_HANDLE:
            strcat(stringP, "PAL__ERR_INVALID_DEVICE_HANDLE");
            break;
         case PAL__ERR_INVALID_OPEN_FUNC:
            strcat(stringP, "PAL__ERR_INVALID_OPEN_FUNC");
            break;
         case PAL__ERR_INVALID_CLOSE_FUNC:
            strcat(stringP, "PAL__ERR_INVALID_CLOSE_FUNC");
            break;
         case PAL__ERR_INVALID_READ_FUNC:
            strcat(stringP, "PAL__ERR_INVALID_READ_FUNC");
            break;
         case PAL__ERR_INVALID_WRITE_FUNC:
            strcat(stringP, "PAL__ERR_INVALID_WRITE_FUNC");
            break;
         case PAL__ERR_INVALID_IOCTL_FUNC:
            strcat(stringP, "PAL__ERR_INVALID_IOCTL_FUNC");
            break;

// Init module errors
         case INIT__ERR_GENERAL_FAIL:
            strcat(stringP, "INIT__ERR_GENERAL_FAIL");
            break;
         case INIT__ERR_SCHEDULER_INIT_FAIL:
            strcat(stringP, "INIT__ERR_SCHEDULER_INIT_FAIL");
            break;
         case INIT__ERR_OPEN_VDMA_FAIL:
            strcat(stringP, "INIT__ERR_OPEN_VDMA_FAIL");
            break;

// MEM_POOL module errors
         case MEM_POOL__ERR_ALLOC_BUF_FAIL_NULL_HANDLE:
            strcat(stringP, "MEM_POOL__ERR_ALLOC_BUF_FAIL_NULL_HANDLE");
            break;
         case MEM_POOL__ERR_ALLOC_BUF_FAIL_BIG_BUF_SIZE:
            strcat(stringP, "MEM_POOL__ERR_ALLOC_BUF_FAIL_BIG_BUF_SIZE");
            break;
         case MEM_POOL__ERR_ALLOC_BUF_FAIL_NO_BUF_IN_POOL:
            strcat(stringP, "MEM_POOL__ERR_ALLOC_BUF_FAIL_NO_BUF_IN_POOL");
            break;

// LOGGER module errors
         case LOGGER__QUE_TIMEOUT:
            strcat(stringP, "LOGGER__QUE_TIMEOUT");
            break;
         case LOGGER__ERR_CREATE_THREAD_FAIL:
            strcat(stringP, "LOGGER__ERR_CREATE_THREAD_FAIL");
            break;
         case LOGGER__ERR_THREAD_ALREADY_EXIST:
            strcat(stringP, "LOGGER__ERR_THREAD_ALREADY_EXIST");
            break;
         case LOGGER__ERR_CREATE_MSG_Q_FAIL:
            strcat(stringP, "LOGGER__ERR_CREATE_MSG_Q_FAIL");
            break;

// I2C module errors
         case I2C__ERR_UPDATE_ADDRESS_I2C_BUSY:
            strcat(stringP, "I2C__ERR_UPDATE_ADDRESS_I2C_BUSY");
            break;
         case I2C__ERR_WRITE_I2C_BUSY:
            strcat(stringP, "I2C__ERR_WRITE_I2C_BUSY");
            break;
         case I2C__ERR_READ_I2C_BUSY:
            strcat(stringP, "I2C__ERR_READ_I2C_BUSY");
            break;
         case I2C__ERR_NO_ACK:
            strcat(stringP, "I2C__ERR_NO_ACK");
            break;
         case I2C__ERR_WRITE_I2C_ABORT:
            strcat(stringP, "I2C__ERR_WRITE_I2C_ABORT");
            break;
         case I2C__ERR_READ_FAIL_TO_RECEIVE_ALL_DATA:
            strcat(stringP, "I2C__ERR_READ_FAIL_TO_RECEIVE_ALL_DATA");
            break;
         case I2C__ERR_IOCTL_FAIL_WRONG_HANDLE:
            strcat(stringP, "I2C__ERR_IOCTL_FAIL_WRONG_HANDLE");
            break;
         case I2C__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE:
            strcat(stringP, "I2C__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE");
            break;
         case I2C__ERR_IOCTL_FAIL_DRV_NOT_OPEN:
            strcat(stringP, "I2C__ERR_IOCTL_FAIL_DRV_NOT_OPEN");
            break;
         case I2C__ERR_INIT_FAIL_WRONG_PAL_HANDLE:
            strcat(stringP, "I2C__ERR_INIT_FAIL_WRONG_PAL_HANDLE");
            break;
         case I2C__ERR_OPEN_FAIL_NULL_PERIPH_ADDRESS:
            strcat(stringP, "I2C__ERR_OPEN_FAIL_NULL_PERIPH_ADDRESS");
            break;
         case I2C__ERR_OPEN_FAIL_NULL_PARAMS:
            strcat(stringP, "I2C__ERR_OPEN_FAIL_NULL_PARAMS");
            break;
         case I2C__ERR_OPEN_FAIL_DRV_NOT_CLOSED:
            strcat(stringP, "I2C__ERR_OPEN_FAIL_DRV_NOT_CLOSED");
            break;
         case I2C__ERR_FAILED_TO_ENABLE_PERIHERAL:
            strcat(stringP, "I2C__ERR_FAILED_TO_ENABLE_PERIHERAL");
            break;
         case I2C__ERR_FAILED_TO_DISABLE_PERIHERAL:
            strcat(stringP, "I2C__ERR_FAILED_TO_DISABLE_PERIHERAL");
            break;
         case I2C__ERR_ILLEGAL_NUM_OF_BYTES_TO_WRITE:
            strcat(stringP, "I2C__ERR_ILLEGAL_NUM_OF_BYTES_TO_WRITE");
            break;
         case I2C__ERR_ILLEGAL_NUM_OF_BYTES_TO_READ:
            strcat(stringP, "I2C__ERR_ILLEGAL_NUM_OF_BYTES_TO_READ");
            break;
         case I2C__ERR_WRITE_ILLEGAL_NUM_OF_ADDRESS_ACCESS_SIZE:
            strcat(stringP, "I2C__ERR_WRITE_ILLEGAL_NUM_OF_ADDRESS_ACCESS_SIZE");
            break;
         case I2C__ERR_READ_ILLEGAL_NUM_OF_ADDRESS_ACCESS_SIZE:
            strcat(stringP, "I2C__ERR_READ_ILLEGAL_NUM_OF_ADDRESS_ACCESS_SIZE");
            break;
         case I2C__ERR_INIT_FAIL:
            strcat(stringP, "I2C__ERR_INIT_FAIL");
            break;
         case I2C__ERR_OPEN_HL_DRV_FAIL_WRONG_DEVICE_ID:
            strcat(stringP, "I2C__ERR_OPEN_HL_DRV_FAIL_WRONG_DEVICE_ID");
            break;

// DATA_BASE module errors
         case DATA_BASE__ERR_MODULE_ALREADY_OPEN:
            strcat(stringP, "DATA_BASE__ERR_MODULE_ALREADY_OPEN");
            break;
         case DATA_BASE__ERR_MODULE_NOT_OPEN:
            strcat(stringP, "DATA_BASE__ERR_MODULE_NOT_OPEN");
            break;
         case DATA_BASE__ERR_CPMUTEX_ACQUIRE_FAIL:
            strcat(stringP, "DATA_BASE__ERR_CPMUTEX_ACQUIRE_FAIL");
            break;

// MEM_MAP module errors
         case MEM_MAP__ERR_REGION_NO_FOUND:
            strcat(stringP, "MEM_MAP__ERR_REGION_NO_FOUND");
            break;

// OS_LYR module errors
         case OS_LYR__ERR_MEM_FD_OPEN_FAIL:
            strcat(stringP, "OS_LYR__ERR_MEM_FD_OPEN_FAIL");
            break;
         case OS_LYR__ERR_REGION_MAP_FAIL:
            strcat(stringP, "OS_LYR__ERR_REGION_MAP_FAIL");
            break;
         case OS_LYR__ERR_REGION_UNMAP_FAIL:
            strcat(stringP, "OS_LYR__ERR_REGION_UNMAP_FAIL");
            break;
         case OS_LYR__ERR_WAIT_THREAD_FAIL:
            strcat(stringP, "OS_LYR__ERR_WAIT_THREAD_FAIL");
            break;

// HW_REGS module errors
         case HW_REGS__TABLE_FULL:
            strcat(stringP, "HW_REGS__TABLE_FULL");
            break;
         case HW_REGS__INSERT_WRONG_PARAM_TO_TABLE:
            strcat(stringP, "HW_REGS__INSERT_WRONG_PARAM_TO_TABLE");
            break;

// MSG_BOX errors
         case MSG_BOX__ERR_INIT_FAIL:
            strcat(stringP, "MSG_BOX__ERR_INIT_FAIL");
            break;
         case MSG_BOX__ERR_OPEN_FAIL:
            strcat(stringP, "MSG_BOX__ERR_OPEN_FAIL");
            break;
         case MSG_BOX__ERR_ADD_MSG_FAIL_BOX_FULL:
            strcat(stringP, "MSG_BOX__ERR_ADD_MSG_FAIL_BOX_FULL");
            break;
         case MSG_BOX__ERR_BOX_IS_EMPTY:
            strcat(stringP, "MSG_BOX__ERR_BOX_IS_EMPTY");
            break;

// SERIAL errors
         case SERIAL__ERR_GENERAL_ERR:
            strcat(stringP, "SERIAL__ERR_GENERAL_ERR");
            break;
         case SERIAL__ERR_OPEN_FAIL:
            strcat(stringP, "SERIAL__ERR_OPEN_FAIL");
            break;
         case SERIAL__ERR_OPEN_FAIL_INVALID_HANDLE:
            strcat(stringP, "SERIAL__ERR_OPEN_FAIL_INVALID_HANDLE");
            break;
         case SERIAL__ERR_CLOSE_FAIL:
            strcat(stringP, "SERIAL__ERR_CLOSE_FAIL");
            break;
         case SERIAL__ERR_SEND_FAIL:
            strcat(stringP, "SERIAL__ERR_SEND_FAIL");
            break;
         case SERIAL__ERR_SEND_FAIL_INVALID_HANDLE:
            strcat(stringP, "SERIAL__ERR_SEND_FAIL_INVALID_HANDLE");
            break;
          case SERIAL__ERR_RECV_FAIL:
            strcat(stringP, "SERIAL__ERR_RECV_FAIL");
            break;
         case SERIAL__ERR_RECV_FAIL_INVALID_HANDLE:
            strcat(stringP, "SERIAL__ERR_RECV_FAIL_INVALID_HANDLE");
            break;
         case SERIAL__ERR_READ_FAIL:
            strcat(stringP, "SERIAL__ERR_READ_FAIL");
            break;
         case SERIAL__ERR_READ_TIME_OUT:
            strcat(stringP, "SERIAL__ERR_READ_TIME_OUT");
            break;
         case SERIAL__ERR_READ_BLOCK_FAIL_INVALID_HANDLE:
            strcat(stringP, "SERIAL__ERR_READ_BLOCK_FAIL_INVALID_HANDLE");
            break;

// QUEUE_LYR errors
         case QUEUE_LYR__ERR_GENERAL_ERR:
            strcat(stringP, "QUEUE_LYR__ERR_GENERAL_ERR");
            break;

// TCP_COM errors
         case TCP_COM__ERR_GENERAL_ERROR:
            strcat(stringP, "TCP_COM__ERR_GENERAL_ERROR");
            break;
         case TCP_COM__ERR_BIND_FAIL:
            strcat(stringP, "TCP_COM__ERR_BIND_FAIL");
            break;
         case TCP_COM__ERR_CONNECT_FAIL:
            strcat(stringP, "TCP_COM__ERR_CONNECT_FAIL");
            break;
         case TCP_COM__ERR_SOCKET_INIT_FAIL:
            strcat(stringP, "TCP_COM__ERR_SOCKET_INIT_FAIL");
            break;
         case TCP_COM__ERR_SOCKET_ADDR_CREATE_FAIL:
            strcat(stringP, "TCP_COM__ERR_SOCKET_ADDR_CREATE_FAIL");
            break;

// UDP_COM errors
         case UDP_COM__ERR_GENERAL_ERROR:
            strcat(stringP, "UDP_COM__ERR_GENERAL_ERROR");
            break;

// UART_COM errors
         case UART_COM__ERR_GENERAL_ERROR:
            strcat(stringP, "UART_COM__ERR_GENERAL_ERROR");
            break;
         case UART_COM__ERR_INIT_FAIL:
            strcat(stringP, "UART_COM__ERR_INIT_FAIL");
            break;
         case UART_COM__ERR_OPEN_FAIL:
            strcat(stringP, "UART_COM__ERR_OPEN_FAIL");
            break;
         case UART_COM__ERR_CLOSE_FAIL:
            strcat(stringP, "UART_COM__ERR_CLOSE_FAIL");
            break;
         case UART_COM__ERR_SEND_FAIL:
            strcat(stringP, "UART_COM__ERR_SEND_FAIL");
            break;

// MSGBOX_COM errors
         case MSGBOX_COM__ERR_GENERAL_ERROR:
            strcat(stringP, "MSGBOX_COM__ERR_GENERAL_ERROR");
            break;
         case MSGBOX_COM__ERR_FAIL_ADD_MSG_LEN_TOO_LARGE:
            strcat(stringP, "MSGBOX_COM__ERR_FAIL_ADD_MSG_LEN_TOO_LARGE");
            break;
         case MSGBOX_COM__ERR_FAIL_REMOVE_MSG_LEN_TOO_LARGE:
            strcat(stringP, "MSGBOX_COM__ERR_FAIL_REMOVE_MSG_LEN_TOO_LARGE");
            break;
         case MSGBOX_COM__ERR_FAIL_PEEK_MSG_LEN_TOO_LARGE:
            strcat(stringP, "MSGBOX_COM__ERR_FAIL_PEEK_MSG_LEN_TOO_LARGE");
            break;

// GME errors
         case GME__ERR_INVALID_POWER_MODE:
            strcat(stringP, "GME__ERR_INVALID_POWER_MODE");
            break;
         case GME__ERR_INVALID_CLK_MODULE:
            strcat(stringP, "GME__ERR_INVALID_CLK_MODULE");
            break;
         case GME__ERR_OPEN_FAIL_NULL_REGS_ADDRESS:
            strcat(stringP, "GME__ERR_OPEN_FAIL_NULL_REGS_ADDRESS");
            break;
         case GME__ERR_OPEN_FAIL_NULL_PARAMS:
            strcat(stringP, "GME__ERR_OPEN_FAIL_NULL_PARAMS");
            break;
         case GME__ERR_OPEN_FAIL_DRV_NOT_CLOSED:
            strcat(stringP, "GME__ERR_OPEN_FAIL_DRV_NOT_CLOSED");
            break;
         case GME__ERR_IOCTL_FAIL_WRONG_HANDLE:
            strcat(stringP, "GME__ERR_IOCTL_FAIL_WRONG_HANDLE");
            break;
         case GME__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE:
            strcat(stringP, "GME__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE");
            break;
         case GME__ERR_IOCTL_FAIL_DRV_NOT_OPEN:
            strcat(stringP, "GME__ERR_IOCTL_FAIL_DRV_NOT_OPEN");
            break;
         case GME__ERR_INIT_FAIL_WRONG_PAL_HANDLE:
            strcat(stringP, "GME__ERR_INIT_FAIL_WRONG_PAL_HANDLE");
            break;
         case GME__ERR_INVALID_IO_PINS:
            strcat(stringP, "GME__ERR_INVALID_IO_PINS");
            break;
         case GME__ERR_GME_NOT_READY:
            strcat(stringP, "GME__ERR_GME_NOT_READY");
            break;

// CEVA_BOOT errors
         case CEVA__ERR_INVALID_CEVA_ID:
            strcat(stringP, "CEVA__ERR_INVALID_CEVA_ID");
            break;
         case CEVA__ERR_FAILED_TO_OPEN_FILE:
            strcat(stringP, "CEVA__ERR_FAILED_TO_OPEN_FILE");
            break;
         case CEVA__ERR_INVALID_DATA_MEM_ID:
            strcat(stringP, "CEVA__ERR_INVALID_DATA_MEM_ID");
            break;
         case CEVA__ERR_CEVA_ALREADY_RUNNING:
            strcat(stringP, "CEVA__ERR_CEVA_ALREADY_RUNNING");
            break;

// USB_CTRL_COM errors
         case USB_CTRL_COM__ERR_OPEN_FAILED:
            strcat(stringP, "USB_CTRL_COM__ERR_OPEN_FAILED");
            break;
         case USB_CTRL_COM__ERR_CLOSE_FAILED:
            strcat(stringP, "USB_CTRL_COM__ERR_CLOSE_FAILED");
            break;
         case USB_CTRL_COM__ERR_WRITE_FAILED:
            strcat(stringP, "USB_CTRL_COM__ERR_WRITE_FAILED");
            break;
         case USB_CTRL_COM__ERR_READ_FAILED:
            strcat(stringP, "USB_CTRL_COM__ERR_READ_FAILED");
            break;
         case USB_CTRL_COM__ERR_LSEEK_FAILED:
            strcat(stringP, "USB_CTRL_COM__ERR_LSEEK_FAILED");
            break;
         case USB_CTRL_COM__ERR_DEV_NOT_INIT:
            strcat(stringP, "USB_CTRL_COM__ERR_DEV_NOT_INIT");
            break;

// IPC_CTRL_COM errors
         case IPC_CTRL_COM__ERR_OPEN_FAILED:
            strcat(stringP, "IPC_CTRL_COM__ERR_OPEN_FAILED");
            break;
         case IPC_CTRL_COM__ERR_CLOSE_FAILED:
            strcat(stringP, "IPC_CTRL_COM__ERR_CLOSE_FAILED");
            break;
         case IPC_CTRL_COM__ERR_SEND_FAILED:
            strcat(stringP, "IPC_CTRL_COM__ERR_SEND_FAILED");
            break;
         case IPC_CTRL_COM__ERR_READ_FAILED:
            strcat(stringP, "IPC_CTRL_COM__ERR_READ_FAILED");
            break;
         case IPC_CTRL_COM__ERR_DEV_NOT_INIT:
            strcat(stringP, "IPC_CTRL_COM__ERR_DEV_NOT_INIT");
            break;

// SYSTEM errors
         case SYSTEM__ERR_DB_IOCTL_FAIL_WRONG_PARAM:
            strcat(stringP, "SYSTEM__ERR_DB_IOCTL_FAIL_WRONG_PARAM");
            break;
         case SYSTEM__ERR_WRITE_SENSOR_REG_FAIL_WRONG_SENSOR_TYPE:
            strcat(stringP, "SYSTEM__ERR_WRITE_SENSOR_REG_FAIL_WRONG_SENSOR_TYPE");
            break;
         case SYSTEM__ERR_WRITE_SENSOR_REG_FAIL_WRONG_REG_WIDTH:
            strcat(stringP, "SYSTEM__ERR_WRITE_SENSOR_REG_FAIL_WRONG_REG_WIDTH");
            break;
         case SYSTEM__ERR_WRITE_SENSOR_REG_FAIL_WRONG_REG_TYPE:
            strcat(stringP, "SYSTEM__ERR_WRITE_SENSOR_REG_FAIL_WRONG_REG_TYPE");
            break;
         case SYSTEM__ERR_READ_SENSOR_REG_FAIL_WRONG_SENSOR_TYPE:
            strcat(stringP, "SYSTEM__ERR_READ_SENSOR_REG_FAIL_WRONG_SENSOR_TYPE");
            break;
         case SYSTEM__ERR_READ_SENSOR_REG_FAIL_WRONG_REG_WIDTH:
            strcat(stringP, "SYSTEM__ERR_READ_SENSOR_REG_FAIL_WRONG_REG_WIDTH");
            break;
         case SYSTEM__ERR_READ_SENSOR_REG_FAIL_WRONG_REG_TYPE:
            strcat(stringP, "SYSTEM__ERR_READ_SENSOR_REG_FAIL_WRONG_REG_TYPE");
            break;
         case SYSTEM__ERR_SENSOR_CFG_FAIL_WRONG_CMD:
            strcat(stringP, "SYSTEM__ERR_SENSOR_CFG_FAIL_WRONG_CMD");
            break;
         case SYSTEM__ERR_FLASH_SIZE_IS_BIGGER_THAN_MAX_FLASH_SIZE:
            strcat(stringP, "SYSTEM__ERR_FLASH_SIZE_IS_BIGGER_THAN_MAX_FLASH_SIZE");
            break;
         case SYSTEM__ERR_FLASH_ADDRESS_IS_NOT_ALIGNED_256_BYTE:
            strcat(stringP, "SYSTEM__ERR_FLASH_ADDRESS_IS_NOT_ALIGNED_256_BYTE");
            break;
         case SYSTEM__ERR_FLASH_IO_FLASH_HANDLE_IS_NULL:
            strcat(stringP, "SYSTEM__ERR_FLASH_IO_FLASH_HANDLE_IS_NULL");
            break;
         case SYSTEM__ERR_LOAD_CEVA_FAIL_WRONG_CEVA_ID:
            strcat(stringP, "SYSTEM__ERR_LOAD_CEVA_FAIL_WRONG_CEVA_ID");
            break;
         case SYSTEM__ERR_INCOMPITABLE_VERSION_ID:
            strcat(stringP, "SYSTEM__ERR_INCOMPITABLE_VERSION_ID");
            break;


// GPIO errors
         case GPIO__ERR_OPEN_FAIL_NULL_REGS_ADDRESS:
            strcat(stringP, "GPIO__ERR_OPEN_FAIL_NULL_REGS_ADDRESS");
            break;
         case GPIO__ERR_OPEN_FAIL_NULL_PARAMS:
            strcat(stringP, "GPIO__ERR_OPEN_FAIL_NULL_PARAMS");
            break;
         case GPIO__ERR_OPEN_FAIL_DRV_NOT_CLOSED:
            strcat(stringP, "GPIO__ERR_OPEN_FAIL_DRV_NOT_CLOSED");
            break;
         case GPIO__ERR_IOCTL_FAIL_WRONG_HANDLE:
            strcat(stringP, "GPIO__ERR_IOCTL_FAIL_WRONG_HANDLE");
            break;
         case GPIO__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE:
            strcat(stringP, "GPIO__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE");
            break;
         case GPIO__ERR_IOCTL_FAIL_DRV_NOT_OPEN:
            strcat(stringP, "GPIO__ERR_IOCTL_FAIL_DRV_NOT_OPEN");
            break;
         case GPIO__ERR_INIT_FAIL_WRONG_PAL_HANDLE:
            strcat(stringP, "GPIO__ERR_INIT_FAIL_WRONG_PAL_HANDLE");
            break;
         case GPIO__ERR_GPIO_NUM_EXCEED_MAX:
            strcat(stringP, "GPIO__ERR_GPIO_NUM_EXCEED_MAX");
            break;

// SPI module errors
         case SPI__ERR_ISPI_CLOCK_LOCK_FAIL:
            strcat(stringP, "SPI__ERR_ISPI_CLOCK_LOCK_FAIL");
            break;
         case SPI__ERR_ISPI_TX_FAIL:
            strcat(stringP, "SPI__ERR_ISPI_TX_FAIL");
            break;
         case SPI__ERR_ISPI_RX_FAIL:
            strcat(stringP, "SPI__ERR_ISPI_RX_FAIL");
            break;

// SPI FLASH module errors
         case SPI_FLASH__ERR_FLASH_STATUS_READ_FAIL:
            strcat(stringP, "SPI_FLASH__ERR_FLASH_STATUS_READ_FAIL");
            break;
         case SPI_FLASH__ERR_FLASH_WIP_ITR_FAIL:
            strcat(stringP, "SPI_FLASH__ERR_FLASH_WIP_ITR_FAIL");
            break;
         case SPI_FLASH__ERR_FLASH_INVALID_PARAMS:
            strcat(stringP, "SPI_FLASH__ERR_FLASH_INVALID_PARAMS");
            break;
         case SPI_FLASH__ERR_OPEN_FAIL_NULL_REGS_ADDRESS:
            strcat(stringP, "SPI_FLASH__ERR_OPEN_FAIL_NULL_REGS_ADDRESS");
            break;
         case SPI_FLASH__ERR_OPEN_FAIL_NULL_PARAMS:
            strcat(stringP, "SPI_FLASH__ERR_OPEN_FAIL_NULL_PARAMS");
            break;
         case SPI_FLASH__ERR_OPEN_FAIL_DRV_NOT_CLOSED:
            strcat(stringP, "SPI_FLASH__ERR_OPEN_FAIL_DRV_NOT_CLOSED");
            break;
         case SPI_FLASH__ERR_IOCTL_FAIL_WRONG_HANDLE:
            strcat(stringP, "SPI_FLASH__ERR_IOCTL_FAIL_WRONG_HANDLE");
            break;
         case SPI_FLASH__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE:
            strcat(stringP, "SPI_FLASH__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE");
            break;
         case SPI_FLASH__ERR_IOCTL_FAIL_DRV_NOT_OPEN:
            strcat(stringP, "SPI_FLASH__ERR_IOCTL_FAIL_DRV_NOT_OPEN");
            break;
         case SPI_FLASH__ERR_INIT_FAIL_WRONG_PAL_HANDLE:
            strcat(stringP, "SPI_FLASH__ERR_INIT_FAIL_WRONG_PAL_HANDLE");
            break;
         case SPI_FLASH__ERR_GPIO_NUM_EXCEED_MAX:
            strcat(stringP, "SPI_FLASH__ERR_GPIO_NUM_EXCEED_MAX");
            break;

// sensors manager module errors
         case SENSORS_MNGR__ERR_RESET_FAIL:
            strcat(stringP, "SENSORS_MNGR__ERR_RESET_FAIL");
            break;
         case SENSORS_MNGR__ERR_START_FAIL:
            strcat(stringP, "SENSORS_MNGR__ERR_START_FAIL");
            break;
         case SENSORS_MNGR__ERR_INIT_FAIL:
            strcat(stringP, "SENSORS_MNGR__ERR_INIT_FAIL");
            break;
         case SENSORS_MNGR__ERR_WRONG_FORMAT:
            strcat(stringP, "SENSORS_MNGR__ERR_WRONG_FORMAT");
            break;
         case SENSORS_MNGR__ERR_CFG_FAIL_WRONG_ID:
            strcat(stringP, "SENSORS_MNGR__ERR_CFG_FAIL_WRONG_ID");
            break;
         case SENSORS_MNGR__ERR_FAIL_OPEN_SENSOR_DRV:
            strcat(stringP, "SENSORS_MNGR__ERR_FAIL_OPEN_SENSOR_DRV");
            break;
         case SENSORS_MNGR__ERR_SENSOR_CFG_FAIL_INVALID_RES:
            strcat(stringP, "SENSORS_MNGR__ERR_SENSOR_CFG_FAIL_INVALID_RES");
            break;

// IAE manager module errors
         case IAE_MNGR__ERR_FAIL_TO_SEND_INT_MSG:
            strcat(stringP, "IAE_MNGR__ERR_FAIL_TO_SEND_INT_MSG");
            break;

// DPE manager module errors
         case DPE_MNGR__ERR_FAIL_TO_SEND_INT_MSG:
            strcat(stringP, "DPE_MNGR__ERR_FAIL_TO_SEND_INT_MSG");
            break;

// CPE manager module errors
         case CDE_MNGR__ERR_FAIL_TO_SEND_INT_MSG:
            strcat(stringP, "CDE_MNGR__ERR_FAIL_TO_SEND_INT_MSG");
            break;
         case CDE_MNGR__ERR_WRONG_DIMENSIONS:
            strcat(stringP, "CDE_MNGR__ERR_WRONG_DIMENSIONS");
            break;
         case CDE_MNGR__ERR_UPDATE_STREAMS_FAIL:
            strcat(stringP, "CDE_MNGR__ERR_UPDATE_STREAMS_FAIL");
            break;

// GME manager module errors
         case GME_MNGR__ERR_WRONG_CEVA_ID:
            strcat(stringP, "GME_MNGR__ERR_WRONG_CEVA_ID");
            break;

// MEM manager module errors
         case MEM_MNGR__ERR_INIT_FAIL_MEM_FAIL:
            strcat(stringP, "MEM_MNGR__ERR_INIT_FAIL_MEM_FAIL");
            break;
         case MEM_MNGR__ERR_OPEN_FAIL_MEM_NOT_CLOSED:
            strcat(stringP, "MEM_MNGR__ERR_OPEN_FAIL_MEM_NOT_CLOSED");
            break;
         case MEM_MNGR__ERR_OPEN_FAIL_MEM_DDR_WRONG_ADDR:
            strcat(stringP, "MEM_MNGR__ERR_OPEN_FAIL_MEM_DDR_WRONG_ADDR");
            break;
         case MEM_MNGR__ERR_OPEN_FAIL_MEM_DDR_NULL_SIZE:
            strcat(stringP, "MEM_MNGR__ERR_OPEN_FAIL_MEM_DDR_NULL_SIZE");
            break;
         case MEM_MNGR__ERR_OPEN_FAIL_MEM_IAE_WRONG_ADDR:
            strcat(stringP, "MEM_MNGR__ERR_OPEN_FAIL_MEM_IAE_WRONG_ADDR");
            break;
         case MEM_MNGR__ERR_OPEN_FAIL_MEM_DPE_WRONG_ADDR:
            strcat(stringP, "MEM_MNGR__ERR_OPEN_FAIL_MEM_DPE_WRONG_ADDR");
            break;
         case MEM_MNGR__ERR_MEM_BUF_CLOSE_FAIL:
            strcat(stringP, "MEM_MNGR__ERR_MEM_BUF_CLOSE_FAIL");
            break;
         case MEM_MNGR__ERR_MEM_BUF_OPEN_FAIL:
            strcat(stringP, "MEM_MNGR__ERR_MEM_BUF_OPEN_FAIL");
            break;
            //proj drv errors
         case PROJ__ERR_PROJ_NUM_NOT_SUPPORTED:
            strcat(stringP, "PROJECTOR__ERR_PROJ_NUM_NOT_SUPPORTED");
         break;
//Calibration module errors
       case CALIBRATION__ERR_MISSING_FIELDS_IN_INI_FILE:
          strcat(stringP, "CALIBRATION__ERR_MISSING_FIELDS_IN_INI_FILE");
          break;


// unknown error code
         default:
            strcat(stringP, "INVALID");
            break;
      }
   }
#endif
   return(stringP);
}

/****************************************************************************
 ***************      G L O B A L     F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: ERRG_printErrLog
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns:
*
*  Context: error handling
*
****************************************************************************/
void INU_STRG_getErrString(ERRG_codeE err, char *stringP)
{
   char processorString[INU_STRP_MAX_STRING_LENGTH] = {0};
   char moduleString[INU_STRP_MAX_STRING_LENGTH] = {0};
   char errString[2*INU_STRP_MAX_STRING_LENGTH] = {0};
   INU_STRP_getProcessorString(err, processorString);
   INU_STRP_getModuleString(err, moduleString);
   INU_STRP_getErrString(err, errString);
   sprintf(stringP, "INU ERROR code: 0x%X, ", err);
   strcat (stringP, processorString);
   strcat (stringP, moduleString);
   strcat (stringP, errString);
}

char *INU_STRG_cmd2String(UINT32 command, char *stringP)
{
   INUG_ioctlCmdE cmd = (INUG_ioctlCmdE)command;

   switch(cmd)
   {
      case INU_CMDG_FREE_BUF_E:
         strcat(stringP, "INU_CMDG_FREE_BUF_E");
         break;
      case INUG_IOCTL_SYSTEM_RD_REG_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_RD_REG_E");
         break;
      case INUG_IOCTL_SYSTEM_WR_REG_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_WR_REG_E");
         break;
      case INUG_IOCTL_SYSTEM_GET_VERSION_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_GET_VERSION_E");
         break;
      case INUG_IOCTL_SYSTEM_PROJECTOR_SET_MODE_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_PROJECTOR_SET_MODE_E");
         break;
      case INUG_IOCTL_SYSTEM_PROJECTOR_GET_MODE_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_PROJECTOR_GET_MODE_E");
         break;
      case INUG_IOCTL_SYSTEM_GET_MODE_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_GET_MODE_E");
         break;
      case INUG_IOCTL_SYSTEM_SET_MODE_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_SET_MODE_E");
         break;
      case INUG_IOCTL_SYSTEM_UART2LOOPBACK_PROBE_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_UART2LOOPBACK_PROBE_E");
         break;
      case INUG_IOCTL_SYSTEM_DB_CONTROL_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_DB_CONTROL_E");
         break;
      case INUG_IOCTL_SYSTEM_POWER_MODE_E:
         strcat(stringP, "INUG_IOCTL_POWER_MODE_E");
         break;
      case INUG_IOCTL_SYSTEM_GET_TIME_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_GET_TIME_E");
         break;
      case INUG_IOCTL_SYSTEM_GET_RTC_LOCK_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_GET_RTC_LOCK_E");
         break;
      case INUG_IOCTL_SYSTEM_GET_STATS_E:
         strcat(stringP, "INUG_IOCTL_SYSTEM_GET_STATS_E");
         break;
      case INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG:
         strcat(stringP, "INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG");
         break;
      case INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG:
         strcat(stringP, "INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG");
         break;
      case INUG_IOCTL_LOGGER_SET_PARAMS_E:
         strcat(stringP, "INUG_IOCTL_LOGGER_SET_PARAMS_E");
         break;
      case INUG_IOCTL_LOGGER_GET_PARAMS_E:
         strcat(stringP, "INUG_IOCTL_LOGGER_GET_PARAMS_E");
         break;
      default :
         break;
   }

   return(stringP);
}

char *INU_STRG_revisionId2String(inu_device__chipRevisionIdE revision, char *stringP)
{

   switch(revision)
   {
      case INU_DEVICE__CHIP_REVISION_NU3000_A_E:
         strcat(stringP, "CHIP_REVISION_NU3000");
         break;
      case INU_DEVICE__CHIP_REVISION_NU3000_B_E:
         strcat(stringP, "CHIP_REVISION_NU3000");
         break;
      case INU_DEVICE__CHIP_REVISION_NU4000_E:
         strcat(stringP, "CHIP_REVISION_NU4000");
         break;
      case INU_DEVICE__CHIP_REVISION_NU4100_E:
         strcat(stringP, "CHIP_REVISION_NU4100");
         break;
      default:
         strcat(stringP, "CHIP_REVISION_NU3000");
         break;
   }

   return(stringP);
}

char *INU_STRG_versionId2String(inu_device__chipVersionIdIdE version, char *stringP)
{

   switch(version)
   {
      case INU_DEVICE__CHIP_VERSION_A_E:
         strcat(stringP, "CHIP_VERSION_A");
         break;
      case INU_DEVICE__CHIP_VERSION_B_E:
         strcat(stringP, "CHIP_VERSION_B");
         break;
      case INU_DEVICE__CHIP_VERSION_C_E:
         strcat(stringP, "CHIP_VERSION_C");
         break;         
      default:
         strcat(stringP, "CHIP_VERSION_A");
         break;
   }

   return(stringP);
}

