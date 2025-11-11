# CMakeLists.txt
# --------------------------------------------
# Este archivo le dice a CMake cómo construir el proyecto.
# --------------------------------------------

# Versión mínima requerida de CMake
cmake_minimum_required(VERSION 3.16)

# Nombre del proyecto
# Este nombre también puede usarse como base para el ejecutable o variables.
project(FractalJulia)

# Crea un ejecutable llamado "fractal_julia"
# A partir del archivo fuente main.cpp.
add_executable(fractal_julia main.cpp)


