#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

#include "../engine/SimEngine.h"

class Renderer
{
public:
    Renderer(sf::RenderWindow &window);

    void setWindow(sf::RenderWindow &window);
    void drawHospital(SimEngine *engine, int numberOfDoctors);
    void drawTraffic(SimEngine *engine, int numberOfIntersections);
    void drawStats(SimulationStats &stats);

private:
    sf::RenderWindow *window;
    sf::Font font;
    bool fontLoaded;

    void ensureFont();
    sf::Vector2f getRingPosition(int index, int total,
                                 float centerX, float centerY,
                                 float radius) const;
};

#endif