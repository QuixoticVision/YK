/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "storage.h"

int write_config(const char *name, data *data);
int read_config(const char *nmae, data *buff);
int delete_config(const char *name);
