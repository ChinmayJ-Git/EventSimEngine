
// Opens and manages the game window using SFML

#ifndef WINDOW_H
#define WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

class Window {

public:

    sf::RenderWindow renderWindow; 
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    bool isWindowOpen;

    void createWindow(int width, int height, std::string title) {
        windowWidth = width;
        windowHeight = height;
        windowTitle = title;

        // unsigned int cast needed here or compiler gives warning
        renderWindow.create(sf::VideoMode({(unsigned int)width, (unsigned int)height}), title);
        renderWindow.setFramerateLimit(60); // 60 fps is smooth enough
        isWindowOpen = true;
    }

    bool checkIfOpen() {
        return renderWindow.isOpen();
    }
    
    void clearScreen() {
        renderWindow.clear(sf::Color(15, 20, 40)); 
    }

    void displayScreen() {
        renderWindow.display();
    }

    void closeWindow() {
        renderWindow.close();
        isWindowOpen = false;
    }

    // checks if user pressed X to close - runs every frame
    void handleEvents() {
        while (auto eventResult = renderWindow.pollEvent()) {
            if (eventResult->is<sf::Event::Closed>()) {
                closeWindow();
            }
        }
    }

};

#endif