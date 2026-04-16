#include "Renderer.h"

#include <cstdio>
#include <cstring>
#include <iostream>

static const float leftPanelX = 0.0f;
static const float leftPanelW = 290.0f;
static const float middlePanelX = 291.0f;
static const float middlePanelW = 260.0f;
static const float rightPanelX = 552.0f;
static const float rightPanelW = 728.0f;

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

Renderer::Renderer(Window *w)
{
    win = w;
    ready = false;
    simulationComplete = false;
    csvSaved = false;
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

void Renderer::setSimulationComplete(bool done)
{
    simulationComplete = done;
}

void Renderer::setCsvSaved(bool saved)
{
    csvSaved = saved;
}

void Renderer::drawPanelTitle(sf::RenderWindow &w, const char *text, float x, float y)
{
    if (font.getInfo().family.empty())
    {
        return;
    }
    sf::Text t(text, font, 24);
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
    drawPanelTitle(w, "WAITING ROOM", leftPanelX + 10.0f, 16.0f);

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
            label = "SPECIFIC";
        }

        char row[160];
        std::snprintf(row, 160, "P#%d | %-8s | %ds wait", ids[i], label, waits[i]);
        drawTrimmedText(w, font, row, leftPanelX + 34.0f, y + 2.0f, 14, leftPanelW - 40.0f, sf::Color::White);
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
    drawPanelTitle(w, "TREATMENT", middlePanelX + 10.0f, 16.0f);
    if (font.getInfo().family.empty())
    {
        return;
    }

    int boxCount = (sim == 0) ? 3 : sim->getDoctorBoxCount();
    if (boxCount < 3)
    {
        boxCount = 3;
    }
    if (boxCount > 6)
    {
        boxCount = 6;
    }

    for (int i = 0; i < boxCount; i++)
    {
        float x = middlePanelX + 8.0f;
        float y = 66.0f + (float)i * 108.0f;

        int patientId = (sim == 0) ? -1 : sim->getDoctorBoxPatientByIndex(i);
        bool busy = patientId >= 0;

        const char *doctorName = (sim == 0) ? "UNKNOWN" : sim->getDoctorBoxNameByIndex(i);
        const char *roleName = (sim == 0) ? "Unknown" : sim->getDoctorBoxRoleByIndex(i);
        int seenToday = (sim == 0) ? 0 : sim->getDoctorBoxSeenByIndex(i);

        sf::RectangleShape card(sf::Vector2f(middlePanelW - 16.0f, 100.0f));
        card.setPosition(x, y);
        card.setFillColor(sf::Color(32, 32, 40));
        card.setOutlineThickness(2.0f);
        card.setOutlineColor(busy ? sf::Color::Yellow : sf::Color::Green);
        w.draw(card);

        char line0[96];
        std::snprintf(line0, 96, "%s (%s)", doctorName, roleName);
        drawTrimmedText(w, font, line0, x + 6.0f, y + 8.0f, 12, middlePanelW - 28.0f, sf::Color::White);

        if (!busy)
        {
            drawTrimmedText(w, font, "Status: Free", x + 6.0f, y + 38.0f, 12, middlePanelW - 28.0f, sf::Color::White);
        }
        else
        {
            char status[64];
            std::snprintf(status, 64, "Status: Treating Patient #%d", patientId);
            drawTrimmedText(w, font, status, x + 6.0f, y + 38.0f, 12, middlePanelW - 28.0f, sf::Color::White);
        }

        char treated[48];
        std::snprintf(treated, 48, "Treated: %d", seenToday);
        drawTrimmedText(w, font, treated, x + 6.0f, y + 68.0f, 12, middlePanelW - 28.0f, sf::Color(220, 220, 220));
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

    if (win != 0 && win->consumeUpPress())
    {
        scrollOffset++;
    }
    if (win != 0 && win->consumeDownPress())
    {
        scrollOffset--;
    }

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

    for (int i = 0; i < 13; i++)
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
        drawTrimmedText(w, font, line, rightPanelX + 8.0f, 54.0f + (float)i * 23.0f,
                        10, rightPanelW - 8.0f, color);
    }
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

    char lines[6][160];
    int lineCount = 0;
    std::snprintf(lines[lineCount], 160, "Total Patients: %d", sim->getTotalPatients());
    lineCount++;
    std::snprintf(lines[lineCount], 160, "Currently Waiting: %d", sim->getTotalWaiting());
    lineCount++;
    std::snprintf(lines[lineCount], 160, "Avg Wait Time: %.1f s", sim->getAverageWait());
    lineCount++;
    std::snprintf(lines[lineCount], 160, "Escalations: %d", sim->getEscalationCount());
    lineCount++;
    std::snprintf(lines[lineCount], 160, "Current Sim Time: %.1f s", sim->getCurrentSimTime());
    lineCount++;

    for (int i = 0; i < lineCount; i++)
    {
        drawTrimmedText(w, font, lines[i], rightPanelX + 10.0f, 420.0f + (float)i * 26.0f,
                        16, rightPanelW - 20.0f, sf::Color::White);
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

    int boxCount = sim->getDoctorBoxCount();
    int totalSeen = 0;
    for (int i = 0; i < boxCount; i++)
    {
        totalSeen += sim->getDoctorBoxSeenByIndex(i);
    }

    int critical = sim->getCountCritical();
    int urgent = sim->getCountUrgent();
    int normal = sim->getCountNormal();
    int commonCount = critical;
    const char *commonLabel = "CRITICAL";
    if (urgent > commonCount)
    {
        commonCount = urgent;
        commonLabel = "SPECIFIC";
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

    char lines[20][180];
    int lineCount = 0;
    std::snprintf(lines[0], 180, "Average Patient Wait: %.1fs", sim->getAverageWait());
    lineCount++;
    std::snprintf(lines[lineCount], 180, "Longest Wait Today: %.1fs", sim->getLongestWait());
    lineCount++;
    std::snprintf(lines[lineCount], 180, "Priority Upgrades (Triage): %d", sim->getEscalationCount());
    lineCount++;
    std::snprintf(lines[lineCount], 180, "Most Common Case: %s (%d%%)", commonLabel, commonPct);
    lineCount++;
    std::snprintf(lines[lineCount], 180, "Patients Seen on Time (waited < 30s): %d%%", onTimePct);
    lineCount++;
    std::snprintf(lines[lineCount], 180, "Peak Waiting Room Size: %d patients", sim->getPeakWaitingRoom());
    lineCount++;

    int usedPct = 0;
    int usedSeen = 0;
    for (int i = 0; i < boxCount && lineCount < 20; i++)
    {
        int seen = sim->getDoctorBoxSeenByIndex(i);
        int pct = 0;
        if (totalSeen > 0)
        {
            if (i == boxCount - 1)
            {
                pct = 100 - usedPct;
            }
            else
            {
                int remSeen = totalSeen - usedSeen;
                int remPct = 100 - usedPct;
                pct = (remSeen > 0) ? (seen * remPct) / remSeen : 0;
            }
        }
        usedPct += pct;
        usedSeen += seen;
        std::snprintf(lines[lineCount], 180, "%s : %d%% patients seen", sim->getDoctorBoxNameByIndex(i), pct);
        lineCount++;
    }

    if (csvSaved)
    {
        std::snprintf(lines[lineCount], 180, "Results saved in results.csv");
    }
    else
    {
        std::snprintf(lines[lineCount], 180, "CSV save failed: check folder write permission");
    }
    lineCount++;
    std::snprintf(lines[lineCount], 180, "Press ESC to exit the application");
    lineCount++;

    for (int i = 0; i < lineCount; i++)
    {
        drawTrimmedText(w, font, lines[i], rightPanelX + 10.0f, 404.0f + (float)i * 20.0f,
                        14, rightPanelW - 20.0f, sf::Color::White);
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
    sep.setPosition(290.0f, 0.0f);
    w.draw(sep);
    sep.setPosition(551.0f, 0.0f);
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