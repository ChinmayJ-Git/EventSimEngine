#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow &window)
{
    this->window = &window;
    fontLoaded = false;
}

void Renderer::setWindow(sf::RenderWindow &window)
{
    this->window = &window;
}

void Renderer::drawHospital(SimEngine *engine, int numberOfDoctors)
{
    if (window == nullptr || engine == nullptr)
    {
        return;
    }

    ensureFont();

    sf::RectangleShape waitingArea(sf::Vector2f(520.0f, 560.0f));
    waitingArea.setPosition(40.0f, 80.0f);
    waitingArea.setFillColor(sf::Color(35, 40, 55));
    window->draw(waitingArea);

    sf::RectangleShape treatmentArea(sf::Vector2f(520.0f, 560.0f));
    treatmentArea.setPosition(620.0f, 80.0f);
    treatmentArea.setFillColor(sf::Color(30, 55, 45));
    window->draw(treatmentArea);

    if (fontLoaded)
    {
        sf::Text waitingLabel("Waiting area", font, 20);
        waitingLabel.setFillColor(sf::Color::White);
        waitingLabel.setPosition(60.0f, 40.0f);
        window->draw(waitingLabel);

        sf::Text treatmentLabel("Treatment area", font, 20);
        treatmentLabel.setFillColor(sf::Color::White);
        treatmentLabel.setPosition(640.0f, 40.0f);
        window->draw(treatmentLabel);
    }

    for (int i = 0; i < numberOfDoctors; i++)
    {
        sf::RectangleShape doctorShape(sf::Vector2f(48.0f, 26.0f));
        doctorShape.setFillColor(sf::Color(70, 120, 240));
        doctorShape.setPosition(1010.0f, 110.0f + (float)i * 60.0f);
        window->draw(doctorShape);
    }

    int waitingCount = 0;
    int busyCount = 0;

    for (int id = 10000; id < 26000; id++)
    {
        Entity *entity = engine->getEntity(id);
        if (entity == nullptr)
        {
            continue;
        }

        if (entity->type != PATIENT)
        {
            continue;
        }

        if (entity->state == FINISHED)
        {
            continue;
        }

        sf::CircleShape patientShape(12.0f);
        if (entity->priorityLevel == 1)
        {
            patientShape.setFillColor(sf::Color(220, 40, 40));
        }
        else if (entity->priorityLevel == 3)
        {
            patientShape.setFillColor(sf::Color(245, 200, 40));
        }
        else
        {
            patientShape.setFillColor(sf::Color(90, 210, 90));
        }

        if (entity->state == WAITING)
        {
            float x = 80.0f + (float)(waitingCount % 10) * 45.0f;
            float y = 120.0f + (float)(waitingCount / 10) * 45.0f;
            patientShape.setPosition(x, y);
            waitingCount++;
        }
        else
        {
            float x = 660.0f + (float)(busyCount % 8) * 45.0f;
            float y = 120.0f + (float)(busyCount / 8) * 45.0f;
            patientShape.setPosition(x, y);
            busyCount++;
        }

        window->draw(patientShape);
    }
}

void Renderer::drawTraffic(SimEngine *engine, int numberOfIntersections)
{
    if (window == nullptr || engine == nullptr)
    {
        return;
    }

    if (numberOfIntersections <= 0)
    {
        return;
    }

    const float centerX = 500.0f;
    const float centerY = 330.0f;
    const float radius = 210.0f;

    for (int i = 0; i < numberOfIntersections; i++)
    {
        int next = (i + 1) % numberOfIntersections;
        sf::Vector2f from = getRingPosition(i, numberOfIntersections, centerX, centerY, radius);
        sf::Vector2f to = getRingPosition(next, numberOfIntersections, centerX, centerY, radius);

        sf::Vertex road[] = {
            sf::Vertex(from, sf::Color(120, 120, 120)),
            sf::Vertex(to, sf::Color(120, 120, 120))};
        window->draw(road, 2, sf::Lines);
    }

    for (int i = 0; i < numberOfIntersections; i++)
    {
        sf::Vector2f position = getRingPosition(i, numberOfIntersections, centerX, centerY, radius);

        sf::RectangleShape intersectionShape(sf::Vector2f(28.0f, 28.0f));
        intersectionShape.setFillColor(sf::Color::White);
        intersectionShape.setPosition(position.x - 14.0f, position.y - 14.0f);
        window->draw(intersectionShape);

        int waitingCars = 0;
        for (int id = 20000; id < 30000; id++)
        {
            Entity *entity = engine->getEntity(id);
            if (entity == nullptr)
            {
                continue;
            }

            if (entity->type == CAR && entity->state == WAITING &&
                entity->currentLocationId == i)
            {
                waitingCars++;
            }
        }

        for (int k = 0; k < waitingCars && k < 6; k++)
        {
            sf::RectangleShape carShape(sf::Vector2f(8.0f, 5.0f));
            carShape.setFillColor(sf::Color(240, 215, 70));
            carShape.setPosition(position.x - 12.0f + (float)(k % 3) * 9.0f,
                                 position.y + 16.0f + (float)(k / 3) * 7.0f);
            window->draw(carShape);
        }

        int lightState = 1;
        int phase = (int)(engine->getCurrentTime() / 30.0);
        if (((phase + i) % 2) != 0)
        {
            lightState = 0;
        }

        sf::CircleShape lightShape(7.0f);
        if (lightState == 1)
        {
            lightShape.setFillColor(sf::Color(70, 220, 80));
        }
        else
        {
            lightShape.setFillColor(sf::Color(220, 60, 60));
        }
        lightShape.setPosition(position.x + 16.0f, position.y - 20.0f);
        window->draw(lightShape);
    }
}

void Renderer::drawStats(SimulationStats &stats)
{
    if (window == nullptr)
    {
        return;
    }

    ensureFont();
    if (!fontLoaded)
    {
        return;
    }

    sf::RectangleShape panel(sf::Vector2f(500.0f, 140.0f));
    panel.setPosition(20.0f, 560.0f);
    panel.setFillColor(sf::Color(15, 15, 20, 210));
    window->draw(panel);

    std::string textValue = "events: " + std::to_string(stats.totalEventsProcessed);
    textValue = textValue + "  finished: " + std::to_string(stats.totalEntitiesFinished);
    textValue = textValue + "\navg wait: " + std::to_string(stats.getAverageWaitTime());
    textValue = textValue + "  longest: " + std::to_string(stats.longestWaitTime);

    sf::Text statsText(textValue, font, 18);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(36.0f, 584.0f);
    window->draw(statsText);
}

void Renderer::ensureFont()
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

sf::Vector2f Renderer::getRingPosition(int index, int total,
                                       float centerX, float centerY,
                                       float radius) const
{
    float ratio = (float)index / (float)total;
    float angle = ratio * 6.2831853f;

    float x = centerX + std::cos(angle) * radius;
    float y = centerY + std::sin(angle) * radius;
    return sf::Vector2f(x, y);
}