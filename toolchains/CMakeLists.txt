add_library(Unity STATIC Unity/src/unity.c)

target_include_directories(Unity PUBLIC
        Unity/src
        )


add_library(CMock STATIC cmock/src/cmock.c)

target_include_directories(CMock PUBLIC
        cmock/src
        )