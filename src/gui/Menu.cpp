#include "Menu.h"

#include <cstdio>
#include <iostream>

Menu::Menu()
{
  selectedField = 0;
  fontLoaded = font.loadFromFile("assets/font.ttf");
  if (!fontLoaded)
  {
    std::cerr << "Error: failed to load assets/font.ttf\n";
  }
}

bool Menu::isReady() const
{
  return fontLoaded;
}

int Menu::getFieldValue(const SimConfig &config, int index) const
{
  if (index == 0)
  {
    return config.criticalDoctors;
  }
  if (index == 1)
  {
    return config.generalDoctors;
  }
  if (index == 2)
  {
    return config.specialistDoctors;
  }
  if (index == 3)
  {
    return config.duration;
  }
  return config.arrivalRate;
}

void Menu::setFieldValue(SimConfig &config, int index, int value)
{
  int minValue = 1;
  int maxValue = 120;
  if (index < 3)
  {
    maxValue = 20;
  }
  if (index == 3)
  {
    minValue = 60;
    maxValue = 7200;
  }
  if (value < minValue)
  {
    value = minValue;
  }
  if (value > maxValue)
  {
    value = maxValue;
  }

  if (index == 0)
  {
    config.criticalDoctors = value;
  }
  else if (index == 1)
  {
    config.generalDoctors = value;
  }
  else if (index == 2)
  {
    config.specialistDoctors = value;
  }
  else if (index == 3)
  {
    config.duration = value;
  }
  else
  {
    config.arrivalRate = value;
  }
}

void Menu::changeField(SimConfig &config, int index, int delta)
{
  setFieldValue(config, index, getFieldValue(config, index) + delta);
}

SimConfig Menu::show(sf::RenderWindow &window)
{
  SimConfig config;
  config.criticalDoctors = 2;
  config.generalDoctors = 2;
  config.specialistDoctors = 2;
  config.duration = 600;
  config.arrivalRate = 8;
  config.fastMode = false;

  if (!fontLoaded)
  {
    config.duration = 0;
    return config;
  }

  const int fieldCount = 6;
  const char *labels[fieldCount] = {
      "Critical Doctors",
      "General Doctors",
      "Specialist Doctors",
      "Simulation Duration (seconds)",
      "Seconds Between Patient Arrivals",
      "Simulation Speed"};
  const char *hints[fieldCount] = {
      "(use LEFT/RIGHT arrows to change)",
      "(use LEFT/RIGHT arrows to change)",
      "(use LEFT/RIGHT arrows to change)",
      "(use LEFT/RIGHT arrows to change)",
      "(use LEFT/RIGHT arrows to change)",
      "(use LEFT/RIGHT arrows to toggle)"};

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        window.close();
        config.duration = 0;
        return config;
      }
      if (event.type != sf::Event::KeyPressed)
      {
        continue;
      }

      sf::Keyboard::Key key = event.key.code;
      if (key == sf::Keyboard::Up)
      {
        selectedField = (selectedField + fieldCount - 1) % fieldCount;
      }
      else if (key == sf::Keyboard::Down)
      {
        selectedField = (selectedField + 1) % fieldCount;
      }
      else if (key == sf::Keyboard::Left)
      {
        if (selectedField < 5)
        {
          changeField(config, selectedField, -1);
        }
        else
        {
          config.fastMode = !config.fastMode;
        }
      }
      else if (key == sf::Keyboard::Right)
      {
        if (selectedField < 5)
        {
          changeField(config, selectedField, 1);
        }
        else
        {
          config.fastMode = !config.fastMode;
        }
      }
      else if (key == sf::Keyboard::Enter)
      {
        return config;
      }
      else if (key == sf::Keyboard::Escape)
      {
        config.duration = 0;
        return config;
      }
      else if (selectedField < 5)
      {
        int digit = -1;
        if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9)
        {
          digit = (int)key - (int)sf::Keyboard::Num0;
        }
        else if (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9)
        {
          digit = (int)key - (int)sf::Keyboard::Numpad0;
        }
        if (digit >= 0)
        {
          int value = getFieldValue(config, selectedField);
          setFieldValue(config, selectedField, value * 10 + digit);
        }
        if (key == sf::Keyboard::BackSpace)
        {
          int value = getFieldValue(config, selectedField);
          setFieldValue(config, selectedField, value / 10);
        }
      }
    }

    window.clear(sf::Color(26, 26, 46));

    sf::Text title("Hospital Emergency Department Simulator", font, 30);
    title.setFillColor(sf::Color::White);
    title.setPosition(60.0f, 24.0f);
    window.draw(title);

    sf::Text subtitle("Configure your simulation below", font, 20);
    subtitle.setFillColor(sf::Color(200, 200, 200));
    subtitle.setPosition(60.0f, 62.0f);
    window.draw(subtitle);

    for (int i = 0; i < fieldCount; i++)
    {
      float y = 120.0f + (float)i * 85.0f;

      sf::Text label(labels[i], font, 24);
      label.setFillColor(sf::Color::White);
      label.setPosition(60.0f, y);
      window.draw(label);

      sf::RectangleShape valueBox(sf::Vector2f(320.0f, 36.0f));
      valueBox.setPosition(820.0f, y - 2.0f);
      valueBox.setFillColor(sf::Color(40, 40, 65));
      valueBox.setOutlineThickness(2.0f);
      valueBox.setOutlineColor((selectedField == i) ? sf::Color::Yellow : sf::Color(120, 120, 140));
      window.draw(valueBox);

      char valueText[96];
      if (i < 5)
      {
        std::snprintf(valueText, 96, "%d", getFieldValue(config, i));
      }
      else
      {
        std::snprintf(valueText, 96, "%s", config.fastMode ? "FAST (100ms)" : "NORMAL (500ms)");
      }
      sf::Text value(valueText, font, 22);
      value.setFillColor(sf::Color::White);
      value.setPosition(835.0f, y + 2.0f);
      window.draw(value);

      sf::Text hint(hints[i], font, 16);
      hint.setFillColor(sf::Color(170, 170, 170));
      hint.setPosition(60.0f, y + 34.0f);
      window.draw(hint);
    }

    sf::Text startText("Press ENTER to begin simulation", font, 24);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(60.0f, 610.0f);
    window.draw(startText);

    sf::Text controls("UP/DOWN to select field | LEFT/RIGHT to change value | ENTER to start", font, 18);
    controls.setFillColor(sf::Color(190, 190, 190));
    controls.setPosition(60.0f, 650.0f);
    window.draw(controls);

    window.display();
  }

  config.duration = 0;
  return config;
}
