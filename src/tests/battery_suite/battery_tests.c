//
// Created by jack on 10/15/20.
//

#include "battery_tests.h"
#include "battery.h"

#include "unity.h"

void test_soc_calculation(void)
{
    TEST_ASSERT_EQUAL_HEX8(50, func());
}

