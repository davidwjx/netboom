/****************************************************************************
 *
 *   FileName: ceva_boot.h
 *
 *   Author: 
 *
 *   Date:
 *
 *   Description:
 *
 ****************************************************************************/
#ifndef CEVA_BOOT_H
#define CEVA_BOOT_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ***************************************************************************/

#define CEVA_BOOTG_BINARY_FILE_NAME            "/media/inuitive/inu_app.cva"

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L       D A T A           ***************
****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
****************************************************************************/

ERRG_codeE CEVA_BOOTG_getCodeMemory();
ERRG_codeE CEVA_BOOTG_getDataMemory();
ERRG_codeE CEVA_BOOTG_loadCeva();
ERRG_codeE CEVA_BOOTG_verify( void );
ERRG_codeE CEVA_BOOTG_sleep( void );
ERRG_codeE CEVA_BOOTG_wakeup( void );
UINT32 CEVA_BOOTG_cevaIsActive();


#ifdef __cplusplus
}
#endif

#endif //	CEVA_INIT_H

