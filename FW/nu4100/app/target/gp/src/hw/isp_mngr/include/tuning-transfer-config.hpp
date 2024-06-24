/******************************************************************************\
|* Copyright (c) <2021> by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")     *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets       *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

//! Powered by Yong Yang

#pragma once

#include <tuning-common/common.hpp>

#define PROJECT_NAME_TUNING_TRANSFER "tuning-transfer"
#define PROJECT_VERSION_TUNING_TRANSFER "0.1.0"
#define PROJECT_TIME_TUNING_TRANSFER "2022-10-12 11-28-11"

#if !defined(CTRL_HTTP)
/* #undef CTRL_HTTP */
#endif // CTRL_HTTP

#if !defined(CTRL_UART)
#define CTRL_UART
#endif // CTRL_UART

#if !defined(CTRL_USB)
/* #undef CTRL_USB */
#endif // CTRL_USB

#if !defined(CTRL_HTTP) && !defined(CTRL_UART) && !defined(CTRL_USB)
#error "Not open any supported transfer protocol"
#endif
