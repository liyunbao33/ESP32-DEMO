/**
 * @file main.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __MAIN_H__
#define __MAIN_H__
#include <stddef.h>

void bt_spp_on(void);

/**
  * @brief  Gets the number of elements in an array
  * @param  arr: Array name (any type)
  * @retval The number of elements in this array
  */
#define __Sizeof(arr) (sizeof(arr)/sizeof(arr[0]))

/**
 * @brief  the function is executed only once and is often used for initialization
 * @param  func:called function(it can also be an assignment or other statement)
 * @retval not
 */
#define __ExecuteOnce(func)     \
  do                            \
  {                             \
    static bool isInit = false; \
    if (!isInit)                \
    {                           \
      func, isInit = true;      \
    }                           \
  } while (0)

#endif