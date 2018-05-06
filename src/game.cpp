#include "game.h"

#include <allegro5/bitmap.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_native_dialog.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iterator>

#include "listeners/mouseListener.h"
#include "listeners/keyListener.h"
#include "listeners/joystickListener.h"
#include "UI/button.h"

int game::level=1;
int game::guests_died_enemies=0;
int game::guests_died_falling=0;
int game::guests_rescued=0;
int game::money=0;

// Constructor
game::game(){

  Guest::speed=0.5;
  game::guests_died_enemies=0;
  game::guests_died_falling=0;
  game::guests_rescued=0;
  game::money=0;
  // Init vars
  old_mouse_x = 0;
  old_mouse_y = 0;

  Message::load();
  Message::clear();

  x_velocity = 0;
  y_velocity = 0;

  selectedGuest = nullptr;

  srand( time(NULL));

  if(level==1){
    load_level("maps/level1.txt");
    guest_spawn=10;
  }
  if(level==2){
    load_level("maps/level2.txt");
    guest_spawn=15;
  }
  if(level==3){
    load_level("maps/level3.txt");
    guest_spawn=70;
  }

  if(level==4){
    load_level("maps/level4.txt");
    guest_spawn=150;
  }


  // Load path images
  path[0] = tools::load_bitmap_ex( "images/tiles/Path_0.png" );
  path[1] = tools::load_bitmap_ex( "images/tiles/Path_1.png" );
  path[2] = tools::load_bitmap_ex( "images/tiles/Path_2.png" );
  path[3] = tools::load_bitmap_ex( "images/tiles/Path_3.png" );
  coaster = tools::load_bitmap_ex( "images/tiles/coaster.png" );
  coaster_small = tools::load_bitmap_ex( "images/tiles/coaster_small.png" );

  level_1_help = tools::load_bitmap_ex( "images/level_1_help.png" );
  level_2_help = tools::load_bitmap_ex( "images/level_2_help.png" );



  // Load font
  font = al_load_ttf_font( "font/font.ttf", 48, 0);
  font_small = al_load_ttf_font( "font/font.ttf", 24, 0);


  // Create buttons
  gameUI.addElement( new Button( 25          , 25, "path_0",  path[0] ));
  gameUI.getElementById("path_0") ->setBackgroundColour( al_map_rgb(100,100,100));


  gameUI.addElement( new Button( 25 + 128    , 25, "path_1",  path[1] ));
  gameUI.getElementById("path_1") ->setBackgroundColour( al_map_rgb(100,100,100));


  gameUI.addElement( new Button( 25 + 128 * 2, 25, "path_2",  path[2] ));
  gameUI.getElementById("path_2") ->setBackgroundColour( al_map_rgb(100,100,100));


  gameUI.addElement( new Button( 25 + 128 * 3, 25, "path_3",  path[3] ));
  gameUI.getElementById("path_3") ->setBackgroundColour( al_map_rgb(100,100,100));

  gameUI.addElement( new UIElement( 25+4+ 128 * 4, 25, "Cost:$100",  font_small ));
  gameUI.getElementByText("Cost:$100") ->setDisableHoverEffect(true);



  gameUI.addElement( new Button( 25  , 25+64, "tweezer", tools::load_bitmap_ex( "images/tweezersButton.png" )));
  gameUI.addElement( new Button( 25  +128 * 1, 25+64, "coaster", coaster_small));
  gameUI.addElement( new Button(  200, 500, "Start Game", font));
  gameUI.addElement( new Button(  200, 500, "Finish", font));

  gameUI.addElement( new Button(  1800, 5, ">>", font));

  gameUI.getElementByText("Finish") -> toggleStatus();

  if(level==1 || level==2){
    gameUI.getElementById("path_0") -> toggleStatus();
        gameUI.getElementById("path_1") -> toggleStatus();

            gameUI.getElementById("path_2") -> toggleStatus();

                gameUI.getElementById("path_3") -> toggleStatus();
                                gameUI.getElementById("tweezer") -> toggleStatus();
                                                                gameUI.getElementById("coaster") -> toggleStatus();
                                                                                                                                gameUI.getElementByText("Cost:$100") -> toggleStatus();





  }
  if(level==1)
                  gameUI.getElementByText(">>") -> toggleStatus();





  // Load images for entrance
  entrance_back = tools::load_bitmap_ex( "images/tiles/EntranceBack.png" );
  entrance_front = tools::load_bitmap_ex( "images/tiles/EntranceFront.png" );
  path_hover = tools::load_bitmap_ex( "images/tiles/Path_Hover.png" );

  // Load grabber images
  cursor_open = tools::load_bitmap_ex( "images/tweezersOpen.png" );
  cursor_closed = tools::load_bitmap_ex( "images/tweezersClosed.png" );

  // Load level

}

