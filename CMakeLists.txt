cmake_minimum_required(VERSION 3.10)
project(freeRTOS_simulation C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/headers/stubs
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/include
)

# Source files for FreeRTOS core
set(FREERTOS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/headers/stubs/win_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/headers/stubs/hooks.c  # ADDED HOOKS
)

# Add executables
add_executable(device1_sim
    src/device1.c
    ${FREERTOS_SOURCES}
)

# Link libraries
if(WIN32)
    target_link_libraries(device1_sim ws2_32)
endif()