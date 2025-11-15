#ifndef FRACTAL_SERIAL_H // evita duplicidad de inclusión
#define FRACTAL_SERIAL_H

#pragma once

#include <cstdint>

// Declara la función que genera el fractal en modo serial.
// Parámetros:
//  x_min, y_min, x_max, y_max : límites del rectángulo complejo a muestrear
//  width, height             : dimensiones del buffer de píxeles
//  pixel_buffer              : puntero a un arreglo de uint32_t donde se escribe el color de cada píxel
void julia_serial_1(double x_min,double y_min, double x_max,
                     double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);

void julia_serial_2(double x_min,double y_min, double x_max,
                     double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);                     

#endif // FRACTAL_SERIAL_H