// Load map from text
void game::load_level( std::string filename ){
  std::string line;
  std::ifstream myfile( filename.c_str() );

  int i = 0;
  if( myfile.is_open() ){
    while ( getline( myfile, line ) ){
      std::istringstream buf( line );
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> tokens( beg, end );


      // your moms a joffset
      int joffset=12;



      int j = 0;
      for( auto& s: tokens){
        if( s == "100" ){
          gameEnemies.push_back( new Enemy( i, j - joffset) );
          gameTiles.push_back( createTile( i, j - joffset, 0) );

        }else
          gameTiles.push_back( createTile( i, j - joffset, tools::convertStringToInt(s) ) );
        j++;
      }
      i++;
    }
    myfile.close();
  }
}

// Update
void game::update(){

  Guest::speed=0.4f;
  spawn_rate=16;
  if(gameUI.getElementByText(">>") ->held()){
    Guest::speed=2;
    spawn_rate=4;

  }


  if(money>=100){

    gameUI.getElementById("path_0") ->setBackgroundColour( al_map_rgb(0,220,0));
    gameUI.getElementById("path_0") ->setDisableHoverEffect(false);
    gameUI.getElementById("path_0") ->setActive(true);

    gameUI.getElementById("path_1") ->setBackgroundColour( al_map_rgb(0,220,0));
    gameUI.getElementById("path_1") ->setDisableHoverEffect(false);
    gameUI.getElementById("path_1") ->setActive(true);

    gameUI.getElementById("path_2") ->setBackgroundColour( al_map_rgb(0,220,0));
    gameUI.getElementById("path_2") ->setDisableHoverEffect(false);
    gameUI.getElementById("path_2") ->setActive(true);

    gameUI.getElementById("path_3") ->setBackgroundColour( al_map_rgb(0,220,0));
    gameUI.getElementById("path_3") ->setDisableHoverEffect(false);
    gameUI.getElementById("path_3") ->setActive(true);



  }else if(level>2){
    gameUI.getElementById("path_0") ->setBackgroundColour( al_map_rgb(100,100,100));
    gameUI.getElementById("path_0") ->setDisableHoverEffect(true);
    gameUI.getElementById("path_0") ->setActive(false);

    gameUI.getElementById("path_1") ->setBackgroundColour( al_map_rgb(100,100,100));
    gameUI.getElementById("path_1") ->setDisableHoverEffect(true);
    gameUI.getElementById("path_1") ->setActive(false);


    gameUI.getElementById("path_2") ->setBackgroundColour( al_map_rgb(100,100,100));
    gameUI.getElementById("path_2") ->setDisableHoverEffect(true);
    gameUI.getElementById("path_2") ->setActive(false);

    gameUI.getElementById("path_3") ->setBackgroundColour( al_map_rgb(100,100,100));
    gameUI.getElementById("path_3") ->setDisableHoverEffect(true);
    gameUI.getElementById("path_3") ->setActive(false);


  }


  if(level==1 &&  guests_rescued + guests_died_enemies + guests_died_falling == 10 && finished==false){
    finished=true;
    gameUI.getElementByText("Finish") -> toggleStatus();

  }
  if(level==2 &&  guests_rescued + guests_died_enemies + guests_died_falling == 15 && finished==false){
    finished=true;
    gameUI.getElementByText("Finish") -> toggleStatus();

  }

  if(level==3 &&  guests_rescued + guests_died_enemies + guests_died_falling == 70 && finished==false){
    finished=true;
    gameUI.getElementByText("Finish") -> toggleStatus();

  }

  if(level==4 &&  guests_rescued + guests_died_enemies + guests_died_falling == 150 && finished==false){
    finished=true;
    gameUI.getElementByText("Finish") -> toggleStatus();

  }




  for( unsigned int i = 0; i < gameParticles.size(); i++ ){

      if(gameParticles.at(i) -> update())
        gameParticles.erase( gameParticles.begin() + i );

  }

  Message::update();
  gameUI.update();

  if( gameUI.getElementById("path_0") -> clicked() )
    editor_tool = 0;
  if( gameUI.getElementById("path_1") -> clicked() )
    editor_tool = 1;
  if( gameUI.getElementById("path_2") -> clicked() )
    editor_tool = 2;
  if( gameUI.getElementById("path_3") -> clicked() )
    editor_tool = 3;
  if( gameUI.getElementById("tweezer") -> clicked() )
    editor_tool = 4;
   if( gameUI.getElementById("coaster") -> clicked() )
    editor_tool = 5;

  if( gameUI.getElementByText("Start Game") -> clicked() ){
   started=true;

   gameUI.getElementByText("Start Game") ->toggleStatus();
  }
  if( gameUI.getElementByText("Finish") -> clicked() )
    set_next_state(STATE_LEVELFINISH);


  // Velocity of mouse
  x_velocity = -1 * ( old_mouse_x - mouseListener::mouse_x );
  y_velocity = -1 * ( old_mouse_y - mouseListener::mouse_y );

  old_mouse_x = mouseListener::mouse_x;
  old_mouse_y = mouseListener::mouse_y;

  for( unsigned int i = 0; i < gameTiles.size(); i++ ){
    int mxo = mouseListener::mouse_x - 64;
    int myo = mouseListener::mouse_y - 32;

    if( (mouseListener::mouse_button & 1 && !gameUI.isHovering()) &&
        mxo > gameTiles.at(i) -> getIsoX() - 32 && mxo < gameTiles.at(i) -> getIsoX() + 32 &&
        myo < gameTiles.at(i) -> getIsoY() + 32 && myo > gameTiles.at(i) -> getIsoY() - 32 ){

        switch( editor_tool){
        case 0:
          gameTiles.at(i) = createTile( gameTiles.at(i) -> getX(), gameTiles.at(i) -> getY(), 5 );
          money-=100;
            editor_tool=4;
          break;
        case 1:
          gameTiles.at(i) = createTile( gameTiles.at(i) -> getX(), gameTiles.at(i) -> getY(), 6 );
          money-=100;
          editor_tool=4;

            editor_tool=4;
          break;
        case 2:
          gameTiles.at(i) = createTile( gameTiles.at(i) -> getX(), gameTiles.at(i) -> getY(), 7 );
          money-=100;

            editor_tool=4;

          break;
        case 3:
          gameTiles.at(i) = createTile( gameTiles.at(i) -> getX(), gameTiles.at(i) -> getY(), 4 );
                    money-=100;

            editor_tool=4;
          break;
        case 5:
          gameTiles.at(i) = createTile( gameTiles.at(i) -> getX(), gameTiles.at(i) -> getY(), 10 );
          break;
        default:
          break;
      }
    }
  }

  // Release guest
  if( selectedGuest){
    selectedGuest -> setX( mouseListener::mouse_x );
    selectedGuest -> setY( mouseListener::mouse_y );

    if( mouseListener::mouse_released & 1 ){
      selectedGuest -> setCaptured( false );
      selectedGuest -> setVelocityX( x_velocity );
      selectedGuest -> setVelocityY( y_velocity );
      selectedGuest -> setDirection(0 );


      gameGuests.push_back( selectedGuest );
      selectedGuest = nullptr;
    }
  }

  // Enemy logic
  for( unsigned int i = 0; i < gameEnemies.size(); i++ )
    gameEnemies.at(i) -> update();

  // Run guest logic
  // In grabber
  if( selectedGuest != nullptr )
    selectedGuest -> update();

  // Rest of guests
  for( unsigned int i = 0; i < gameGuests.size(); i++ ){
    bool off_map = true;
    bool guest_alive = true;

    bool is_cart = gameGuests.at(i) -> getIsCart();
    int guest_x = gameGuests.at(i) -> getX() + 8;
    int guest_y = gameGuests.at(i) -> getY() + 32;


    gameGuests.at(i) -> update();

    // Pick up guest
    if( !selectedGuest && editor_tool == 4 && !selectedGuest &&
        tools::clicked( gameGuests.at(i) -> getX() - 25,
                        gameGuests.at(i) -> getX() + 25,
                        gameGuests.at(i) -> getY()-30,
                        gameGuests.at(i) -> getY() + 45 )){
      selectedGuest = gameGuests.at(i);
      selectedGuest -> setCaptured( true );
      gameGuests.erase( gameGuests.begin() + i );
      continue;
    }

    // Collision with tiles
    for( unsigned int j = 0; j < gameTiles.size(); j++ ){
    int guest_x = gameGuests.at(i) -> getX() + 8;
        int guest_y = gameGuests.at(i) -> getY() + 32;
      if(gameTiles.at(j) -> colliding_loose( guest_x, guest_y )){
        off_map=false;
      }

      if( guest_alive ){
        // Shorthand

        int current = gameTiles.at(j) -> getType();

        // Collision with map tile
        if( gameTiles.at(j) -> colliding( guest_x, guest_y ) && !is_cart){

          off_map = false;

          //wattaa
          if( current == 8 && !is_cart){
            if(!is_cart){
              std::string stringyboi = gameGuests.at(i) -> getName() + " has died from drowining.";
              Message::sendMessage(stringyboi);
              gameParticles.push_back(createParticle(guest_x,guest_y,1));
            }
            gameGuests.erase( gameGuests.begin() + i );
            guest_alive = false;
            guests_died_falling ++;
            break;
          }
          if( current == 3 && !is_cart){
                        gameParticles.push_back(createParticle(guest_x,guest_y,2));

            gameGuests.erase( gameGuests.begin() + i );
            guest_alive = false;
            guests_rescued ++;

            break;
          }

        }
        if( gameTiles.at(j) -> colliding_loose( guest_x, guest_y ) && !is_cart){
          if( current == 9 ){
            if(gameGuests.at(i) ->giveUmbrella()){
              money+=10;
              gameParticles.push_back(createParticle(guest_x,guest_y,0));
            }
          }

        }



        // Touching special tile
        if( gameTiles.at(j) -> colliding_tight( guest_x, guest_y )){
          // Directional tilesoff_map
          if( current == 4 )
            gameGuests.at(i) -> setDirection(3);
          if( current == 5 )
            gameGuests.at(i) -> setDirection(0);
          if( current == 6 )
            gameGuests.at(i) -> setDirection(1);
          if( current == 7 )
            gameGuests.at(i) -> setDirection(2);

          // End point

          // Water tile
        }
      }
    }

    // Off the edge
    if( off_map ){
        if(!is_cart){
            std::string stringyboi = gameGuests.at(i) -> getName() + " has died from drowining.";
            Message::sendMessage(stringyboi);
            gameParticles.push_back(createParticle(guest_x,guest_y,1));
          }
      gameGuests.erase( gameGuests.begin() + i );
      guests_died_falling++;
      continue;
    }

    // Guest with enemy collision
    if( guest_alive ){
      for( unsigned int j = 0; j < gameEnemies.size(); j++){
        if( tools::collision( gameGuests.at(i) -> getX(),
                              gameGuests.at(i) -> getX() + 16,
                              gameEnemies.at(j) -> getX() + 100,
                              gameEnemies.at(j) -> getX() + 400,
                              gameGuests.at(i) -> getY(),
                              gameGuests.at(i) -> getY() + 54,
                              gameEnemies.at(j) -> getY() + 100,
                              gameEnemies.at(j) -> getY() + 200)){
          gameEnemies.at(j) -> applyDamage(abs(gameGuests.at(i) -> getVelocityX()) + abs(gameGuests.at(i) -> getVelocityY()));

          if(!is_cart){
            std::string stringyboi = gameGuests.at(i) -> getName() + " has died from an angry octopus.";
            Message::sendMessage(stringyboi);
            gameParticles.push_back(createParticle(guest_x,guest_y,1));

          }
          gameGuests.erase( gameGuests.begin() + i );

          guests_died_enemies++;
          break;
        }
      }
    }
  }
  frame++;
  // Spawn guests
  for( unsigned int i = 0; i < gameTiles.size(); i++ ){
    if( gameTiles.at(i) -> getType() == 2 ){
      if( frame>=spawn_rate){
        if(guest_spawn>0 && started){
          gameGuests.push_back( createGuest( gameTiles.at(i) -> getIsoX() + 64 - 8,
                                         gameTiles.at(i) -> getIsoY() + 32 - 20 ));
          guest_spawn--;
        }
        frame=0;

      }

    }
  }

  for( unsigned int i = 0; i < gameTiles.size(); i++ ){
    if( gameTiles.at(i) -> getType() == 10 ){
      if(tools::random_int(1,100)==1){
        gameGuests.push_back( createCart( gameTiles.at(i) -> getIsoX() +32,
                                         gameTiles.at(i) -> getIsoY()-64 ));

      }
    }
  }
}

