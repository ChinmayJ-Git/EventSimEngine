#include "Renderer.h"

#include <cstdio>
#include <cstring>
#include <iostream>

static const float leftPanelX = 0.0f;
static const float leftPanelW = 380.0f;
static const float middlePanelX = 390.0f;
static const float middlePanelW = 390.0f;
static const float rightPanelX = 790.0f;
static const float rightPanelW = 480.0f;

static void drawTrimmedText(sf::RenderWindow &w, sf::Font &font, const char *src,
                            float x, float y, unsigned int size, float maxWidth, sf::Color color)
{
    char line[192];
    std::snprintf(line, 192, "%s", src);

    sf::Text text(line, font, size);
    text.setFillColor(color);

    int len = (int)std::strlen(line);
    while (len > 3 && text.getLocalBounds().width > maxWidth)
    {
        len--;
        line[len] = '\0';
        line[len - 1] = '.';
        line[len - 2] = '.';
        line[len - 3] = '.';
        text.setString(line);
    }

    text.setPosition(x, y);
    w.draw(text);
}

static void toUpperText(char *text)
{
    if (text == 0)
    {
        return;
    }
    for (int i = 0; text[i] != '\0'; i++)
    {
        if (text[i] >= 'a' && text[i] <= 'z')
        {
            text[i] = (char)(text[i] - 'a' + 'A');
        }
    }
}

Renderer::Renderer(Window *w)
{
    win = w;
    ready = false;
    simulationComplete = false;
    scrollOffset = 0;
    upHeld = false;
    downHeld = false;
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

void Renderer::setSimulationComplete(bool done)
{
    simulationComplete = done;
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
    sf::RectangleShape panel(sf::Vector2f(leftPanelW, 720.0f));
    panel.setPosition(leftPanelX, 0.0f);
    panel.setFillColor(sf::Color(18, 18, 18));
    w.draw(panel);

    if (font.getInfo().family.empty())
    {
        return;
    }

    int totalWaiting = (sim == 0) ? 0 : sim->getTotalWaiting();
    char title[64];
    std::snprintf(title, 64, "WAITING ROOM (%d patients)", totalWaiting);
    drawPanelTitle(w, title, leftPanelX + 10.0f, 16.0f);

    if (sim == 0)
    {
        return;
    }

    int ids[15];
    int priorities[15];
    int waits[15];
    int count = sim->getWaitingRows(ids, priorities, waits, 15);
    for (int i = 0; i < count; i++)
    {
        float y = 76.0f + (float)i * 30.0f;

        sf::RectangleShape box(sf::Vector2f(20.0f, 20.0f));
        box.setPosition(leftPanelX + 10.0f, y + 2.0f);
        if (priorities[i] == 1)
        {
            box.setFillColor(sf::Color::Red);
        }
        else if (priorities[i] == 2)
        {
            box.setFillColor(sf::Color(255, 140, 0));
        }
        else
        {
            box.setFillColor(sf::Color::Green);
        }
        w.draw(box);

        const char *label = "NORMAL";
        if (priorities[i] == 1)
        {
            label = "CRITICAL";
        }
        else if (priorities[i] == 2)
        {
            label = "URGENT";
        }

        char row[160];
        std::snprintf(row, 160, "Patient #%d  |  %-8s  |  Waiting: %ds", ids[i], label, waits[i]);
        drawTrimmedText(w, font, row, leftPanelX + 40.0f, y + 1.0f, 16, leftPanelW - 50.0f, sf::Color::White);
    }

    if (totalWaiting > 15)
    {
        char more[64];
        std::snprintf(more, 64, "...and %d more waiting", totalWaiting - 15);
        drawTrimmedText(w, font, more, leftPanelX + 10.0f, 76.0f + 15.0f * 30.0f + 8.0f,
                        16, leftPanelW - 20.0f, sf::Color(210, 210, 210));
    }
}

void Renderer::drawTreatmentPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(middlePanelW, 720.0f));
    panel.setPosition(middlePanelX, 0.0f);
    panel.setFillColor(sf::Color(24, 24, 24));
    w.draw(panel);
    drawPanelTitle(w, "TREATMENT ROOMS", middlePanelX + 10.0f, 16.0f);
    if (font.getInfo().family.empty())
    {
        return;
    }

    const char *typeNames[3] = {"Emergency", "General", "Specialist"};
    const char *priorityNames[4] = {"-", "CRITICAL", "URGENT", "NORMAL"};
    for (int i = 0; i < 3; i++)
    {
        float x = middlePanelX + 10.0f + (float)i * 127.0f;
        bool busy = sim != 0 && sim->getDoctorBusyNow(i) > 0;

        sf::RectangleShape card(sf::Vector2f(116.0f, 220.0f));
        card.setPosition(x, 100.0f);
        card.setFillColor(sf::Color(32, 32, 40));
        card.setOutlineThickness(3.0f);
        card.setOutlineColor(busy ? sf::Color::Yellow : sf::Color::Green);
        w.draw(card);

        char line0[48];
        std::snprintf(line0, 48, "%s", sim ? sim->getDoctorName(i) : "UNKNOWN");
        toUpperText(line0);
        drawTrimmedText(w, font, line0, x + 10.0f, 112.0f, 16, 96.0f, sf::Color::White);

        char line1[48];
        std::snprintf(line1, 48, "Role: %s", typeNames[i]);
        drawTrimmedText(w, font, line1, x + 10.0f, 142.0f, 16, 96.0f, sf::Color::White);

        if (!busy)
        {
            drawTrimmedText(w, font, "✓ AVAILABLE", x + 10.0f, 212.0f, 16, 96.0f, sf::Color::White);

            char line3[48];
            std::snprintf(line3, 48, "Seen today: %d", sim ? sim->getDoctorSeenToday(i) : 0);
            drawTrimmedText(w, font, line3, x + 10.0f, 242.0f, 16, 96.0f, sf::Color::White);
        }
        else
        {
            int patientId = sim ? sim->getDoctorCurrentPatient(i) : -1;
            int priority = sim ? sim->getDoctorCurrentPriority(i) : 0;
            if (priority < 0 || priority > 3)
            {
                priority = 0;
            }

            drawTrimmedText(w, font, "NOW TREATING:", x + 10.0f, 196.0f, 16, 96.0f, sf::Color::White);

            char line3[48];
            std::snprintf(line3, 48, "Patient #%d", patientId);
            drawTrimmedText(w, font, line3, x + 10.0f, 226.0f, 16, 96.0f, sf::Color::White);

            char line4[64];
            std::snprintf(line4, 64, "Priority: %s", priorityNames[priority]);
            drawTrimmedText(w, font, line4, x + 10.0f, 256.0f, 16, 96.0f, sf::Color::White);
        }
    }
}

