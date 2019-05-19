cmake_minimum_required(VERSION 2.6)
project(flasher)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions("-DLINUX -Wall -Werror")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

if(WIN32)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory (dcdate)
add_subdirectory (flasher)
add_subdirectory (libmmc)
add_subdirectory (libcmdh)










