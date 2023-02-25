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

/**
  * @brief  Gets the number of elements in an array
  * @param  arr: Array name (any type)
  * @retval The number of elements in this array
  */
#define __Sizeof(arr) (sizeof(arr)/sizeof(arr[0]))

#endif