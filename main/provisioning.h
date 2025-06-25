#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*service_name_func_t)(char *service_name);

void provisioning_setup(service_name_func_t show_service_name, bool force_provisioning);
void get_device_service_name(char *service_name, size_t max);

#ifdef __cplusplus
}
#endif