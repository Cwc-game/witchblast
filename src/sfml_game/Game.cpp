/**  This file is part of Witch Blast.
  *
  *  Witch Blast is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 3 of the License, or
  *  (at your option) any later version.
  *
  *  Witch Blast is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with Witch Blast.  If not, see <http://www.gnu.org/licenses/>.
  */
  

#include "Game.h"

#include "GameEntity.h"
#include "EntityManager.h"

#include <iostream>

Game::Game()
{
}

float Game::create(int screenWidth, int screenHeight, std::string windowsTitle, bool fullScreen, bool vsync)
{
  this->screenWidth = screenWidth;
  this->screenHeight = screenHeight;

  float offset = 0;
#if defined(SFML_SYSTEM_ANDROID)
  float ratio = float(sf::VideoMode::getDesktopMode().width) / float(sf::VideoMode::getDesktopMode().height);
  this->screenWidth = screenHeight * ratio;
  //offset = 
  sf::err() << " ratio " << ratio << std::endl;
#endif

  sf::err() << " fullScreen " << fullScreen << std::endl;
  sf::err() << " this->screenWidth "  << this->screenWidth << std::endl;
  sf::err() << " this->screenHeight " << this->screenHeight << std::endl;
  sf::err() << " sf::VideoMode::getDesktopMode().width " << sf::VideoMode::getDesktopMode().width  << std::endl;
  sf::err() << " sf::VideoMode::getDesktopMode().height " << sf::VideoMode::getDesktopMode().height << std::endl;

  if (fullScreen){
    app = new sf::RenderWindow(sf::VideoMode(this->screenWidth, this->screenHeight), windowsTitle, sf::Style::Fullscreen);
  }else{
    app = new sf::RenderWindow(sf::VideoMode(this->screenWidth, this->screenHeight), windowsTitle);
  }

  sf::err() << " app->getDefaultView.getSize.x " << app->getDefaultView().getSize().x << std::endl;
  sf::err() << " app->getDefaultView.getSize.y " << app->getDefaultView().getSize().y << std::endl;
  //app->getDefaultView.getSize.width;

#if defined(SFML_SYSTEM_ANDROID)
/*
 float offratio = float(app->getDefaultView().getSize().y)  / float(screenHeight);
 float realWidth = float(screenWidth) * offratio;
 offset = (realWidth - float(screenWidth))/ 2.0;
*/
  float diff = float(app->getDefaultView().getSize().x) - float(screenWidth);
  offset = diff / 2.0;

// sf::err() << " offratio " << offratio << std::endl;
 sf::err() << " offset " << offset << std::endl;

#endif



  if (vsync) app->setVerticalSyncEnabled(true);
  else app->setFramerateLimit(60);
  
  return offset;
}

Game::~Game()
{
  printf("Deleting the game...\n");
  delete(app);
}

float Game::getAbsolutTime()
{
  static sf::Clock clock;
  return clock.getElapsedTime().asSeconds();
}

void Game::startGame()
{
  lastTime = getAbsolutTime();

  // Start game loop
  while (app->isOpen())
  {
    // Process events
    sf::Event event;

    while (app->pollEvent(event))
    {
      // Close window : exit
      if (event.type == sf::Event::Closed)
        app->close();

    }

    onUpdate();
    onRender();
  }

  quitGame();
}

void Game::quitGame()
{
}

void Game::onRender()
{
  // clear the view
  app->clear(sf::Color(32, 32, 32));

  // render the game objects
  EntityManager::getInstance().render(app);

  app->display();
}

void Game::onUpdate()
{
  float delta = getAbsolutTime() - lastTime;
  lastTime = getAbsolutTime();
  EntityManager::getInstance().animate(delta);
}

