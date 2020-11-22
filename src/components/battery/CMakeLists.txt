#if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
#    add_executable(battery_suite_app
#            test_battery.c
#            battery.c
#            )
#
#    include_directories(components/battery)
#
#    target_link_libraries(battery_suite_app
#            Unity
#    #        CMock
#            )
#
#    add_test(battery_test battery_suite_app)
#else()
#    include_directories(../../drivers)
endif()
