#include "Window.h"

Window::Window(int width, int height, const std::string &title)
    : window(sf::VideoMode((unsigned int)width, (unsigned int)height), title)
{
    window.setFramerateLimit(60);
}

bool Window::isOpen()
{
    return window.isOpen();
}

void Window::pollEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
    }
}

void Window::clear()
{
    window.clear(sf::Color(18, 22, 30));
}

void Window::display()
{
    window.display();
}

void Window::close()
{
    window.close();
}

sf::RenderWindow &Window::getWindow()
{
    return window;
}