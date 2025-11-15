#include <iostream>
#include <complex> // permite trabajar con números complejos

#include <fmt/core.h>
#include <SFML/Graphics.hpp>

// Proviene de nuestro proyecto: fuente embebida y funciones del fractal
#include "arial.ttf.h"
#include "fractal_serial.h"


#ifdef _WIN32
#include <windows.h>
#endif

// Parámetros: límites del rectángulo complejo que vamos a muestrear
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

// Control de iteraciones y constante c para el conjunto de Julia
int max_iterations = 100;
std::complex<double> c(-0.7, 0.27015);

// Tamaño de la imagen (ancho x alto)
#define WIDTH 1600
#define HEIGHT 900

// Buffer de píxeles (RGBA empaquetado en uint32_t). Se inicializa en main.
uint32_t *pixel_buffer = nullptr;

enum class runtime_type
{
    SERIAL_1 = 0,
    SERIAL_2 = 1
};


int main()
{
    // - inicializar el buffer de píxeles
    pixel_buffer = new uint32_t[WIDTH * HEIGHT];

   // tipo de dibujo
    runtime_type r_type = runtime_type::SERIAL_1;

    // Inicializar SFML para mostrar el resultado
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia Set - SFML");

#ifdef _WIN32
    // Maximizamos la ventana en Windows (opcional)
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif

    // Creamos una textura SFML y le pasamos el buffer de píxeles.
    // Nota: sf::Texture::update espera un puntero a bytes; aquí se castea.
    sf::Texture texture({WIDTH, HEIGHT});
    texture.update((const uint8_t *)pixel_buffer);

    sf::Sprite sprite(texture);

    // Texto en pantalla (usa la fuente embebida en arial_ttf)
    const sf::Font font(arial_ttf, sizeof(arial_ttf));
    sf::Text text(font, "Julia Set", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    std::string options = "OPTIONS: [1] Serial 1 [2] Serial 2";
    sf::Text textOptions(font, options, 24);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getView().getSize().y - 40});

    // contar FPS
    int frames = 0;
    int fps = 0;
    sf::Clock clockFrames; // nos permite medir el tiempo

    while (window.isOpen())
    {
        // Nota: la forma correcta de usar pollEvent en SFML suele ser:
        // sf::Event event; while (window.pollEvent(event)) { ... }
        // en el código actual se usa otra forma (std::optional) que puede necesitar ajuste.
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

                 // Añadir mas eventos de teclado
            // KeyReleased es para cuando suelto la tecla
            // KeyPressed es para cuando presiono la tecla
            else if (event->is<sf::Event::KeyReleased>())
            {
                auto evt = event->getIf<sf::Event::KeyReleased>();

                switch (evt->scancode)
                {
                case sf::Keyboard::Scan::Up:
                    max_iterations += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iterations -= 10;
                    if (max_iterations < 10)
                        max_iterations = 10;
                    break;
                case sf::Keyboard::Scan::Num1:
                    r_type = runtime_type::SERIAL_1;
                    break;
                case sf::Keyboard::Scan::Num2:
                    r_type = runtime_type::SERIAL_2;
                    break;
                }
            }
        }

         std::string mode = "";
        if (r_type == runtime_type::SERIAL_1)
        {
            mode = "SERIAL 1";
            julia_serial_1(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::SERIAL_2)
        {
            mode = "SERIAL 2";
            julia_serial_2(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        }

        texture.update((const uint8_t *)pixel_buffer); // actualizar la textura con el nuevo buffer de pixeles

        // calcular FPS
        frames++;
        if (clockFrames.getElapsedTime().asSeconds() >= 1.0f) // cada segundo
        {
            fps = frames;          // GUARDAMOS los FPS
            frames = 0;            // REINICIAMOS el contador
            clockFrames.restart(); // reiniciamos el reloj
        }

        // ACTUALIZAR EL TITULO
        auto msg = fmt::format("Julia Set: Iterations {} - FPS: {} - Mode: {}", max_iterations, fps, mode);
        text.setString(msg);

        window.clear();
        {
            window.draw(sprite);
            window.draw(text);
            window.draw(textOptions);
        }
        window.display();
    }

    // Liberar memoria al final (buena práctica)
    delete[] pixel_buffer;
    return 0;
}
