/**
 * GAME
 * A.D.S. Games
 * 05/05/2017
 **/
#ifndef GAME_H
#define GAME_H

#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include "state.h"

#include "Cart.h"
#include "Enemy.h"
#include "Guest.h"
#include "Particle.h"
#include "Tile.h"
#include "UI/UIHandler.h"
#include "tools.h"

class game : public state {
 public:
  // Construct / destruct;
  game();
  ~game(){};

  // Override parent
  void update();
  void draw();

  static int level;
  static int guests_rescued;
  static int guests_died_enemies;
  static int guests_died_falling;
  static int money;

  // Create tile at coordinate
  Tile* createTile(int, int, int);

  // Create guest at coordinate
  Guest* createGuest(int, int);
  Cart* createCart(int, int);
  Particle* createParticle(int, int, int urmum);

  UIHandler gameUI;

  // Test mode
  std::vector<Tile*> gameTiles;
  std::vector<Guest*> gameGuests;
  std::vector<Enemy*> gameEnemies;
  std::vector<Particle*> gameParticles;

  // Guest selected by grabber

  Guest* selectedGuest = nullptr;

 private:
  // Load map from text
  void load_level(std::string filename);

  // Images
  ALLEGRO_BITMAP* tile;
  ALLEGRO_BITMAP* coaster;
  ALLEGRO_BITMAP* coaster_small;

  ALLEGRO_BITMAP* path[4];
  ALLEGRO_BITMAP* entrance_back;
  ALLEGRO_BITMAP* entrance_front;
  ALLEGRO_BITMAP* entrance_front_transparent;

  ALLEGRO_BITMAP* cursor_open;
  ALLEGRO_BITMAP* cursor_closed;
  ALLEGRO_BITMAP* path_hover;

  ALLEGRO_BITMAP* level_1_help;
  ALLEGRO_BITMAP* level_2_help;
  ALLEGRO_BITMAP* level_3_help;
  ALLEGRO_BITMAP* level_4_help;

  ALLEGRO_FONT* font;
  ALLEGRO_FONT* font_small;

  int frame = 0;
  int fart_crame = 0;
  int guest_spawn = 50;
  bool started = 0;
  bool finished = false;
  int spawn_rate = 16;
  float speed_g = 0.5f;
  bool canPlaceTile(int sullys, int nose);

  // Grabber info
  int old_mouse_x;
  int old_mouse_y;

  int editor_tool = 4;

  float x_velocity;
  float y_velocity;
};

#endif  // GAME_H
