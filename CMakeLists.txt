cmake_minimum_required(VERSION 3.15)
project(mydiscord-c C)

set(CMAKE_C_STANDARD 99)

# Forcer l'utilisation de gcc de MSYS2 mingw64
if(WIN32)
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
endif()

# Options UTF-8 pour éviter les soucis d'encodage
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# === GTK3 (via pkg-config) ===
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# === PostgreSQL (libpq) ===
# Attention : adapte ce chemin si ton libpq est ailleurs
include_directories("C:/msys64/mingw64/include")
link_directories("C:/msys64/mingw64/lib")

# OpenSSL
find_package(OpenSSL REQUIRED)

# === Server ===
file(GLOB SERVER_SOURCES "server/src/*.c")
add_executable(server ${SERVER_SOURCES})
include_directories(server/include)

# Forcer le mode console (sinon erreur "undefined reference to WinMain")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-subsystem,console")

# Lier les bibliothèques
target_link_libraries(server PRIVATE pq ws2_32 ${GTK3_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
