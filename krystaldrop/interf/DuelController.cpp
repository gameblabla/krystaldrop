#include <assert.h>

#include "Application.h"
#include "DuelController.h"
#include "../game/parameter.h"
#include "../game/hand.h"
#include "../game/row.h"
#include "../game/set.h"
#include "../util/direct.h"
#include "../video/font.h"
#include "../video/gem.h"
#include "../video/sprite.h"
#include "../video/Display.h"
#include "../video/spriteinstance.h"

#define WIDTH 3
#define HEIGHT 3
#define MAX_IN_HAND 3

KD_DuelController::KD_DuelController(): KD_Controller()
{
}

KD_DuelController::~KD_DuelController()
{
}

#define KD_A_QUIT    1
#define KD_A_ADDLINE 2
#define KD_A_TAKEGEM 3
#define KD_A_DROPGEM 4

#define KD_A_LEFT    6
#define KD_A_RIGHT   7




bool KD_DuelController::init()
{
	
#define HEIGHT_FIELD_PIXEL (HEIGHT*32)
/* debug */
param= new KD_Parameters();
param->SetVideoParameters (28, 32, HEIGHT_FIELD_PIXEL, 50, 130);
param->SetGameParameters (3, 0, -1, 0, 1, 1, -1, -1);
set= new KD_Set(WIDTH, HEIGHT, MAX_IN_HAND, param);
/* */


	// Never use action 0 because it's the void action
	bindKeyDown(SDLK_ESCAPE, KD_A_QUIT);
	bindKeyDown(SDLK_SPACE,  KD_A_ADDLINE);
	bindKeyDown(SDLK_UP,     KD_A_DROPGEM);
	bindKeyDown(SDLK_DOWN,   KD_A_TAKEGEM);
    
	bindKeyDown(SDLK_LEFT,   KD_A_LEFT);
	bindKeyDown(SDLK_RIGHT,  KD_A_RIGHT);
  

	TACCRes *accFile = new TACCRes();
    
    signed res;
	res= accFile->LoadACC("gems.acc");
//	res= accFile->LoadACC("immp.acc");
    assert (!res);
    
	blue_spr= new KD_Sprite();
    assert (blue_spr);
	res= blue_spr->Load(accFile,"b.txt");
    assert (res);
    /* return value ? */
	delete accFile;

	smallFont = Display::Slapstick->resize(0.5);
	
	return true;
}

bool KD_DuelController::processEvent(int value)
{ static KD_Gem* g;
  static KD_Gem* gtab[WIDTH+1];
  signed status;
  
	switch(value)
	{
	}

	return false;
}

bool KD_DuelController::display()
{ assert (set);

  Display::clearScreen();
  Display::Slapstick->xyprintf(50,50,"Krystal Drop \n 1234567890");
  return true;
}

bool KD_DuelController::quit()
{
  return true;
}
