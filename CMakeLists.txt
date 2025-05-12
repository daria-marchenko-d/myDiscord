cmake_minimum_required(VERSION 3.10)
project(MyDiscord C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS} include)
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(mydiscord_client
    src/main.c
    src/gui.c
    src/network.c
    src/database.c
    src/crypto.c
    src/utils.c
)

add_executable(mydiscord_server
    server_main/main_server.c
    src/server.c
    src/database.c
    src/crypto.c
    src/utils.c
)

target_link_libraries(mydiscord_client
    ${GTK4_LIBRARIES}
    pthread
    sqlite3
    ws2_32
)

target_link_libraries(mydiscord_server
    pthread
    sqlite3
    ws2_32
)
