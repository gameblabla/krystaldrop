#ifndef TitleController_H
#define TitleController_H

#include "Controller.h"

#include "../video/spriteinstance.h"

class KD_Background;
class KD_Font;
class KD_Music;
class KD_Sprite;

/* main title controller */
class KD_TitleController: public KD_Controller
{ protected:
   KD_Music* music;  
   KD_SpriteInstance* title[2];
   KD_Sprite*         spr;
   KD_Font*           main_font;
   KD_Background*     back;
   unsigned long      first_tick;
   float x_f; /* "krystal"'s x */
   float y_f; /* "drop"'s y */
   char state;/* "krystal"'s state */    
   char state2; /* "drop"'s state */ 
   float* Anim_Offset;
  
     void    DisplayTitle();
     void    DisplayTexts();
  
public:
             KD_TitleController();
	virtual ~KD_TitleController();

	virtual bool init();
	virtual bool processEvent(int value);
	virtual bool display();
	virtual bool quit();
};

#endif
