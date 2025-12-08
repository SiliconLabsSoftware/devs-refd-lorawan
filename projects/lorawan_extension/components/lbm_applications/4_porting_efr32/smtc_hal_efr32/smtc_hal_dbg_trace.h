/***************************************************************************//**
 * @file hal_dbg_trace.h
 * @brief LBM Debug Trace Header File
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SMTC_HAL_DBG_TRACE_H
#define SMTC_HAL_DBG_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "smtc_hal_trace.h"
#include "smtc_modem_api.h"
#include "app_log.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/*!
 * @brief Stringify constants
 */
#define xstr(a) str(a)
#define str(a) #a

#define HAL_FEATURE_OFF                             0
#define HAL_FEATURE_ON                              !HAL_FEATURE_OFF

#ifndef HAL_DBG_TRACE
#define HAL_DBG_TRACE                               HAL_FEATURE_ON
#endif

#ifndef HAL_DBG_TRACE_COLOR
#define HAL_DBG_TRACE_COLOR                         HAL_FEATURE_OFF
#endif

#ifndef HAL_DBG_TRACE_RP
#define HAL_DBG_TRACE_RP                            HAL_FEATURE_OFF
#endif

#if (HAL_DBG_TRACE_COLOR == HAL_FEATURE_ON)
    #define HAL_DBG_TRACE_COLOR_BLACK               "\x1B[0;30m"
    #define HAL_DBG_TRACE_COLOR_RED                 "\x1B[0;31m"
    #define HAL_DBG_TRACE_COLOR_GREEN               "\x1B[0;32m"
    #define HAL_DBG_TRACE_COLOR_YELLOW              "\x1B[0;33m"
    #define HAL_DBG_TRACE_COLOR_BLUE                "\x1B[0;34m"
    #define HAL_DBG_TRACE_COLOR_MAGENTA             "\x1B[0;35m"
    #define HAL_DBG_TRACE_COLOR_CYAN                "\x1B[0;36m"
    #define HAL_DBG_TRACE_COLOR_WHITE               "\x1B[0;37m"
    #define HAL_DBG_TRACE_COLOR_DEFAULT             "\x1B[0m"
#else
    #define HAL_DBG_TRACE_COLOR_BLACK   ""
    #define HAL_DBG_TRACE_COLOR_RED     ""
    #define HAL_DBG_TRACE_COLOR_GREEN   ""
    #define HAL_DBG_TRACE_COLOR_YELLOW  ""
    #define HAL_DBG_TRACE_COLOR_BLUE    ""
    #define HAL_DBG_TRACE_COLOR_MAGENTA ""
    #define HAL_DBG_TRACE_COLOR_CYAN    ""
    #define HAL_DBG_TRACE_COLOR_WHITE   ""
    #define HAL_DBG_TRACE_COLOR_DEFAULT ""
#endif

#if (HAL_DBG_TRACE)

  #define SMTC_HAL_TRACE_PRINTF(...)      app_log_debug(__VA_ARGS__)
  #define SMTC_HAL_TRACE_MSG(msg)                                    \
  do                                                                 \
  {                                                                  \
    SMTC_HAL_TRACE_PRINTF("%s%s", HAL_DBG_TRACE_COLOR_DEFAULT, msg); \
  } while (0)
  #define SMTC_HAL_TRACE_MSG_COLOR(msg, color)                                \
  do                                                                          \
  {                                                                           \
    SMTC_HAL_TRACE_PRINTF("%s%s%s", color, msg, HAL_DBG_TRACE_COLOR_DEFAULT); \
  } while (0)
  #define SMTC_HAL_TRACE_INFO(...)                                         \
  do                                                                       \
  {                                                                        \
    SMTC_HAL_TRACE_PRINTF(HAL_DBG_TRACE_COLOR_GREEN "INFO: " __VA_ARGS__); \
  } while (0)
  #define SMTC_HAL_TRACE_WARNING(...)                                       \
  do                                                                        \
  {                                                                         \
    SMTC_HAL_TRACE_PRINTF(HAL_DBG_TRACE_COLOR_YELLOW "WARN: " __VA_ARGS__); \
  } while (0)
  #define SMTC_HAL_TRACE_ERROR(...)                                       \
  do                                                                      \
  {                                                                       \
    SMTC_HAL_TRACE_PRINTF(HAL_DBG_TRACE_COLOR_RED "ERROR: " __VA_ARGS__); \
  } while (0)
  #define SMTC_HAL_TRACE_ARRAY(msg, array, len)                        \
  do                                                                   \
  {                                                                    \
    SMTC_HAL_TRACE_PRINTF("%s - (%lu bytes):\n", msg, (uint32_t)len);  \
    for (uint32_t i = 0; i < (uint32_t)len; i++) {                     \
      if (((i % 16) == 0) && (i > 0)) { SMTC_HAL_TRACE_PRINTF("\n"); } \
      SMTC_HAL_TRACE_PRINTF(" %02X", array[i]);                        \
    }                                                                  \
    SMTC_HAL_TRACE_PRINTF("\n");                                       \
  } while (0)
  #define SMTC_HAL_TRACE_PACKARRAY(msg, array, len)                                           \
  do                                                                                          \
  {                                                                                           \
    for (uint32_t i = 0; i < (uint32_t)len; i++) { SMTC_HAL_TRACE_PRINTF("%02X", array[i]); } \
  } while (0)
