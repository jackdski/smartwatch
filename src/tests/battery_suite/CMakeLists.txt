add_executable(battery_suite_app
        battery_tests.c
    )

target_link_libraries(battery_suite_app
        Unity
    )

add_test(battery_test battery_suite_app)
