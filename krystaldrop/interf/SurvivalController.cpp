#include "SurvivalController.h"

#include "Application.h"
#include "../video/Display.h"
#include "../video/font.h"

#include "../video/sprite.h"
#include "../video/spriteinstance.h"
#include "../sound/music.h"
#include "../sound/sound.h"
#include "../util/direct.h"

#define KD_A_QUIT    1
#define KD_A_ADDLINE 2
#define KD_A_TAKEGEM 3
#define KD_A_DROPGEM 4
#define KD_A_REMOVEGEM 5
#define KD_A_LEFT    6
#define KD_A_RIGHT   7
#define KD_DUMPROWS  8

KD_SurvivalController::KD_SurvivalController() : KD_Controller()
{
	music=new KD_Music();

	plopSound=new KD_Sound();
}

KD_SurvivalController::~KD_SurvivalController()
{
	delete plopSound;

	delete music;
}

void KD_SurvivalController::loadSprites()
{ signed res;
  TACCRes *accFile;
  
  accFile= new TACCRes();
  res= accFile->LoadACC("art/border.acc");
  horizontalBar= new KD_Sprite();
  res= horizontalBar->Load(accFile,"horizontalbar.txt");
  verticalBar  = new KD_Sprite();
  res= verticalBar->Load(accFile,"verticalbar.txt");
  upleftBar    = new KD_Sprite();
  res= upleftBar->Load(accFile,"upleftcorner.txt");
  uprightBar   = new KD_Sprite();
  res= uprightBar->Load(accFile,"uprightcorner.txt");
  delete accFile;

	accFile = new TACCRes();
	res= accFile->LoadACC("art/clown.acc");
	clown = new KD_Sprite();
	res= clown->Load(accFile,"clown.txt");
	delete accFile;

	accFile= new TACCRes();
	res= accFile->LoadACC("art/gems.acc");
	gem[KD_BLUE]=   new KD_Sprite();
	res= gem[KD_BLUE]  ->Load(accFile,"b.txt");
	gem[KD_GREEN]=  new KD_Sprite();
	res= gem[KD_GREEN] ->Load(accFile,"g.txt");
	gem[KD_RED]=    new KD_Sprite();
	res= gem[KD_RED]   ->Load(accFile,"r.txt");
	gem[KD_YELLOW]= new KD_Sprite();
	res= gem[KD_YELLOW]->Load(accFile,"y.txt");
 
	delete accFile;

	plopSound->LoadSound("waterdrop.wav");
}

void KD_SurvivalController::unLoadSprites()
{
	plopSound->UnloadSound();

	delete gem[KD_BLUE];
	delete gem[KD_GREEN];
	delete gem[KD_RED];
	delete gem[KD_YELLOW];
	delete clown;
	delete uprightBar;
	delete upleftBar;
	delete verticalBar;
	delete horizontalBar;

}

void KD_SurvivalController::loadMusic(char *fileName)
{
	music->Load(fileName);
}

bool KD_SurvivalController::init()
{
	loadSprites();

	// Never use action 0 because it's the void action
	bindKeyDown(SDLK_ESCAPE, KD_A_QUIT);
	bindKeyDown(SDLK_LEFT,   KD_A_LEFT);
	bindKeyDown(SDLK_RIGHT,  KD_A_RIGHT);
	bindKeyDown(SDLK_SPACE,  KD_A_ADDLINE);
	bindKeyDown(SDLK_UP,     KD_A_DROPGEM);
	bindKeyDown(SDLK_DOWN,   KD_A_TAKEGEM);
  bindKeyDown(SDLK_d, KD_DUMPROWS);

	table.setWidth(9);
	table.setHeight(12);
	table.setGemWidth(32);
	table.setGemHeight(32);
	table.setPosition(100,50);

	table.setHorizontalBar(horizontalBar);
	table.setVerticalBar(verticalBar);
	table.setUpperLeftBar(upleftBar);
	table.setUpperRightBar(uprightBar);

	table.setClownSprite(clown);

	table.setGems(gem);

	table.loadGemsToCome("table.txt");

	table.setLoopGems(false);

	table.setGemProbability(KD_BLUE, 100);
	table.setGemProbability(KD_RED, 100);
	table.setGemProbability(KD_GREEN, 100);
	table.setGemProbability(KD_YELLOW, 100);

	table.Init();

	table.setPlopSound(plopSound);

	table.addLine();
	table.addLine();
	table.addLine();

	loadMusic("puzzle.mp3");
	//loadMusic("music.ogg");

	music->PlayMusic();

	return true;
}

bool KD_SurvivalController::processEvent(int value)
{ switch(value)
	{	case KD_A_QUIT:
			KD_Application::getApplication()->sendStopEvent();
			return true;
		case KD_A_LEFT:
			table.MoveLeft();
			return true;
		case KD_A_RIGHT:
			table.MoveRight();
			return true;
		case KD_A_TAKEGEM:
			table.takeGems();
			return true;
		case KD_A_DROPGEM:
			table.dropGems();
			return true;
		case KD_A_ADDLINE:
			table.addLine();
			return true;
	}

	return false;
}


bool KD_SurvivalController::display()
{
	Display::clearScreen();
	table.Display();
	
	return true;
}


bool KD_SurvivalController::quit()
{
	music->StopMusic();
	music->CloseMusic();

	unLoadSprites();

	table.deInit();
	return true;
}