void Renderer::drawEventLogPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(rightPanelW, 360.0f));
    panel.setPosition(rightPanelX, 0.0f);
    panel.setFillColor(sf::Color(14, 14, 14));
    w.draw(panel);
    drawPanelTitle(w, "EVENT LOG", rightPanelX + 10.0f, 16.0f);

    if (font.getInfo().family.empty() || sim == 0)
    {
        return;
    }

    bool upNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool downNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    if (upNow && !upHeld)
    {
        scrollOffset--;
    }
    if (downNow && !downHeld)
    {
        scrollOffset++;
    }
    upHeld = upNow;
    downHeld = downNow;

    int total = sim->getEventLogCount();
    int maxOffset = 0;
    if (total > 12)
    {
        maxOffset = total - 12;
    }
    if (scrollOffset < 0)
    {
        scrollOffset = 0;
    }
    if (scrollOffset > maxOffset)
    {
        scrollOffset = maxOffset;
    }

    for (int i = 0; i < 12; i++)
    {
        int idx = total - 1 - (scrollOffset + i);
        if (idx < 0)
        {
            break;
        }

        int type = sim->getEventLogType(idx);
        sf::Color color = sf::Color::White;
        if (type == 1)
        {
            color = sf::Color::Cyan;
        }
        else if (type == 2)
        {
            color = sf::Color::Yellow;
        }
        else if (type == 3)
        {
            color = sf::Color::Green;
        }

        char line[192];
        std::snprintf(line, 192, "%s", sim->getEventLogLine(idx));
        drawTrimmedText(w, font, line, rightPanelX + 10.0f, 64.0f + (float)i * 22.0f,
                        13, rightPanelW - 20.0f, color);
    }

    char footer[96];
    std::snprintf(footer, 96, "UP/DOWN arrows to scroll | %d/500 events", total);
    drawTrimmedText(w, font, footer, rightPanelX + 10.0f, 334.0f,
                    12, rightPanelW - 20.0f, sf::Color(200, 200, 200));
}

void Renderer::drawStatsPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(rightPanelW, 360.0f));
    panel.setPosition(rightPanelX, 360.0f);
    panel.setFillColor(sf::Color(20, 20, 20));
    w.draw(panel);
    drawPanelTitle(w, "LIVE STATS", rightPanelX + 10.0f, 376.0f);

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
        drawTrimmedText(w, font, lines[i], rightPanelX + 10.0f, 430.0f + (float)i * 34.0f,
                        20, rightPanelW - 20.0f, sf::Color::White);
    }
}

