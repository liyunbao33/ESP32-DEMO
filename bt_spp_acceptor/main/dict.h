/**
 * @file dict.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __DICT_H__
#define __DICT_H__

typedef struct dict_t_struct
{
    char *key;
    void *value;
    struct dict_t_struct *next;
} dict_t;

dict_t **dict_alloc(void);
void dict_dealloc(dict_t *dict);
void *dict_getItem(dict_t *dict, char *key);
void dict_delItem(dict_t **dict, char *key);
void dict_addItem(dict_t **dict, char *key, void *value);
dict_t **dict_invert(dict_t *dict);


#endif
