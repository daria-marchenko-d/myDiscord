cmake_minimum_required(VERSION 3.15)
project(MyGTK4App C)

set(CMAKE_C_STANDARD 99)

# Options UTF-8 pour éviter les soucis d'encodage
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# === GTK4 ===
find_package(GTK4 REQUIRED)

# === PostgreSQL (libpq) ===
find_package(PostgreSQL REQUIRED)

# OpenSSL
find_package(OpenSSL REQUIRED)

# === Server ===
file(GLOB SERVER_SOURCES "server/src/*.c")
add_executable(server ${SERVER_SOURCES})
include_directories(server/include)

# Lier les bibliothèques pour le serveur
target_link_libraries(server PRIVATE pq OpenSSL::SSL OpenSSL::Crypto)

# === Client ===
add_executable(MyGTK4App
    client/main.c
    client/ui.c
    client/ui_callbacks.c # Ajoute ui_callbacks.c
)

target_include_directories(MyGTK4App PRIVATE
    ${GTK4_INCLUDE_DIRS}
)

target_link_libraries(MyGTK4App
    ${GTK4_LIBRARIES}
)

# === Configuration spécifique à Windows (si nécessaire) ===
if(WIN32)
    # Définir les options de compilation spécifiques à Windows
    add_definitions(-D_WIN32)

    # Ajouter les chemins d'inclusion spécifiques à Windows (si nécessaire)
    # include_directories("C:/msys64/mingw64/include")

    # Ajouter les bibliothèques spécifiques à Windows (si nécessaire)
    # link_directories("C:/msys64/mingw64/lib")

    # Lier les bibliothèques supplémentaires spécifiques à Windows (si nécessaire)
    # target_link_libraries(MyGTK4App gdi32)
endif()
