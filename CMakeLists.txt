cmake_minimum_required(VERSION 3.15)
project(mydiscord-c C)

set(CMAKE_C_STANDARD 99)

# Compiler configuration
if(WIN32)
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
include_directories(${GTK4_INCLUDE_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

# Database and crypto
find_package(OpenSSL REQUIRED)
find_library(PQ_LIBRARY pq REQUIRED HINTS "C:/msys64/mingw64/lib")
include_directories("C:/msys64/mingw64/include") # Ensure include directory is present

# Project structure
include_directories(
    ${CMAKE_SOURCE_DIR}/client/include
    ${CMAKE_SOURCE_DIR}/server/include
)

# Client executable
add_executable(client
    client/src/main.c
    client/src/ui.c
    server/src/db.c # <--- ДОДАНО ЦЕЙ РЯДОК
)

target_link_libraries(client
    ${GTK4_LIBRARIES}
    ${PQ_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Server executable (optional)
if(EXISTS "${CMAKE_SOURCE_DIR}/server/src/server.c")
    add_executable(server
        server/src/server.c
        server/src/db.c
    )
    target_link_libraries(server
        ${PQ_LIBRARY}
        ws2_32
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()
