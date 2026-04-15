#include "Renderer.h"

#include <cstdio>
#include <iostream>

Renderer::Renderer(Window *w)
{
    win = w;
    ready = false;
    lastWaitingCount = 0;
    newPatientOffset = 0.0f;
    scrollOffset = 0;
    ready = loadFont();
}

bool Renderer::loadFont()
{
    if (!font.loadFromFile("assets/font.ttf"))
    {
        std::cerr << "Error: failed to load assets/font.ttf\n";
        return false;
    }
    return true;
}

bool Renderer::isReady() const
{
    return ready;
}

void Renderer::drawPanelTitle(sf::RenderWindow &w, const char *text, float x, float y)
{
    if (font.getInfo().family.empty())
    {
        return;
    }
    sf::Text t(text, font, 28);
    t.setFillColor(sf::Color::White);
    t.setPosition(x, y);
    w.draw(t);
}

void Renderer::drawPatientCircle(sf::RenderWindow &w, float x, float y, int patientId, int priority)
{
    sf::CircleShape c(15.0f);
    if (priority == 1)
    {
        c.setFillColor(sf::Color::Red);
    }
    else if (priority == 2)
    {
        c.setFillColor(sf::Color(255, 140, 0));
    }
    else
    {
        c.setFillColor(sf::Color::Green);
    }
    c.setPosition(x, y);
    w.draw(c);

    if (font.getInfo().family.empty())
    {
        return;
    }
    sf::Text idText(std::to_string(patientId), font, 14);
    idText.setFillColor(sf::Color::Black);
    idText.setPosition(x + 7.0f, y + 5.0f);
    w.draw(idText);
}

void Renderer::drawWaitingPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(400.0f, 720.0f));
    panel.setPosition(0.0f, 0.0f);
    panel.setFillColor(sf::Color(18, 18, 18));
    w.draw(panel);
    drawPanelTitle(w, "WAITING ROOM", 20.0f, 16.0f);

    if (sim == 0)
    {
        return;
    }

    int totalWaiting = sim->getTotalWaiting();
    if (totalWaiting > 15)
    {
        scrollOffset = totalWaiting - 15;
    }
    else
    {
        scrollOffset = 0;
    }
    if (totalWaiting > lastWaitingCount)
    {
        newPatientOffset = -35.0f;
    }
    if (newPatientOffset < 0.0f)
    {
        newPatientOffset += 5.0f;
    }

    int ids[15];
    int priorities[15];
    int count = sim->getWaitingSnapshot(ids, priorities, 15, scrollOffset);
    for (int i = 0; i < count; i++)
    {
        float y = 90.0f + (float)i * 38.0f;
        if (i == count - 1)
        {
            y += newPatientOffset;
        }
        drawPatientCircle(w, 52.0f, y, ids[i], priorities[i]);
    }

    if (font.getInfo().family.empty())
    {
        return;
    }
    if (totalWaiting > 15)
    {
        char line[64];
        std::snprintf(line, 64, "Showing 15 of %d", totalWaiting);
        sf::Text t(line, font, 16);
        t.setFillColor(sf::Color::White);
        t.setPosition(20.0f, 680.0f);
        w.draw(t);
    }

    lastWaitingCount = totalWaiting;
}

void Renderer::drawTreatmentPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(450.0f, 720.0f));
    panel.setPosition(400.0f, 0.0f);
    panel.setFillColor(sf::Color(24, 24, 24));
    w.draw(panel);
    drawPanelTitle(w, "TREATMENT ROOMS", 430.0f, 16.0f);

    sf::Color colors[3] = {sf::Color::Blue, sf::Color::Cyan, sf::Color(160, 32, 240)};
    for (int i = 0; i < 3; i++)
    {
        float x = 430.0f + (float)i * 140.0f;
        sf::RectangleShape station(sf::Vector2f(120.0f, 220.0f));
        station.setPosition(x, 110.0f);
        station.setFillColor(colors[i]);
        w.draw(station);

        if (!font.getInfo().family.empty())
        {
            sf::Text label(sim ? sim->getDoctorLabel(i) : "DOCTOR", font, 16);
            label.setFillColor(sf::Color::White);
            label.setPosition(x + 8.0f, 118.0f);
            w.draw(label);
        }

        sf::RectangleShape icon(sf::Vector2f(28.0f, 28.0f));
        icon.setPosition(x + 46.0f, 150.0f);
        icon.setFillColor(sf::Color::White);
        w.draw(icon);
        sf::RectangleShape vLine(sf::Vector2f(4.0f, 18.0f));
        sf::RectangleShape hLine(sf::Vector2f(18.0f, 4.0f));
        vLine.setPosition(x + 58.0f, 155.0f);
        hLine.setPosition(x + 51.0f, 162.0f);
        vLine.setFillColor(sf::Color::Red);
        hLine.setFillColor(sf::Color::Red);
        w.draw(vLine);
        w.draw(hLine);

        if (sim != 0 && sim->getDoctorBusyNow(i) > 0)
        {
            int patientId = sim->getDoctorCurrentPatient(i);
            drawPatientCircle(w, x + 43.0f, 225.0f, patientId, i + 1);
            if (!font.getInfo().family.empty())
            {
                char line[64];
                std::snprintf(line, 64, "BUSY x%d", sim->getDoctorBusyNow(i));
                sf::Text busy(line, font, 16);
                busy.setFillColor(sf::Color::White);
                busy.setPosition(x + 20.0f, 280.0f);
                w.draw(busy);
            }
        }
        else if (!font.getInfo().family.empty())
        {
            sf::Text freeText("AVAILABLE", font, 16);
            freeText.setFillColor(sf::Color::Green);
            freeText.setPosition(x + 16.0f, 255.0f);
            w.draw(freeText);
        }
    }
}

