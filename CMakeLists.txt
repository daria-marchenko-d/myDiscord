cmake_minimum_required(VERSION 3.15)
project(MyGTK4App C)

set(CMAKE_C_STANDARD 99)

# Options UTF-8 pour éviter les soucis d'encodage
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# === GTK4 ===
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})


# === PostgreSQL (libpq) ===
find_package(PostgreSQL REQUIRED)
if(NOT PostgreSQL_FOUND)
    message(FATAL_ERROR "PostgreSQL not found")
endif()

# === Ici Condition Windows pour PostgreSQL ===
if(WIN32)
    # Chemins spécifiqus à Windows (à adapter slon ton installation)
    include_directories("C:/Program Files/PostgreSQL/<version>/include")
    link_directories("C:/Program Files/PostgreSQL/<version>/lib")
else()
    # Chemins pour macOs (ou autres systèmes non-Windows)    
    include_directories(/opt/homebrew/opt/libpq/include)
    link_directories(/opt/homebrew/opt/libpq/lib)
endif()

# OpenSSL
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found")
endif()

# === Server ===
file(GLOB SERVER_SOURCES "server/src/*.c")
add_executable(server ${SERVER_SOURCES})
include_directories(server/include)
target_link_libraries(server PRIVATE pq OpenSSL::SSL OpenSSL::Crypto)

# === Client ===
#file(GLOB CLIENT_SOURCES "client/*.c") # Recherche automatique des sources

#add_executable(MyGTK4App ${CLIENT_SOURCES})

#include_directories(client/include ${GTK4_INCLUDE_DIRS}) # Ajoute client/include

#target_link_libraries(MyGTK4App PRIVATE PkgConfig::GTK4)

# === Configuration spécifique à Windows (si nécessaire) ===
if(WIN32)
    # Définir les options de compilation spécifiques à Windows
    add_definitions(-D_WIN32)

    # Ajouter les chemins d'inclusion spécifiques à Windows (si nécessaire)
    # set(CMAKE_INCLUDE_PATH "C:/msys64/mingw64/include;${CMAKE_INCLUDE_PATH}")

    # Ajouter les bibliothèques spécifiques à Windows (si nécessaire)
    # set(CMAKE_LIBRARY_PATH "C:/msys64/mingw64/lib;${CMAKE_LIBRARY_PATH}")

    # Lier les bibliothèques supplémentaires spécifiques à Windows (si nécessaire)
    # target_link_libraries(MyGTK4App gdi32 ws2_32)
endif()

# === Installation ===

#install(TARGETS MyGTK4App DESTINATION bin)
install(TARGETS server DESTINATION bin)
