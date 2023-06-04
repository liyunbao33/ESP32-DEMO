/**
 * @file filter_algorithm.c
 * @author liyunbao (liyunbao33@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "filter_algorithm.h"
#include "stdbool.h"

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

/**
 * @brief first order filter
 * 
 * @param fac filter factor
 * @param value  Value to be filtered
 * @return int32_t filter value
 */
#define FAC (0.3)
float FirstOrderFilter(int32_t value)
{
    volatile static float last_filter_value = 0;
    float filter_value;

    __ExecuteOnce(last_filter_value = value);

    filter_value = FAC*value + (1 - FAC)*last_filter_value;
    last_filter_value = filter_value;
    return filter_value;
}

/**
 * @brief bubble sort algorithm
 * 
 * @param arr array
 * @param len array length
 */
static void bubble_sort(int32_t arr[], int32_t len)
{
    int i,j,temp;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < len - 1 - i; j++) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j+1];
                arr[j+1] = arr[j];
                arr[j] = temp;
            }
        }
    }
}


/**
 * @brief Arith_AverageFiter
 * 
 * @param value: value to be filtered 
 * @return int32_t: filter value 
 */
#define ARR_NUM 5
int32_t Arith_AverageFiter(int32_t value)
{
    volatile static int32_t filter_value = 0;
    volatile static int32_t filter_cnt = 0;
    static int32_t arr[ARR_NUM];

    __ExecuteOnce(filter_value = value);

    if(filter_cnt < ARR_NUM)  
    {
        filter_cnt++;
        arr[filter_cnt] = value;
    }
    else
    {
        filter_cnt = 0;
        bubble_sort(arr, ARR_NUM);
        for(uint8_t i = 1; i < ARR_NUM - 2; i++)
        {
            filter_value += arr[i];
        }
        filter_value = filter_value / (ARR_NUM - 2);
    }

    return filter_value;
}

/**
 * @brief LadderFiter
 * 
 * @param value value to be filtered 
 * @return int32_t filter value
 */
int32_t LadderFiter(int32_t value)
{
    volatile static int32_t filter_cnt = 0;
    volatile static int32_t total_value = 0;
    volatile static int32_t filter_value = 0;

    __ExecuteOnce(filter_value = value);

    if(filter_cnt < 16)  
    {
        filter_cnt++;
        total_value += value;
        if(filter_cnt >= 16)
        {
            filter_value = (total_value >> 4);
            total_value >>= 1; 
        }
    }
    else if(filter_cnt < 24)
    {
        filter_cnt++;
        total_value += value;
        if(filter_cnt >= 24)
        {
            filter_cnt = 16;
            filter_value = (total_value >> 4);
            total_value >>= 1;
        }
    }

    return filter_value;
}


