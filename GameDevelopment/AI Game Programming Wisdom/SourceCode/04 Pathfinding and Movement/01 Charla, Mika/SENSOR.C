/* 
 *    Simple 4 point sensor (whisker) that, when activated,
 *    pushes the "bot".
 *    This program demonstrates a simple coralling mechanic.
 */

#define screen_w      320
#define screen_h      200

#define solid_wall    5    /*Collision Properties*/
#define empty_space   0

#define sradius       24   /*Sensor Radius*/
#define distance      8    /*Sensor offset from Bot*/
#define repel         .08  /*Sensor repel strength*/
#define bots          32   /*Number of Bots*/
#define northbias     0    /*Bias, used to push bot in desired direction*/
#define southbias     0
#define eastbias      0
#define westbias      0

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "allegro.h"

BITMAP *vfb;

int main()
{
   int BotPosx[bots] ;
   int BotPosy[bots] ;
   float w_east[bots];
   float w_west[bots];
   float w_north[bots];
   float w_south[bots];
   int x;
   int y;

   int r;
   int c;
   BITMAP *Bot_Image;
   BITMAP *backdrop;

   allegro_init();
   install_keyboard(); 
   install_mouse();
   install_timer();
   set_gfx_mode(GFX_VGA, 320, 200, 0, 0);
   set_pallete(desktop_pallete);

   Bot_Image = create_bitmap(32, 32);
   backdrop = create_bitmap(320,200);
   vfb = create_bitmap(320,200);
   clear(Bot_Image); 
   clear(backdrop);

   for (c=0; c<8; c++)                             /*Create Border*/
       rect(backdrop,c,c,319-c,199-c,solid_wall);
   for (c=0; c<4; c++)                             /*Create Bot image */
      circle(Bot_Image, 16, 16, c*2, c);

      rectfill(backdrop,120,0,180,80,solid_wall);  /*Create "bridge"*/
      rectfill(backdrop,120,120,180,200,solid_wall);

      for (c=0; c < bots; c++)    /*Clear Variables for Init*/
      {
        BotPosx[c] = 160;
        BotPosy[c] = 100;
        w_east[c] = 0;
        w_west[c] = 0;
        w_north[c] = 0;
        w_south[c] = 0;
      }


/* Main Loop */

   do  {

   vsync();
   clear(vfb);

   if (mouse_b & 1)       /*Left Mouse Button Draws a wall*/
            rectfill(backdrop,(mouse_x)/8*8,(mouse_y)/8*8,(mouse_x)/8*8+7,(mouse_y)/8*8+7,solid_wall);
   if (mouse_b & 2)       /*Right Mouse Button Removes a wall*/
            rectfill(backdrop,(mouse_x)/8*8,(mouse_y)/8*8,(mouse_x)/8*8+7,(mouse_y)/8*8+7,empty_space);

   blit(backdrop,vfb,0,0,0,0,320,200);  /* Screen Building Process */
   rectfill(vfb,mouse_x-4,mouse_y-4,mouse_x+4,mouse_y+4,2);

   for (c=0;c<bots;c++)     /* Update Array of Bots */
   {

   for (x=0;x<sradius;x++)  /* Check West sensor on Bot */
   {
        for (y=0;y<sradius;y++)
            if (getpixel(vfb,BotPosx[c]-distance+x,BotPosy[c]+y))
               w_west[c]=w_west[c]+repel;
   }

   for (x=0;x<sradius;x++) /* Check East sensor on Bot */
   {
        for (y=0;y<sradius;y++)
            if (getpixel(vfb,BotPosx[c]+distance+x,BotPosy[c]+y))
               w_east[c]=w_east[c]+repel;
   }
   for (x=0;x<sradius;x++) /* Check North sensor on Bot */
   {
        for (y=0;y<sradius;y++)
            if (getpixel(vfb,BotPosx[c]+x,BotPosy[c]+y-distance))
               w_north[c]=w_north[c]+repel;
   }
   for (x=0;x<sradius;x++) /* Check South sensor on Bot */
   {
        for (y=0;y<sradius;y++)
            if (getpixel(vfb,BotPosx[c]+x,BotPosy[c]+y+distance))
               w_south[c]=w_south[c]+repel;
   }


   BotPosx[c] = BotPosx[c]+(w_west[c]-w_east[c]);   /* Modify Bot position with Sensor data */
   BotPosy[c] = BotPosy[c]+(w_north[c]-w_south[c]);

   w_east[c] = w_east[c] *.9;  /* Sensor bias erosion - decelerate */
   w_west[c] = w_west[c] *.9;
   w_north[c] = w_north[c] *.9;
   w_south[c] = w_south[c] *.9;

   w_west[c] = w_west[c]+westbias; /* Trick sensor with a bias */
   w_east[c] = w_east[c]+eastbias;
   w_north[c] = w_north[c]+northbias;
   w_south[c] = w_south[c]+southbias;


   masked_blit(Bot_Image,vfb,0,0,BotPosx[c]-8,BotPosy[c]-8,32,32); /*Draw Bot*/
   }

   blit(vfb,screen,0,0,0,0,320,200); /* Blit Virtual screen to actual screen */

   } while (!keypressed());

   destroy_bitmap(Bot_Image);
   destroy_bitmap(vfb);
   destroy_bitmap(backdrop);

   return 0;
}

