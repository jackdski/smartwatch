//
// Created by jack on 10/20/20.
//

#include "utils.h"
#include "app_config.h"

#if(PRINT_CONFIG == 0)
#include <stdio.h>
#endif

void _print(char * str)
{
#if(PRINT_CONFIG == 1)
    NRF_LOG_INFO(str);
#else
    printf(str);
#endif
}