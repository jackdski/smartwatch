//
// Created by jack on 10/27/20.
//

// Unit Test Framework
#include "unity.h"
//#include "cmock.h"

#include "battery.h"


// Setup/Teardown
void setUp(void)
{

}

void tearDown(void)
{

}


// helper functions
//uint16_t SGM40562_sample_battery_mv(void)
//{
//    return 1668 ;
//}


//tests

void test_soc_calculation(void)
{
    TEST_ASSERT_TRUE(battery_is_soc_valid(0));
    TEST_ASSERT_TRUE(battery_is_soc_valid(50));
    TEST_ASSERT_TRUE(battery_is_soc_valid(100));
    TEST_ASSERT_FALSE(battery_is_soc_valid(-128));
}

void test_battery_states(void)
{

}


// main
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_soc_calculation);
    return UNITY_END();
}