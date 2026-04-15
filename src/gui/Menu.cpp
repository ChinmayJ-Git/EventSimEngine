#include "Menu.h"

#include <iostream>
#include <string>

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

  const char *labels[6] = {
      "Critical Doctors",
      "General Doctors",
      "Specialist Doctors",
      "Simulation Duration (seconds)",
      "Seconds Between Patient Arrivals",
      "Simulation Speed"};

  sf::RectangleShape startButton(sf::Vector2f(420.0f, 60.0f));
  startButton.setPosition(430.0f, 640.0f);
  startButton.setFillColor(sf::Color(30, 170, 70));

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

      if (event.type == sf::Event::KeyPressed)
      {
        if (event.key.code == sf::Keyboard::Up)
        {
          selectedField = (selectedField + 6) % 7;
        }
        if (event.key.code == sf::Keyboard::Down)
        {
          selectedField = (selectedField + 1) % 7;
        }
        if (selectedField < 5 && event.key.code == sf::Keyboard::Left)
        {
          changeField(config, selectedField, -1);
        }
        if (selectedField < 5 && event.key.code == sf::Keyboard::Right)
        {
          changeField(config, selectedField, 1);
        }
        if (selectedField == 5 && (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Enter))
        {
          config.fastMode = !config.fastMode;
        }
        if (selectedField == 6 && event.key.code == sf::Keyboard::Enter)
        {
          return config;
        }
        if (event.key.code == sf::Keyboard::Escape)
        {
          config.duration = 0;
          return config;
        }

        if (selectedField < 5)
        {
          int digit = -1;
          if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9)
          {
            digit = (int)event.key.code - (int)sf::Keyboard::Num0;
          }
          if (event.key.code >= sf::Keyboard::Numpad0 && event.key.code <= sf::Keyboard::Numpad9)
          {
            digit = (int)event.key.code - (int)sf::Keyboard::Numpad0;
          }
          if (digit >= 0)
          {
            int value = getFieldValue(config, selectedField);
            setFieldValue(config, selectedField, value * 10 + digit);
          }
          if (event.key.code == sf::Keyboard::BackSpace)
          {
            int value = getFieldValue(config, selectedField);
            setFieldValue(config, selectedField, value / 10);
          }
        }
      }

      if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
      {
        float mx = (float)event.mouseButton.x;
        float my = (float)event.mouseButton.y;
        for (int i = 0; i < 6; i++)
        {
          float y = 150.0f + (float)i * 75.0f;
          if (mx >= 90.0f && mx <= 1180.0f && my >= y - 8.0f && my <= y + 42.0f)
          {
            selectedField = i;
          }
        }
        if (mx >= 430.0f && mx <= 850.0f && my >= 640.0f && my <= 700.0f)
        {
          return config;
        }
      }
    }

    window.clear(sf::Color(12, 15, 24));

    sf::Text title("HOSPITAL EMERGENCY SIMULATOR", font, 46);
    title.setFillColor(sf::Color::White);
    title.setPosition(160.0f, 36.0f);
    window.draw(title);

    for (int i = 0; i < 6; i++)
    {
      float y = 150.0f + (float)i * 75.0f;
      sf::Color color = (selectedField == i) ? sf::Color::Yellow : sf::Color::White;
      sf::Text label(labels[i], font, 28);
      label.setFillColor(color);
      label.setPosition(90.0f, y);
      window.draw(label);

      std::string valueText;
      if (i < 5)
      {
        valueText = std::to_string(getFieldValue(config, i));
      }
      else
      {
        valueText = config.fastMode ? "FAST (100ms per event)" : "NORMAL (500ms per event)";
      }
      sf::Text value(valueText, font, 28);
      value.setFillColor(color);
      value.setPosition(730.0f, y);
      window.draw(value);
    }

    if (selectedField == 6)
    {
      startButton.setOutlineThickness(3.0f);
      startButton.setOutlineColor(sf::Color::Yellow);
    }
    else
    {
      startButton.setOutlineThickness(0.0f);
    }
    window.draw(startButton);

    sf::Text startText("START SIMULATION", font, 30);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(495.0f, 652.0f);
    window.draw(startText);

    sf::Text hint("Arrow keys to move/edit, Enter to toggle speed/start, Esc to cancel", font, 20);
    hint.setFillColor(sf::Color(185, 185, 185));
    hint.setPosition(170.0f, 605.0f);
    window.draw(hint);

    window.display();
  }

  config.duration = 0;
  return config;
}
