#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "Window.h"
#include "../scenarios/HospitalSim.h"

class Renderer
{
private:
    sf::Font font;
    bool ready;
    Window *win;
    int lastWaitingCount;
    float newPatientOffset;
    int scrollOffset;

    void drawWaitingPanel(sf::RenderWindow &w, HospitalSim *sim);
    void drawTreatmentPanel(sf::RenderWindow &w, HospitalSim *sim);
    void drawEventLogPanel(sf::RenderWindow &w, HospitalSim *sim);
    void drawStatsPanel(sf::RenderWindow &w, HospitalSim *sim);
    void drawPanelTitle(sf::RenderWindow &w, const char *text, float x, float y);
    void drawPatientCircle(sf::RenderWindow &w, float x, float y, int patientId, int priority);

public:
    Renderer(Window *w);
    bool loadFont();
    bool isReady() const;
    void drawAll(HospitalSim *sim);
    void drawCompleteOverlay();
};

#endif