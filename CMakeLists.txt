# CMakeLists.txt
# --------------------------------------------
# Este archivo le dice a CMake cómo construir el proyecto.
# --------------------------------------------

# Nota: Cada ves que se modifique este archivo, dar a Delete Cache y volver a configurar el proyecto.

# Versión mínima requerida de CMake
cmake_minimum_required(VERSION 3.16)

# Nombre del proyecto
# Este nombre también puede usarse como base para el ejecutable o variables.
project(FractalJulia)

# Busca e importa la librería 'fmt'.
# 'find_package' localiza el paquete ya instalado en el sistema
# o en vcpkg. 'CONFIG REQUIRED' indica que debe existir.
find_package(fmt CONFIG REQUIRED)

# Añade soporte para instrucciones AVX2
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")

# Busca e importa la librería 'SFML' con sus componentes.
# Aquí se incluyen los módulos que el programa necesita:
# Network, Graphics, Window, Audio y System.
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

# Crea un ejecutable llamado "fractal_julia"
# A partir del archivo fuente main.cpp.
add_executable(fractal_julia main.cpp
               palette.cpp
               fractal_serial.cpp
               fractal_simd.cpp
)

# Enlaza las librerías externas al ejecutable.
# 'target_link_libraries' conecta el ejecutable con las
# dependencias requeridas para compilar y ejecutar correctamente.
target_link_libraries(fractal_julia PRIVATE 
    fmt::fmt
    SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System)
