#include "game.h"
#include <allegro5/bitmap.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_native_dialog.h>

// Constructor
game::game(){

      tile = tools::load_bitmap_ex("images/tiles/Grass.png");


}

// Destructor
game::~game(){


}
void game::update(){

}

// Draw to screen
void game::draw(){
  // Background
  al_clear_to_color( al_map_rgb(40,40,60));

  al_draw_bitmap(tile,300,300,0);



}
