cmake_minimum_required(VERSION 2.6)
project(flasher)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions("-DLINUX -Wall -Werror")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

add_subdirectory (flasher)
