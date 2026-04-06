// Window.h
// Written by: Aastha Malpani
// Opens and manages the game window using SFML

#ifndef WINDOW_H
#define WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

class Window {

public:

    sf::RenderWindow renderWindow; // the actual window on screen
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    bool isWindowOpen;

    // opens the window with given size and title
    void createWindow(int width, int height, std::string title) {
        windowWidth = width;
        windowHeight = height;
        windowTitle = title;

        // unsigned int cast needed here or compiler gives warning
        renderWindow.create(sf::VideoMode({(unsigned int)width, (unsigned int)height}), title);
        renderWindow.setFramerateLimit(60); // 60 fps is smooth enough
        isWindowOpen = true;
    }

    // returns true if window is still running
    bool checkIfOpen() {
        return renderWindow.isOpen();
    }

    // wipes screen clean before drawing next frame
    void clearScreen() {
        renderWindow.clear(sf::Color(15, 20, 40)); // dark navy background
    }

    // shows the drawn frame on screen
    void displayScreen() {
        renderWindow.display();
    }

    // closes the window
    void closeWindow() {
        renderWindow.close();
        isWindowOpen = false;
    }

    // checks if user pressed X to close - runs every frame
    void handleEvents() {
        while (auto eventResult = renderWindow.pollEvent()) {
            if (eventResult->is<sf::Event::Closed>()) {
                closeWindow(); // close if X button clicked
            }
        }
    }

};

#endif