#else
// When HAL_DBG_TRACE is disabled, define empty macros
  #define SMTC_HAL_TRACE_PRINTF(...)
  #define SMTC_HAL_TRACE_MSG(msg)
  #define SMTC_HAL_TRACE_MSG_COLOR(msg, color)
  #define SMTC_HAL_TRACE_INFO(...)
  #define SMTC_HAL_TRACE_WARNING(...)
  #define SMTC_HAL_TRACE_ERROR(...)
  #define SMTC_HAL_TRACE_ARRAY(msg, array, len)
  #define SMTC_HAL_TRACE_PACKARRAY(msg, array, len)
#endif

/*!
 * @brief Helper macro that returned a human-friendly message if a command does not return SMTC_MODEM_RC_OK
 *
 * @remark The macro is implemented to be used with functions returning a @ref smtc_modem_return_code_t
 *
 * @param[in] rc  Return code
 */

#define ASSERT_SMTC_MODEM_RC(rc_func)                                                    \
  do                                                                                     \
  {                                                                                      \
    smtc_modem_return_code_t rc = rc_func;                                               \
    if ( rc == SMTC_MODEM_RC_NOT_INIT )                                                  \
    {                                                                                    \
      SMTC_HAL_TRACE_ERROR("In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                           xstr(SMTC_MODEM_RC_NOT_INIT) );                               \
    }                                                                                    \
    else if ( rc == SMTC_MODEM_RC_INVALID )                                              \
    {                                                                                    \
      SMTC_HAL_TRACE_ERROR("In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                           xstr(SMTC_MODEM_RC_INVALID) );                                \
    }                                                                                    \
    else if ( rc == SMTC_MODEM_RC_BUSY )                                                 \
    {                                                                                    \
      SMTC_HAL_TRACE_ERROR("In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                           xstr(SMTC_MODEM_RC_BUSY) );                                   \
    }                                                                                    \
    else if ( rc == SMTC_MODEM_RC_FAIL )                                                 \
    {                                                                                    \
      SMTC_HAL_TRACE_ERROR("In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                           xstr(SMTC_MODEM_RC_FAIL) );                                   \
    }                                                                                    \
    else if ( rc == SMTC_MODEM_RC_NO_TIME )                                              \
    {                                                                                    \
      SMTC_HAL_TRACE_WARNING("In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__, \
                             xstr(SMTC_MODEM_RC_NO_TIME) );                              \
    }                                                                                    \
    else if ( rc == SMTC_MODEM_RC_INVALID_STACK_ID )                                     \
    {                                                                                    \
      SMTC_HAL_TRACE_ERROR("In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                           xstr(SMTC_MODEM_RC_INVALID_STACK_ID) );                       \
    }                                                                                    \
    else if ( rc == SMTC_MODEM_RC_NO_EVENT )                                             \
    {                                                                                    \
      SMTC_HAL_TRACE_INFO("In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,    \
                          xstr(SMTC_MODEM_RC_NO_EVENT) );                                \
    }                                                                                    \
  } while ( 0 )

// ----------------------------------------------------------------------------
//                                Global Variables
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif  // SMTC_HAL_DBG_TRACE_H
