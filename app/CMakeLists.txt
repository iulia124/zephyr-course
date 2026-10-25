cmake_minimum_required(VERSION 3.20.0)

list(APPEND EXTRA_ZEPHYR_MODULES
     ${CMAKE_CURRENT_SOURCE_DIR}/../modules/iulia_led_sensor)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(l6_led_sensor)

target_sources(app PRIVATE src/main.c)