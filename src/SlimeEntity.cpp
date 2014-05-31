#include "SlimeEntity.h"
#include "PlayerEntity.h"
#include "sfml_game/SpriteEntity.h"
#include "sfml_game/ImageManager.h"
#include "sfml_game/SoundManager.h"
#include "Constants.h"
#include "WitchBlastGame.h"

SlimeEntity::SlimeEntity(float x, float y, bool invocated)
  : EnnemyEntity (ImageManager::getImageManager()->getImage(IMAGE_SLIME), x, y)
{
  creatureSpeed = 0.0f;
  velocity = Vector2D(0.0f, 0.0f);
  hp = SLIME_HP;
  meleeDamages = SLIME_DAMAGES;

  this->invocated = invocated;
  if (invocated)
  {
    type = ENTITY_ENNEMY_INVOCATED;
    jumpingDelay = 0.0f;
    age = 0.0f;
  }
  else
  {
    type = ENTITY_ENNEMY;
    jumpingDelay = 0.6f + 0.1f * (rand() % 20);
  }

  bloodColor = bloodGreen;
  frame = 0;
  shadowFrame = 3;

  isJumping = false;
  h = 0.0f;

  viscosity = 0.98f;
}

void SlimeEntity::animate(float delay)
{
  float slimeDelay = delay;
  if (specialState[SpecialStateIce].active) slimeDelay = delay * STATUS_FROZEN_MULT;

  if (isJumping)
  {
    hVelocity -= 700.0f * slimeDelay;

    h += hVelocity * slimeDelay;

    if (h <= 0.0f)
    {
      if (hp <= 0)
        dying();
      else
      {
        h = 0.0f;
        if (isFirstJumping)
        {
          isFirstJumping = false;
          hVelocity = 160.0f;
          SoundManager::getSoundManager()->playSound(SOUND_SLIME_IMAPCT);
        }
        else
        {
          jumpingDelay = 0.4f + 0.1f * (rand() % 20);
          isJumping = false;
          SoundManager::getSoundManager()->playSound(SOUND_SLIME_IMAPCT_WEAK);
        }
      }
    }
    if (hVelocity > 0.0f) frame = 2;
    else frame = 0;
  }
  else
  {
    jumpingDelay -= slimeDelay;
    if (jumpingDelay < 0.0f)
    {
      SoundManager::getSoundManager()->playSound(SOUND_SLIME_JUMP);
      hVelocity = 350.0f + rand() % 300;
      isJumping = true;
      isFirstJumping = true;

      float randVel = 250.0f + rand() % 250;

      if (rand() % 2 == 0)
      {
        float tan = (game().getPlayer()->getX() - x) / (game().getPlayer()->getY() - y);
        float angle = atan(tan);

        if (game().getPlayer()->getY() > y)
          setVelocity(Vector2D(sin(angle) * randVel,
                                     cos(angle) * randVel));
        else
          setVelocity(Vector2D(-sin(angle) * randVel,
                                     -cos(angle) * randVel));
      }
      else
        velocity = Vector2D(randVel);
    }
    else if (jumpingDelay < 0.25f)
      frame = 1;
    else frame = 0;
  }

  EnnemyEntity::animate(delay);
}

void SlimeEntity::render(sf::RenderTarget* app)
{
  if (!isDying && shadowFrame > -1)
  {
    // shadow
    sprite.setPosition(x, y);
    sprite.setTextureRect(sf::IntRect(shadowFrame * width, 0, width, height));
    app->draw(sprite);
  }
  sprite.setPosition(x, y - h);
  sprite.setTextureRect(sf::IntRect(frame * width, 0, width, height));
  app->draw(sprite);

  #ifdef SHOW_BOUNDING_BOX
  sf::Vertex line[] =
  {
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top + boundingBox.height)),
      sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top))
  };
  app->draw(line, 8, sf::Lines);
  #endif
}

