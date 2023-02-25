/**
 * @file dict.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-24
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "dict.h"
#include "stdlib.h"
#include <string.h>

dict_t **dict_alloc(void)
{
    return malloc(sizeof(dict_t));
}

void dict_dealloc(dict_t *dict)
{
    dict_t *ptr;
    for (ptr = dict; ptr != NULL; ptr = ptr->next)
    {
        free(ptr);
    }
}

int dict_size(dict_t *dict)
{
    int size = 0;

    dict_t *ptr;
    for (ptr = dict; ptr != NULL; ptr = ptr->next)
    {
        size++;
    }

    return size;
}

void *dict_getItem(dict_t *dict, char *key)
{
    dict_t *ptr;

    for (ptr = dict; ptr != NULL; ptr = ptr->next)
    {
        if (strcmp(ptr->key, key) == 0)
        {
            return ptr->value;
        }
    }

    return NULL;
}

void dict_delItem(dict_t **dict, char *key)
{
    dict_t *ptr, *prev;

    for (ptr = *dict, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next)
    {
        if (strcmp(ptr->key, key) == 0)
        {
            if (ptr->next != NULL)
            {
                if (prev == NULL)
                {
                    *dict = ptr->next;
                }
                else
                {
                    prev->next = ptr->next;
                }
            }
            else if (prev != NULL)
            {
                prev->next = NULL;
            }
            else
            {
                *dict = NULL;
            }

            free(ptr->key);
            free(ptr->value);
            free(ptr);
            ptr = NULL;
            return;
        }
    }
}

void dict_addItem(dict_t **dict, char *key, void *value)
{
    dict_delItem(dict, key); /* If we already have a item with this key, delete it. */
    dict_t *d = malloc(sizeof(struct dict_t_struct));
    d->key = malloc(strlen(key) + 1);
    strcpy(d->key, key);
    d->value = value;
    d->next = *dict;
    *dict = d;
}

dict_t **dict_invert(dict_t *dict)
{
    dict_t **invert = dict_alloc();

    dict_t *ptr;
    for (ptr = dict; ptr != NULL; ptr = ptr->next)
    {
        dict_addItem(invert, ptr->value, ptr->key);
    }

    return invert;
}