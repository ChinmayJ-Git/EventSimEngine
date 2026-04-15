#ifndef WINDOW_H
#define WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

class Window
{
private:
    sf::RenderWindow window;
    bool isOpen;

public:
    Window(std::string title, int width, int height);
    void clear();
    void display();
    void close();
    bool checkOpen();
    void pollEvents();
    sf::RenderWindow &getWindow();
};

#endif