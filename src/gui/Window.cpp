#include "Window.h"

Window::Window(std::string title, int width, int height)
    : window(sf::VideoMode(1280, 720), title)
{
    (void)width;
    (void)height;
    isOpen = true;
    upPressed = false;
    downPressed = false;
}

void Window::clear() { window.clear(sf::Color::Black); }
void Window::display() { window.display(); }

void Window::close()
{
    window.close();
    isOpen = false;
}

bool Window::checkOpen()
{
    isOpen = window.isOpen();
    return isOpen;
}

void Window::pollEvents()
{
    upPressed = false;
    downPressed = false;
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            close();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Up)
            {
                upPressed = true;
            }
            else if (event.key.code == sf::Keyboard::Down)
            {
                downPressed = true;
            }
        }
    }
}

bool Window::consumeUpPress()
{
    bool wasPressed = upPressed;
    upPressed = false;
    return wasPressed;
}

bool Window::consumeDownPress()
{
    bool wasPressed = downPressed;
    downPressed = false;
    return wasPressed;
}

sf::RenderWindow &Window::getWindow()
{
    return window;
}