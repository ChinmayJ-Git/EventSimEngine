
// Starting screen where user picks a scenario

#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>

class Menu {

public:

    int selectedScenario;
    bool scenarioChosen;

    sf::RectangleShape hospitalButton;
    sf::RectangleShape networkButton;
    sf::RectangleShape trafficButton;

    sf::Font menuFont;
    sf::Text titleText;
    sf::Text hospitalText;
    sf::Text networkText;
    sf::Text trafficText;

    void setupMenu() {
        selectedScenario = 0;
        scenarioChosen = false;

        hospitalButton.setSize(sf::Vector2f(300, 60));
        hospitalButton.setPosition(sf::Vector2f(450, 250));
        hospitalButton.setFillColor(sf::Color(0, 150, 100));

        networkButton.setSize(sf::Vector2f(300, 60));
        networkButton.setPosition(sf::Vector2f(450, 350));
        networkButton.setFillColor(sf::Color(0, 100, 180));

        trafficButton.setSize(sf::Vector2f(300, 60));
        trafficButton.setPosition(sf::Vector2f(450, 450));
        trafficButton.setFillColor(sf::Color(180, 80, 0));
    }

    void handleMouseClick(int mouseX, int mouseY) {
        if (mouseX >= 450 && mouseX <= 750 && mouseY >= 250 && mouseY <= 310) {
            selectedScenario = 1;
            scenarioChosen = true;
        }
        if (mouseX >= 450 && mouseX <= 750 && mouseY >= 350 && mouseY <= 410) {
            selectedScenario = 2;
            scenarioChosen = true;
        }
        if (mouseX >= 450 && mouseX <= 750 && mouseY >= 450 && mouseY <= 510) {
            selectedScenario = 3;
            scenarioChosen = true;
        }
    }

    void drawMenu(sf::RenderWindow& window) {
        window.draw(hospitalButton);
        window.draw(networkButton);
        window.draw(trafficButton);
    }

};

#endif