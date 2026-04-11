#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <SFML/Graphics.hpp>
#include <string>

class Dashboard
{
public:
    Dashboard()
    {
        fontLoaded = false;
    }

    void draw(sf::RenderWindow &window, double currentTime,
              int eventsProcessed, int entitiesFinished,
              double avgWait, double longestWait)
    {
        sf::RectangleShape panel(sf::Vector2f(280.0f, (float)window.getSize().y));
        panel.setPosition((float)window.getSize().x - 280.0f, 0.0f);
        panel.setFillColor(sf::Color(20, 24, 30, 220));
        window.draw(panel);

        ensureFont();
        if (!fontLoaded)
        {
            return;
        }

        sf::Text title("Live dashboard", font, 24);
        title.setFillColor(sf::Color::White);
        title.setPosition((float)window.getSize().x - 260.0f, 24.0f);
        window.draw(title);

        std::string lines = "time: " + std::to_string(currentTime);
        lines = lines + "\nevents: " + std::to_string(eventsProcessed);
        lines = lines + "\nfinished: " + std::to_string(entitiesFinished);
        lines = lines + "\navg wait: " + std::to_string(avgWait);
        lines = lines + "\nlongest wait: " + std::to_string(longestWait);

        sf::Text values(lines, font, 18);
        values.setFillColor(sf::Color::White);
        values.setPosition((float)window.getSize().x - 260.0f, 84.0f);
        window.draw(values);
    }

private:
    sf::Font font;
    bool fontLoaded;

    void ensureFont()
    {
        if (fontLoaded)
        {
            return;
        }

        if (font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        {
            fontLoaded = true;
            return;
        }

        if (font.loadFromFile("C:/SFML/examples/shader/resources/sansation.ttf"))
        {
            fontLoaded = true;
            return;
        }

        fontLoaded = false;
    }
};

#endif
