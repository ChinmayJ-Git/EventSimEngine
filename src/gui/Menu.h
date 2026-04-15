#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>

struct SimConfig
{
    int criticalDoctors;
    int generalDoctors;
    int specialistDoctors;
    int duration;
    int arrivalRate;
    bool fastMode;
};

class Menu
{
public:
    Menu();
    bool isReady() const;
    SimConfig show(sf::RenderWindow &window);

private:
    sf::Font font;
    bool fontLoaded;
    int selectedField;

    int getFieldValue(const SimConfig &config, int index) const;
    void setFieldValue(SimConfig &config, int index, int value);
    void changeField(SimConfig &config, int index, int delta);
};

#endif