
// implements the Window class

#include "Window.h"

Window::Window() {
    windowWidth = 1200;
    windowHeight = 700;
    windowTitle = "Event Simulation Engine";
    isWindowOpen = false;
}

Window::~Window() {
    if (renderWindow.isOpen()) {
        renderWindow.close();
    }
}