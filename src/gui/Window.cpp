#include "Window.h"

Window::Window(std::string title, int width, int height)
    : window(sf::VideoMode(1280, 720), title)
{
    (void)width;
    (void)height;
    isOpen = true;
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
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            close();
        }
    }
}

sf::RenderWindow &Window::getWindow()
{
    return window;
}