void Renderer::drawCompletionPanel(sf::RenderWindow &w, HospitalSim *sim)
{
    sf::RectangleShape panel(sf::Vector2f(rightPanelW, 360.0f));
    panel.setPosition(rightPanelX, 360.0f);
    panel.setFillColor(sf::Color(20, 20, 20));
    w.draw(panel);
    drawPanelTitle(w, "SIMULATION COMPLETE", rightPanelX + 10.0f, 376.0f);

    if (font.getInfo().family.empty() || sim == 0)
    {
        return;
    }

    int emUse = (int)(sim->getDoctorUtilisation(0) + 0.5);
    int genUse = (int)(sim->getDoctorUtilisation(1) + 0.5);
    int specUse = (int)(sim->getDoctorUtilisation(2) + 0.5);

    int seen0 = sim->getDoctorSeenToday(0);
    int seen1 = sim->getDoctorSeenToday(1);
    int seen2 = sim->getDoctorSeenToday(2);
    int busiestIndex = 0;
    int busiestCount = seen0;
    if (seen1 > busiestCount)
    {
        busiestIndex = 1;
        busiestCount = seen1;
    }
    if (seen2 > busiestCount)
    {
        busiestIndex = 2;
        busiestCount = seen2;
    }

    int critical = sim->getCountCritical();
    int urgent = sim->getCountUrgent();
    int normal = sim->getCountNormal();
    int commonCount = critical;
    const char *commonLabel = "CRITICAL";
    if (urgent > commonCount)
    {
        commonCount = urgent;
        commonLabel = "URGENT";
    }
    if (normal > commonCount)
    {
        commonCount = normal;
        commonLabel = "NORMAL";
    }

    int totalCases = critical + urgent + normal;
    int commonPct = (totalCases > 0) ? (int)((double)commonCount * 100.0 / (double)totalCases + 0.5) : 0;
    int onTimePct = (sim->getTotalPatients() > 0)
                        ? (int)((double)sim->getPatientsOnTime() * 100.0 / (double)sim->getTotalPatients() + 0.5)
                        : 0;

    char lines[11][180];
    std::snprintf(lines[0], 180, "Average Patient Wait: %.1fs", sim->getAverageWait());
    std::snprintf(lines[1], 180, "Longest Wait Today: %.1fs", sim->getLongestWait());
    std::snprintf(lines[2], 180, "Priority Upgrades (Triage): %d", sim->getEscalationCount());
    std::snprintf(lines[3], 180, "Dr. Anjali (Emergency) - Time Occupied: %d%%", emUse);
    std::snprintf(lines[4], 180, "Dr. Mahesh (General) - Time Occupied: %d%%", genUse);
    std::snprintf(lines[5], 180, "Dr. Rahul (Specialist) - Time Occupied: %d%%", specUse);
    std::snprintf(lines[6], 180, "Busiest Doctor: %s (%d patients)", sim->getDoctorName(busiestIndex), busiestCount);
    std::snprintf(lines[7], 180, "Most Common Case: %s (%d%%)", commonLabel, commonPct);
    std::snprintf(lines[8], 180, "Patients Seen on Time (waited < 30s): %d%%", onTimePct);
    std::snprintf(lines[9], 180, "Peak Waiting Room Size: %d patients", sim->getPeakWaitingRoom());
    if (emUse > 80)
    {
        std::snprintf(lines[10], 180, "Recommendation: Consider adding 1 more Emergency doctor");
    }
    else
    {
        std::snprintf(lines[10], 180, "Recommendation: Emergency load is manageable");
    }

    for (int i = 0; i < 11; i++)
    {
        drawTrimmedText(w, font, lines[i], rightPanelX + 10.0f, 416.0f + (float)i * 27.0f,
                        15, rightPanelW - 20.0f, sf::Color::White);
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

    sf::RectangleShape sep(sf::Vector2f(1.0f, 720.0f));
    sep.setFillColor(sf::Color::White);
    sep.setPosition(385.0f, 0.0f);
    w.draw(sep);
    sep.setPosition(785.0f, 0.0f);
    w.draw(sep);

    drawEventLogPanel(w, sim);
    if (simulationComplete)
    {
        drawCompletionPanel(w, sim);
    }
    else
    {
        drawStatsPanel(w, sim);
    }
}