
// draws entities on simulation window

#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <string>

class Renderer {

public:

    sf::Font font;
    bool fontLoaded;

    sf::CircleShape patientCircle;
    sf::RectangleShape entityBox;
    sf::Text labelText;

    void setupRenderer() {
        fontLoaded = false;

        patientCircle.setRadius(15);
        patientCircle.setFillColor(sf::Color(0, 200, 150));

        entityBox.setSize(sf::Vector2f(20, 20));
        entityBox.setFillColor(sf::Color(100, 150, 255));
    }

    void drawPatient(sf::RenderWindow& window, float x, float y) {
        patientCircle.setPosition(sf::Vector2f(x, y));
        window.draw(patientCircle);
    }

    void drawCar(sf::RenderWindow& window, float x, float y, sf::Color color) {
        entityBox.setPosition(sf::Vector2f(x, y));
        entityBox.setFillColor(color);
        window.draw(entityBox);
    }

    void drawCircle(sf::RenderWindow& window, float x, float y, float radius, sf::Color color) {
        sf::CircleShape circle(radius);
        circle.setPosition(sf::Vector2f(x, y));
        circle.setFillColor(color);
        window.draw(circle);
    }

    void drawRect(sf::RenderWindow& window, float x, float y, float width, float height, sf::Color color) {
        sf::RectangleShape rect(sf::Vector2f(width, height));
        rect.setPosition(sf::Vector2f(x, y));
        rect.setFillColor(color);
        window.draw(rect);
    }

};

#endif