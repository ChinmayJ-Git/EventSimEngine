#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>

class Menu
{
public:
    int show(sf::RenderWindow &window)
    {
        sf::Font font;
        bool fontLoaded = false;
        if (font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        {
            fontLoaded = true;
        }
        else if (font.loadFromFile("C:/SFML/examples/shader/resources/sansation.ttf"))
        {
            fontLoaded = true;
        }

        sf::RectangleShape hospitalButton(sf::Vector2f(420.0f, 84.0f));
        hospitalButton.setPosition(430.0f, 270.0f);
        hospitalButton.setFillColor(sf::Color(30, 140, 95));

        sf::RectangleShape trafficButton(sf::Vector2f(420.0f, 84.0f));
        trafficButton.setPosition(430.0f, 390.0f);
        trafficButton.setFillColor(sf::Color(170, 95, 25));

        sf::Text title;
        sf::Text hospitalText;
        sf::Text trafficText;

        if (fontLoaded)
        {
            title.setFont(font);
            title.setCharacterSize(56);
            title.setString("EventSimEngine");
            title.setFillColor(sf::Color::White);

            sf::FloatRect bounds = title.getLocalBounds();
            float titleX = ((float)window.getSize().x - bounds.width) * 0.5f;
            title.setPosition(titleX, 120.0f);

            hospitalText.setFont(font);
            hospitalText.setCharacterSize(30);
            hospitalText.setString("Hospital Simulation");
            hospitalText.setFillColor(sf::Color::White);
            sf::FloatRect hb = hospitalText.getLocalBounds();
            hospitalText.setPosition(430.0f + (420.0f - hb.width) * 0.5f, 294.0f);

            trafficText.setFont(font);
            trafficText.setCharacterSize(30);
            trafficText.setString("Traffic Simulation");
            trafficText.setFillColor(sf::Color::White);
            sf::FloatRect tb = trafficText.getLocalBounds();
            trafficText.setPosition(430.0f + (420.0f - tb.width) * 0.5f, 414.0f);
        }

        int choice = 0;
        while (window.isOpen() && choice == 0)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    return 1;
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;

                    if (x >= 430 && x <= 850 && y >= 270 && y <= 354)
                    {
                        choice = 1;
                    }
                    if (x >= 430 && x <= 850 && y >= 390 && y <= 474)
                    {
                        choice = 2;
                    }
                }
            }

            window.clear(sf::Color(16, 20, 28));
            window.draw(hospitalButton);
            window.draw(trafficButton);
            if (fontLoaded)
            {
                window.draw(title);
                window.draw(hospitalText);
                window.draw(trafficText);
            }
            window.display();
        }

        if (choice == 0)
        {
            return 1;
        }
        return choice;
    }
};

#endif