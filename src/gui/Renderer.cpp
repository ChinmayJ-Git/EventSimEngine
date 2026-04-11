
// renderer implementation

#include "Renderer.h"

Renderer::Renderer() {
    fontLoaded = false;

    patientCircle.setRadius(15);
    patientCircle.setFillColor(sf::Color(0, 200, 150));

    entityBox.setSize(sf::Vector2f(20, 20));
    entityBox.setFillColor(sf::Color(100, 150, 255));
}

Renderer::~Renderer() {
}