
// scenario selection screen

#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>

class Menu {

public:

    int selectedScenario;
    bool scenarioChosen;

    sf::RectangleShape hospitalButton;
    sf::RectangleShape trafficButton;

    sf::Font menuFont;
    sf::Text titleText;
    sf::Text hospitalText;
    sf::Text trafficText;

    // setup buttons
    void setupMenu() {
        selectedScenario = 0;
        scenarioChosen = false;

        // hospital button
        hospitalButton.setSize(sf::Vector2f(300, 60));
        hospitalButton.setPosition(sf::Vector2f(450, 280));
        hospitalButton.setFillColor(sf::Color(0, 150, 100));

        // traffic button
        trafficButton.setSize(sf::Vector2f(300, 60));
        trafficButton.setPosition(sf::Vector2f(450, 380));
        trafficButton.setFillColor(sf::Color(180, 80, 0));
    }

    // check which button clicked
    void handleMouseClick(int mouseX, int mouseY) {
        if (mouseX >= 450 && mouseX <= 750 && mouseY >= 280 && mouseY <= 340) {
            selectedScenario = 1;
            scenarioChosen = true;
        }
        if (mouseX >= 450 && mouseX <= 750 && mouseY >= 380 && mouseY <= 440) {
            selectedScenario = 2;
            scenarioChosen = true;
        }
    }

    // draw buttons
    void drawMenu(sf::RenderWindow& window) {
        window.draw(hospitalButton);
        window.draw(trafficButton);
    }

    
};

#endif