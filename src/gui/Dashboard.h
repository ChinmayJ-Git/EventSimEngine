
// Shows live simulation stats on the side of the screen

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <SFML/Graphics.hpp>
#include <string>

class Dashboard {

public:

    sf::Font dashFont;
    bool fontLoaded;

    sf::RectangleShape dashboardBackground;

    sf::Text titleText;
    sf::Text totalEntitiesText;
    sf::Text activeEntitiesText;
    sf::Text completedEntitiesText;
    sf::Text averageWaitText;
    sf::Text currentTimeText;

    int totalEntities;
    int activeEntities;
    int completedEntities;
    double averageWaitTime;
    double currentSimTime;

    void setupDashboard() {
        totalEntities = 0;
        activeEntities = 0;
        completedEntities = 0;
        averageWaitTime = 0.0;
        currentSimTime = 0.0;
        fontLoaded = false;

        dashboardBackground.setSize(sf::Vector2f(250, 700));
        dashboardBackground.setPosition(sf::Vector2f(950, 0));
        dashboardBackground.setFillColor(sf::Color(20, 25, 50));
    }

    void updateStats(int total, int active, int completed, double avgWait, double simTime) {
        totalEntities = total;
        activeEntities = active;
        completedEntities = completed;
        averageWaitTime = avgWait;
        currentSimTime = simTime;
    }

    void drawDashboard(sf::RenderWindow& window) {
        window.draw(dashboardBackground);
    }

};

#endif
