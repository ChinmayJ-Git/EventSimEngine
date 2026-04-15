#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <SFML/Graphics.hpp>

class Dashboard
{
private:
    sf::Font *font;
    float x;
    float y;

public:
    Dashboard(sf::Font *f, float startX, float startY)
    {
        font = f;
        x = startX;
        y = startY;
    }

    void draw(sf::RenderWindow &w, int totalPatients, double avgWait,
              double longestWait, int escalations,
              int criticalWaiting, int urgentWaiting, int normalWaiting)
    {
        if (font == 0)
        {
            return;
        }
        sf::Text t1("Total: " + std::to_string(totalPatients), *font, 18);
        sf::Text t2("Avg wait: " + std::to_string(avgWait), *font, 18);
        sf::Text t3("Longest: " + std::to_string(longestWait), *font, 18);
        sf::Text t4("Escalations: " + std::to_string(escalations), *font, 18);
        sf::Text t5("Critical: " + std::to_string(criticalWaiting), *font, 18);
        sf::Text t6("Urgent: " + std::to_string(urgentWaiting), *font, 18);
        sf::Text t7("Normal: " + std::to_string(normalWaiting), *font, 18);
        sf::Text *arr[7] = {&t1, &t2, &t3, &t4, &t5, &t6, &t7};
        for (int i = 0; i < 7; i++)
        {
            arr[i]->setPosition(x, y + 30.0f * (float)i);
            arr[i]->setFillColor(sf::Color::White);
            w.draw(*arr[i]);
        }
    }
};

#endif