// Creates a tile at screen coordinate
Tile *game::createTile( int x, int y, int type ){
  Tile *newTile = new Tile( x, y, type );
  return newTile;
}

// Creates a guest at screen coordinate
Guest *game::createGuest( int newX, int newY ){
  Guest *newGuest = new Guest( newX, newY);
  return newGuest;
}

Cart *game::createCart( int newX, int newY ){
  Cart *newCart = new Cart( newX, newY );
  return newCart;
}

Particle *game::createParticle( int newX, int newY,int newtype ){
  Particle *newParticle = new Particle( newX, newY ,newtype);
  return newParticle;
}


// Draw to screen
void game::draw(){
  // Background
  al_clear_to_color( al_map_rgb( 40, 40, 60 ) );

  // Draw tiles
  for( unsigned int i = 0; i < gameTiles.size(); i++ ){
    gameTiles.at(i) -> draw();
  }

  if( keyListener::key[ALLEGRO_KEY_G] ){
    for( unsigned int i = 0; i < gameTiles.size(); i++ ){
      al_draw_rectangle( gameTiles.at(i) -> getIsoX() + 32, gameTiles.at(i) -> getIsoY() + 0, gameTiles.at(i) -> getIsoX() + 96, gameTiles.at(i) -> getIsoY() + 64, al_map_rgb( 99, 33, 00), 1);
    }
  }

  al_draw_bitmap( entrance_back, 129, 640, 0 );

  // Draw guests
  for( unsigned int i = 0; i < gameGuests.size(); i++ )
    gameGuests.at(i) -> draw();

  al_draw_bitmap( entrance_front, 255, 767, 0 );

  // Draw enemies
  for( unsigned int i = 0; i < gameEnemies.size(); i++ )
    gameEnemies.at(i) -> draw();

  gameUI.draw();

  // Modified x and y for isometric conversions
  for( unsigned int i = 0; i < gameTiles.size(); i++ ){
    if( editor_tool >= 0 && editor_tool <= 3 &&
        gameTiles.at(i) -> colliding( mouseListener::mouse_x, mouseListener::mouse_y ) ){
      al_draw_bitmap( path_hover, gameTiles.at(i) -> getIsoX(), gameTiles.at(i) -> getIsoY(), 0);
    }
    if( editor_tool == 5 && gameTiles.at(i) -> colliding( mouseListener::mouse_x, mouseListener::mouse_y ) )
      al_draw_bitmap( coaster, gameTiles.at(i) -> getIsoX()-200, gameTiles.at(i) -> getIsoY()-300, 0);
  }

  // Picked up guest
  if( selectedGuest != nullptr ){
    selectedGuest -> draw();
    al_draw_bitmap( cursor_closed, mouseListener::mouse_x - 8, mouseListener::mouse_y - 56, 0 );
  }


  else if( editor_tool == 4 ){
    al_draw_bitmap( cursor_open, mouseListener::mouse_x - 8, mouseListener::mouse_y - 56, 0 );
  }

  switch( editor_tool){
    case 0:
      al_draw_bitmap( path[0], mouseListener::mouse_x - 64, mouseListener::mouse_y - 32, 0 );
      break;
    case 1:
      al_draw_bitmap( path[1], mouseListener::mouse_x - 64, mouseListener::mouse_y - 32, 0 );
      break;
    case 2:
      al_draw_bitmap( path[2], mouseListener::mouse_x - 64, mouseListener::mouse_y - 32, 0 );
      break;
    case 3:
      al_draw_bitmap( path[3], mouseListener::mouse_x - 64, mouseListener::mouse_y - 32, 0 );
      break;
     case 5:
      al_draw_bitmap( coaster, mouseListener::mouse_x - 238, mouseListener::mouse_y - 319, 0 );
      break;
    default:
      break;
  }
  for( unsigned int i = 0; i < gameTiles.size(); i++ ){
    if(gameTiles.at(i) -> getType() == 10)

      gameTiles.at(i) -> draw();
}
  for( unsigned int i = 0; i < gameTiles.size(); i++ ){
    if(gameTiles.at(i) -> getType() == 10)

      gameTiles.at(i) -> draw();
}
for( unsigned int i = 0; i < gameGuests.size(); i++ ){
    if(gameGuests.at(i) -> getIsCart())

      gameGuests.at(i) -> draw();
}

  for( unsigned int i = 0; i < gameParticles.size(); i++ ){

      gameParticles.at(i) -> draw();
  }

  if(level==1 && !started)
    al_draw_bitmap( level_1_help,0, 0, 0 );

 if(level==2 && !started)
    al_draw_bitmap( level_2_help,0, 0, 0 );

  // Cursor pointer
 // al_draw_rectangle( mouseListener::mouse_x, mouseListener::mouse_y, mouseListener::mouse_x + 3, mouseListener::mouse_y + 3, al_map_rgb( 255, 255, 255), 3);


  //whitespace is heavenly
  //angels glow white because they're made of pure whitespace
  int inpark=0;

  for( unsigned int i = 0; i < gameGuests.size(); i++ ){
    if(!gameGuests.at(i) -> getIsCart())

      inpark++;
  }

//  al_draw_textf( font, al_map_rgb( 0, 0, 0), 10, 170, 0, "Guests in park:%u",(unsigned int)inpark);
//  al_draw_textf( font, al_map_rgb( 0, 0, 0), 10, 220, 0, "Guests rescued:%i",guests_rescued);
//  al_draw_textf( font, al_map_rgb( 0, 0, 0), 10, 270, 0, "Guests died to enemies:%i",guests_died_enemies);
//  al_draw_textf( font, al_map_rgb( 0, 0, 0), 10, 320, 0, "Guests died to falling:%i",guests_died_falling);

  al_draw_textf( font, al_map_rgb( 100, 0, 0), 30, 200, 0, "Money:%i",money);

 // al_draw_textf( font, al_map_rgb( 255, 255, 255), 600, 20, 0, "Cost:100$",money);


  if(level==1)
    al_draw_textf( font_small, al_map_rgb( 0, 0, 0), 30, 260, 0, "Remaining Guests:%i",10-(guests_rescued + guests_died_enemies + guests_died_falling ));
  if(level==2)
    al_draw_textf( font_small, al_map_rgb( 0, 0, 0), 30, 260, 0, "Remaining Guests:%i",15-(guests_rescued + guests_died_enemies + guests_died_falling ));
  if(level==3)
    al_draw_textf( font_small, al_map_rgb( 0, 0, 0), 30, 260, 0, "Remaining Guests:%i",70-(guests_rescued + guests_died_enemies + guests_died_falling ));

  if(level==4)
    al_draw_textf( font_small, al_map_rgb( 0, 0, 0), 30, 260, 0, "Remaining Guests:%i",150-(guests_rescued + guests_died_enemies + guests_died_falling ));


  Message::draw();



  ///send help

  //its 2 am and this is the funniest thing ive ever seen

}
