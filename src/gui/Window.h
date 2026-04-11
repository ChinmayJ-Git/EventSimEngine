#ifndef WINDOW_H
#define WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

class Window
{
public:
    Window(int width = 1280, int height = 720,
           const std::string &title = "EventSimEngine");

    bool isOpen();
    void pollEvents();
    void clear();
    void display();
    void close();
    sf::RenderWindow &getWindow();

private:
    sf::RenderWindow window;
};

#endif