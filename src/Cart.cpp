#include "Cart.h"

Cart::Cart(int newx, int newy)
{
  cart = tools::load_bitmap_ex( "images/tiles/car.png" );
  x=newx;
  y=newy;
  /*
  x_velocity=-40;
  y_velocity=-20;
  */
  accel = 0.0f;
  x_velocity=-0;
  y_velocity=-0;
  is_cart=true;

}

Cart::~Cart()
{
  //dtor
}

void Cart::update()
{
  accel += 0.1f;
    x += accel * -4;
    y += accel * -4;

}

void Cart::draw()
{
  al_draw_bitmap(cart, x, y, 0 );

}