void Renderer::drawEventLogPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(430.0f, 360.0f));
    panel.setPosition(850.0f, 0.0f);
    panel.setFillColor(sf::Color(14, 14, 14));
    w.draw(panel);
    drawPanelTitle(w, "EVENT LOG", 870.0f, 16.0f);

    if (font.getInfo().family.empty() || sim == 0)
    {
        return;
    }

    int lines = sim->getEventLogCount();
    if (lines > 12)
    {
        lines = 12;
    }
    for (int i = 0; i < lines; i++)
    {
        sf::Text t(sim->getEventLogLine(i), font, 16);
        int type = sim->getEventLogType(i);
        if (type == 1)
        {
            t.setFillColor(sf::Color::Cyan);
        }
        else if (type == 2)
        {
            t.setFillColor(sf::Color::Yellow);
        }
        else if (type == 3)
        {
            t.setFillColor(sf::Color::Green);
        }
        else
        {
            t.setFillColor(sf::Color::White);
        }
        t.setPosition(860.0f, 64.0f + (float)i * 24.0f);
        w.draw(t);
    }
}

void Renderer::drawStatsPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(430.0f, 360.0f));
    panel.setPosition(850.0f, 360.0f);
    panel.setFillColor(sf::Color(20, 20, 20));
    w.draw(panel);
    drawPanelTitle(w, "LIVE STATS", 870.0f, 376.0f);

    if (font.getInfo().family.empty() || sim == 0)
    {
        return;
    }

    char lines[8][96];
    std::snprintf(lines[0], 96, "Total Patients: %d", sim->getTotalPatients());
    std::snprintf(lines[1], 96, "Currently Waiting: %d", sim->getTotalWaiting());
    std::snprintf(lines[2], 96, "Avg Wait Time: %.1f s", sim->getAverageWait());
    std::snprintf(lines[3], 96, "Escalations: %d", sim->getEscalationCount());
    std::snprintf(lines[4], 96, "Emergency Doctor: %d%% busy", (int)(sim->getDoctorUtilisation(0) + 0.5));
    std::snprintf(lines[5], 96, "General Doctor: %d%% busy", (int)(sim->getDoctorUtilisation(1) + 0.5));
    std::snprintf(lines[6], 96, "Specialist Doctor: %d%% busy", (int)(sim->getDoctorUtilisation(2) + 0.5));
    std::snprintf(lines[7], 96, "Current Sim Time: %.1f s", sim->getCurrentSimTime());

    for (int i = 0; i < 8; i++)
    {
        sf::Text t(lines[i], font, 20);
        t.setFillColor(sf::Color::White);
        t.setPosition(862.0f, 430.0f + (float)i * 34.0f);
        w.draw(t);
    }
}

void Renderer::drawAll(HospitalSim *sim)
{
    if (!ready || win == 0 || !win->checkOpen())
    {
        return;
    }
    sf::RenderWindow &w = win->getWindow();
    drawWaitingPanel(w, sim);
    drawTreatmentPanel(w, sim);
    drawEventLogPanel(w, sim);
    drawStatsPanel(w, sim);
}

void Renderer::drawCompleteOverlay()
{
    if (!ready || win == 0 || !win->checkOpen())
    {
        return;
    }

    sf::RenderWindow &w = win->getWindow();
    sf::RectangleShape shade(sf::Vector2f(1280.0f, 720.0f));
    shade.setPosition(0.0f, 0.0f);
    shade.setFillColor(sf::Color(0, 0, 0, 110));
    w.draw(shade);

    sf::RectangleShape card(sf::Vector2f(640.0f, 180.0f));
    card.setPosition(320.0f, 260.0f);
    card.setFillColor(sf::Color(22, 26, 38));
    card.setOutlineThickness(3.0f);
    card.setOutlineColor(sf::Color(64, 220, 120));
    w.draw(card);

    sf::Text title("SIMULATION COMPLETE", font, 48);
    title.setFillColor(sf::Color::White);
    title.setPosition(360.0f, 300.0f);
    w.draw(title);

    sf::Text hint("Close the window to exit", font, 24);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(500.0f, 370.0f);
    w.draw(hint);
}