void SlimeEntity::calculateBB()
{
    boundingBox.left = (int)x - width / 2 + SLIME_BB_LEFT;
    boundingBox.width = width - SLIME_BB_WIDTH_DIFF;
    boundingBox.top = (int)y - height / 2 + SLIME_BB_TOP;
    boundingBox.height =  height - SLIME_BB_HEIGHT_DIFF;
}

void SlimeEntity::collideMapRight()
{
 // if (x > OFFSET_X + MAP_WIDTH * TILE_WIDTH)
    velocity.x = -velocity.x * 0.8f;
}

void SlimeEntity::collideMapLeft()
{
 // if (x < OFFSET_X + MAP_WIDTH )
    velocity.x = -velocity.x * 0.8f;
}

void SlimeEntity::collideMapTop()
{
//  if (y > OFFSET_Y + MAP_HEIGHT * TILE_HEIGHT)
    velocity.y = -velocity.y * 0.8f;
}

void SlimeEntity::collideMapBottom()
{
 // if (y < OFFSET_Y + MAP_HEIGHT )
    velocity.y = -velocity.y * 0.8f;
}

void SlimeEntity::collideWithEnnemy(GameEntity* collidingEntity)
{
  if (recoil.active && recoil.stun) return;

  EnnemyEntity* entity = static_cast<EnnemyEntity*>(collidingEntity);
  if (entity->getMovingStyle() == movWalking)
  {
    Vector2D vel = Vector2D(entity->getX(), entity->getY()).vectorTo(Vector2D(x, y), 100.0f );
    giveRecoil(false, vel, 0.3f);

    computeFacingDirection();
  }
}

bool SlimeEntity::collideWithMap(int direction)
{
    calculateBB();

    int xTile0 = (boundingBox.left - offsetX) / tileWidth;
    int xTilef = (boundingBox.left + boundingBox.width - offsetX) / tileWidth;
    int yTile0 = (boundingBox.top - offsetY) / tileHeight;
    int yTilef = (boundingBox.top + boundingBox.height - offsetY) / tileHeight;

    if (boundingBox.top < 0) yTile0 = -1;

    for (int xTile = xTile0; xTile <= xTilef; xTile++)
        for (int yTile = yTile0; yTile <= yTilef; yTile++)
        {
          if (xTile == 0 || xTile == MAP_WIDTH - 1 || yTile == 0 || yTile == MAP_HEIGHT - 1)
          {
            switch (direction)
            {
            case DIRECTION_LEFT:
                if (map->isLeftBlocking(xTile, yTile)) return true;
                break;

            case DIRECTION_RIGHT:
                if (map->isRightBlocking(xTile, yTile)) return true;
                break;

            case DIRECTION_TOP:
                if (map->isUpBlocking(xTile, yTile)) return true;
                break;

            case DIRECTION_BOTTOM:
                if (map->isDownBlocking(xTile, yTile)) return true;
                break;
            }
          }
        }

    return false;
}

void SlimeEntity::dying()
{
  isDying = true;
  SpriteEntity* deadSlime = new SpriteEntity(ImageManager::getImageManager()->getImage(IMAGE_CORPSES), x, y, 64, 64);
  deadSlime->setZ(OFFSET_Y);
  deadSlime->setFrame(FRAME_CORPSE_SLIME);
  deadSlime->setType(ENTITY_CORPSE);

  for (int i = 0; i < 4; i++) game().generateBlood(x, y, bloodColor);

  if (!invocated) drop();
  SoundManager::getSoundManager()->playSound(SOUND_ENNEMY_DYING);
}

void SlimeEntity::prepareDying()
{
    if (!isJumping)
      dying();
}

bool SlimeEntity::canCollide()
{
  return h <= 70.0f;
}

BaseCreatureEntity::enumMovingStyle SlimeEntity::getMovingStyle()
{
  if (h <= 70.0f)
    return movWalking;
  else
    return movFlying;
}
