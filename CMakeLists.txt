cmake_minimum_required(VERSION 3.15)
project(MyGTK4App C)

set(CMAKE_C_STANDARD 11)

# Utiliser pkg-config pour GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Inclure les répertoires d’en-têtes GTK4
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

# Fichier source principal
add_executable(MyGTK4App main.c)

# Lier les bibliothèques GTK4
target_link_libraries(MyGTK4App ${GTK4_LIBRARIES})
