# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

# FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS
        "main.c"
        "accelerometer.c"
        "led.c"
        "buzzer.c"
        "heartbeat.c"
    INCLUDE_DIRS "."
)
