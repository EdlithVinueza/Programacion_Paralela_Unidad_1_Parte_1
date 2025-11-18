#include "fractal_simd.h"

#include <complex>
#include <immintrin.h> // Instrucciones SIMD para trabajar con la unidad vectorial avx

#include "palette.h"

extern int max_iterations;     // número máximo de iteraciones antes de declarar "no divergente"
extern std::complex<double> c; // constante 'c' del conjunto de Julia

void julia_simd(double x_min, double y_min, double x_max,
                double y_max, uint32_t width, uint32_t height, uint32_t *pixel_buffer)

{
    // Paso entre píxeles en coordenadas complejas
    double dx = (x_max - x_min) / (width);
    double dy = (y_max - y_min) / (height);

    // vamos a repetir 8 veces los valores para trabajar con 8 floats a la vez

    // (x_min, x_min, x_min, x_min, x_min, x_min, x_min, x_min)
    __m256 xmin = _mm256_set1_ps(x_min);
    // Cambiar ymax por ymin
    __m256 ymin = _mm256_set1_ps(y_min);

    __m256 xcale = _mm256_set1_ps(dx); // (dx, dx, dx, dx, dx, dx, dx, dx)
    __m256 ycale = _mm256_set1_ps(dy); // (dy, dy, dy, dy, dy, dy, dy, dy)

    __m256 c_real = _mm256_set1_ps(c.real()); // constante c real (cr,cr,cr,cr,cr,cr,cr,cr)
    __m256 c_imag = _mm256_set1_ps(c.imag()); // constante c imaginaria (ci,ci,ci,ci,ci,ci,ci,ci)

    // devemos comparar que la normaa sea menor a 4.0 de lo contrario diverge y salimos del ciclo
    __m256 threshold = _mm256_set1_ps(4.0f); // (4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0)

    // para comaprar que todos los valores hayan diverigdo dento del ciclo while
    __m256 one = _mm256_set1_ps(1); // (1,1,1,1,1,1,1,1)
    // a lo largo del programa se van a ir sumando

    /// para interar sobe los pixeles (imagen)
    for (int i = 0; i < width; i++) // porcesamo 1
    {
        for (int j = 0; j < height; j += 8) // porcesamos 8

        {
            __m256 mx = _mm256_set1_ps(i);                                                     // (i,i,i,i,i,i,i,i)
            __m256 my = _mm256_set_ps(j + 7, j + 6, j + 5, j + 4, j + 3, j + 2, j + 1, j + 0); // (j+7,j+6,j+5,j+4,j+3,j+2,j+1,j+0)

            // OPERACION

            // 1) parte real
            //  i*dx + x_min
            __m256 cr = _mm256_add_ps(xmin, _mm256_mul_ps(mx, xcale)); // x = xmin + i * dx

            // 2) parte imaginaria
            // y_min + j*dy
            __m256 ci = _mm256_add_ps(ymin, _mm256_mul_ps(my, ycale)); // y = ymin + j * dy

            int iter = 1;
            __m256 zr = cr; // inicializamos zr = x
            __m256 zi = ci; // inicializamos zi = y

            // mk es el nuemro de interaciones que cada componente ha permanecido dentro del umbral
            __m256 mk = _mm256_set1_ps(iter); // todos inician en 1 interaciones

            while (iter < max_iterations)
            {
                // calcular zn+1 = zn^2 + c

                __m256 zr2 = _mm256_mul_ps(zr, zr); // zr*zr --> zr^2
                __m256 zi2 = _mm256_mul_ps(zi, zi); // zi*zi --> zi^2

                // Calcul de la parte imaginaria
                __m256 zrzi = _mm256_mul_ps(zr, zi); // zr*zi

                zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), c_real);   // zr^2 - zi^2 + cr
                zi = _mm256_add_ps(_mm256_add_ps(zrzi, zrzi), c_imag); // 2*zr*zi + ci

                // calcular la norma |z|^2 = zr^2 + zi^2
                zr2 = _mm256_mul_ps(zr, zr);           // zr*zr --> zr^2
                zi2 = _mm256_mul_ps(zi, zi);           // zi*zi --> zi^2
                __m256 mag2 = _mm256_add_ps(zr2, zi2); // |z|^2 = zr^2 + zi^2

                // comparar con el umbral
                __m256 mask = _mm256_cmp_ps(mag2, threshold, _CMP_LE_OS); // mag2 < 4.0
                //_CMP_LT_OQ  es para comparar si es menor

                // validar que las 8 componentes sigan siendo menores al umbral
                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk); // si alguna componente es mayor o igual a 4.0 se pone en 0

                if (_mm256_movemask_ps(mask) == 0) // si todas las componentes son mayores o iguales a 4.0
                {
                    break; // salimos del ciclo
                }
                iter++;
            }

            // devemos desenpaquetar los 8 valores de iter para asignar los colores
            float d[8];
            _mm256_storeu_ps(d, mk); // almacenamos los valores de iter en d

            // interamos para darle color (tenemos 8 colores en nuestra paleta)
            for (int it = 0; it < 8; it++)
            {
                int index = (j + it) * width + i;

                if (index < width * height) // aseguramos no salirnos del buffer
                {
                    // CORRECCIÓN: usar d[it] en lugar de d[i]
                    if (d[it] < max_iterations)
                    {
                        int color_idx = ((int)d[it]) % PALETTE_SIZE; // indice del color en la paleta
                        auto color = color_ramp[color_idx];
                        pixel_buffer[index] = color;
                    }
                    else
                    {
                        pixel_buffer[index] = 0xFF000000; // color negro para puntos dentro del conjunto
                    }
                }
            }
        }
    }
}