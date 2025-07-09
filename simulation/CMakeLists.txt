cmake_minimum_required(VERSION 3.10)
project(freeRTOS_simulation C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/config/stubs
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/portable/Common
)

set(FREERTOS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../freeRTOS_OS/FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_4.c
    # Config files 
    ${CMAKE_CURRENT_SOURCE_DIR}/config/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config/stubs/hooks.c
)

# Add executables
add_executable(device_system
    src/main.c
    src/device1.c
    src/device2.c
    ${FREERTOS_SOURCES}
)

# for heap debugging
target_compile_definitions(device_system PRIVATE configUSE_MALLOC_FAILED_HOOK=1)

if(WIN32)
    target_link_libraries(device_system ws2_32)
    set_target_properties(device_system PROPERTIES LINK_FLAGS "--enable-stdcall-fixup")
endif()