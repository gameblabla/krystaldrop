#include "../global.h"

#include "Application.h"
#include "eventmanager.h"
#include "HighScoresController.h"
#include "keyboard.h"
#include "SurvivalController.h"
#include "../names.h"
#ifndef NO_SOUND
#include "../sound/sound.h"
#endif
#include "../video/Display.h"
#include "../video/font.h"
#include "../video/inputbox.h"
#include "../video/sprite.h"
#include "../video/spriteinstance.h"
#include "../video/spriteevent.h"
#include "../video/textevent.h"
#include "../video/fountainevent.h"

#define KD_A_NOACTION 0
#define KD_A_QUIT    1
#define KD_A_ADDLINE 2
#define KD_A_TAKEGEM 3
#define KD_A_DROPGEM 4
#define KD_A_REMOVEGEM 5
#define KD_A_LEFT    6
#define KD_A_RIGHT   7
#define KD_A_QUITLOSE 8


KD_SurvivalController::KD_SurvivalController() : KD_Controller()
{
	#ifndef NO_SOUND  
	plopSound= new KD_Sound();
	chocSound= new KD_Sound();
	gemsDownSound= new KD_Sound();
	gemsUpSound= new KD_Sound();
	gemsDownSound->setVolume(50);
	gemsUpSound->setVolume(50);
	for (int i=0; i<KD_SND_NBCLASHSOUND; i++)
	{
		clashSound[i]=new KD_Sound();
	}
	#endif  
  
  image_manager= NULL;
  memset (images, 0, sizeof(images));

  background= NULL;

	currentLevel=0;
	clashCount=0;
	maxClashCount=0;

	comboEvent=0;

	timeOfNewState = Display::ticks;

	gemsToLevel[0]=20;
	gemsToLevel[1]=50;
	gemsToLevel[2]=80;
	gemsToLevel[3]=120;
	gemsToLevel[4]=160;
	gemsToLevel[5]=200;
	gemsToLevel[6]=250;
	gemsToLevel[7]=300;
	gemsToLevel[8]=350;
	gemsToLevel[9]=400;
	gemsToLevel[10]=500;
	gemsToLevel[11]=600;
	gemsToLevel[12]=700;
	gemsToLevel[13]=800;
	gemsToLevel[14]=1000;
	
	speedOfLevel[0]=11000;
	speedOfLevel[1]=9500;
	speedOfLevel[2]=8000;
	speedOfLevel[3]=7000;
	speedOfLevel[4]=6000;
	speedOfLevel[5]=5000;
	speedOfLevel[6]=4500;
	speedOfLevel[7]=4000;
	speedOfLevel[8]=3500;
	speedOfLevel[9]=3100;
	speedOfLevel[10]=2700;
	speedOfLevel[11]=2300;
	speedOfLevel[12]=2000;
	speedOfLevel[13]=1700;
	speedOfLevel[14]=1500;
	currentTimeBetweenLines=speedOfLevel[0];
    
    leftDoor= NULL;
}

KD_SurvivalController::~KD_SurvivalController()
{ 
	#ifndef NO_SOUND  
	DELETE (plopSound);
	DELETE (chocSound);
	DELETE (gemsDownSound);
	DELETE (gemsUpSound);
	for (int i=0; i<KD_SND_NBCLASHSOUND; i++)
	{
		DELETE (clashSound[i]);
	}
	#endif
}

void KD_SurvivalController::loadSprites()
{ signed res;
  TACCRes *accFile;
  
  horizontalBar= new KD_Sprite();
  res= horizontalBar->Load("horizontalbar.txt");
  assert(res);
  verticalBar  = new KD_Sprite();
  res= verticalBar->Load("verticalbar.txt");
  assert(res);
  upleftBar    = new KD_Sprite();
  res= upleftBar->Load("upleftcorner.txt");
  assert(res);
  uprightBar   = new KD_Sprite();
  res= uprightBar->Load("uprightcorner.txt");
  assert(res);

  leftDoor = new KD_Sprite();
  res= leftDoor->Load( "doorl.txt");
  assert(res);
  rightDoor = new KD_Sprite();
  res= rightDoor->Load( "doorr.txt");
  assert(res);

  bottomBar   = new KD_Sprite();
  res= bottomBar->Load( "bottombar.txt");
  assert(res);
  
  image_manager= KD_ImageManager::getImageManager();
  assert (image_manager);
  image_manager->Load( "terrain2.png");
  
  background= image_manager->getImage("terrain2.png");
  background->disableAlpha();  

  for (short gem_index= 0; gem_index< KD_GEM_NB_KINDS; gem_index++)
  { gem[gem_index]= new KD_Sprite();
    res= gem[gem_index]->Load( GEM_ANIM_NAME[gem_index]);
    /* # test res ! */
  }
     
  /* character images */
  characterSprite= new KD_Sprite();
  res= characterSprite->Load( CHAR_ANIM_NAME[pl_chars[0]]);
  assert(res);
                   
  clown = new KD_Sprite();
  res= clown->Load( "lightchip.txt");
  /* For some reasons it was resized to abnormal proportions ??? - Gameblabla */
  //clown->resize(1.8f);

  particle = new KD_Sprite();
  res= particle->Load("star.txt");

  lineSprite= new KD_Sprite();
  CHECK_ALLOC (lineSprite);
  res= lineSprite->Load("line.txt");
}


void KD_SurvivalController::unLoadSprites()
{
/*	plopSound->UnloadSound();*/

  KD_ImageManager::getImageManager()->releaseImage(background);
  background= NULL;

  for (short i= 0; i< KD_GEM_NB_KINDS; i++) DELETE (gem[i]);
  
  DELETE (clown);
  DELETE (uprightBar);
  DELETE (upleftBar);
  DELETE (verticalBar);
  DELETE (horizontalBar);
  DELETE (leftDoor);
  DELETE (rightDoor);
  DELETE (bottomBar);
  DELETE (characterSprite);
  DELETE (particle);
  DELETE (lineSprite);
}


bool KD_SurvivalController::init()
{
	loadSprites();

	controllerState = KD_CSTATE_PLAYING;

	// Never use action 0 because it's the void action
	bindKeyDown(SDLK_ESCAPE, KD_A_QUIT);
	bindKeyDown(SDLK_LEFT,   KD_A_LEFT);
	bindKeyDown(SDLK_RIGHT,  KD_A_RIGHT);
	bindKeyDown(SDLK_RETURN,  KD_A_ADDLINE);
	bindKeyDown(SDLK_RSHIFT, KD_A_DROPGEM);
	bindKeyDown(SDLK_RCTRL,  KD_A_TAKEGEM);
  
	#ifndef NO_SOUND
	plopSound->LoadSound("art/waterdrop.wav");
	gemsDownSound->LoadSound("art/swing.wav");
	gemsUpSound->LoadSound("art/swing2.wav");
	chocSound->LoadSound("art/choc.wav");
	for (int i=0; i<KD_SND_NBCLASHSOUND; i++)
	{
		clashSound[i]->LoadSound(CHAR_CLASHSOUND_NAME[i]);
	}
	#endif  

	

	clashCount=0;
	maxClashCount=0;
	currentLevel=0;

	table.Init();

	table.setWidth(9);
	table.setHeight(12);
	table.setGemWidth(32);
	table.setGemHeight(32);
  
#define DIFFICULTY 9
signed Position_X= (640- DIFFICULTY* 32)/ 2;
	table.setPosition(Position_X,50);

	table.setHorizontalBar(horizontalBar);
	table.setVerticalBar(verticalBar);
	table.setUpperLeftBar(upleftBar);
	table.setUpperRightBar(uprightBar);
	table.setLeftDoor(leftDoor);
	table.setRightDoor(rightDoor);
	table.activateDoors(true);
	table.setBottomBar(bottomBar);
	table.setLineSprite(lineSprite);

	table.setClownSprite(clown);

	table.setGems(gem);
    for (int gem_type= 0; gem_type< KD_GEM_NB_KINDS; gem_type++)
      table.setGemProbability (gem_type, 12);
    
	table.loadGemsToCome("table.txt");

	table.setLoopGems(false);

	table.InitSet();

	table.addLine();
	table.addLine();
	table.addLine();

	characterSpriteInstance = new KD_SpriteInstance(characterSprite);

	characterSpriteInstance->x=Position_X + DIFFICULTY*32/2;
	characterSpriteInstance->y=50 + 32*12;

	timer= new KD_TextEvent();
    CHECK_ALLOC (timer);
	timer->setTextFont(Display::Slapstick);
	timer->setCoordinates(75,450);
	timer->printFromCenter();
	timer->activateTextTimer();
	timer->activateEvent();
    
    PLAYMUSIC("art/survival.ogg");

	#ifndef NO_SOUND
	table.setPlopSound(plopSound);
	table.setGemsDownSound(gemsDownSound);
	table.setGemsUpSound(gemsUpSound);
	table.setClashSounds(clashSound);
	#endif

	/*KD_SpriteEvent *ev = new KD_SpriteEvent();
	ev->setSprite(characterSprite);
	ev->setGravityMove(320,400,0.2f,-3.0f,0.05f,255,255,255,255,255,0,0,128,3);
	ev->activateEvent();*/

/*	KD_FountainEvent *fount = new KD_FountainEvent();
	fount->setCoordinates(320,400);
	fount->setTimeToLive(10);
	fount->setParticle(0.0f,-6.0f,20.0f/180.0f*3.14f, 0.2f, 0.05f,particle,20);
	fount->setParticleColors(255,255,255,255,255,0,0,128);
	fount->activateEvent();*/

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
		case KD_A_QUITLOSE:
			//KD_Application::getApplication()->gotoController(???);
			return true;
	}

	return false;
}


bool KD_SurvivalController::display()
{
  
	switch (controllerState)
	{
	case KD_CSTATE_PLAYING:
		return displayPlayingState();
	case KD_CSTATE_LOSE:
		return displayLoseState();
	case KD_CSTATE_HIGHSCORE:
		return displayHighScoreState();
	}
	return false;
}

bool KD_SurvivalController::displayPlayingState()
{
	/// ADD DE LIGNES TEMPORAIRE
	static int last_line_added_time=0;
	if ((signed)(SDL_GetTicks()-last_line_added_time)> currentTimeBetweenLines)
	{
		last_line_added_time = SDL_GetTicks();
		table.addLine();
	}

	background->Display(0,0);
#ifdef DISPLAY_FPS
	Display::DisplayFramesPerSecond (12,42+2+2,20);
#endif     

	characterSpriteInstance->Display (1);
    
	table.Display();

	if (table.getHasClashed())
	{ int i;
		for (i= 0; i<NB_LEVELS; i++)
			if (table.getNbGemsDropped() < gemsToLevel[i])
				break;

		if (currentLevel!=i)
		{
			currentLevel = i;
			currentTimeBetweenLines = speedOfLevel[i];
			KD_TextEvent *level = new KD_TextEvent();
			level->setTextFont(Display::Slapstick);
			level->printFromCenter();
			level->setText("Level %d!",currentLevel);
			//level->setQuadraticMove(320,-50,255,320,150,255,320,-50,0,3);
			level->setQuadraticMove(320,-50,255,255,255,255,0.5f,0.5f,0,
								    320,/*150*/100,255,255,255,/*128*/250,1.0f,1.0f,0,
									320,/*150*/80,255,255,255,0,2.0f,2.0f,0,3);
			level->activateEvent();
		}
	}

	if (table.isTestMaxHeightNeeded())
	{
		// Test what is the maximum height of the field. If not enough, add new gems.
		int maxHeight = table.getMaxHeight();
		// it may be a good idea to insert a timer before adding those lines.
		if (maxHeight <= 2 && table.isAddingGems()==false && table.getIsHoldingGems()==false && table.getClashCount()==0)
        { 
			table.addLine();
			last_line_added_time = SDL_GetTicks();
		}

		// If 3/4 of the screen is filled
		if ( maxHeight*4 > table.getHeight()*3  &&  characterMood != KD_STRESSED)
		{
			characterMood = KD_STRESSED;
			//characterSpriteInstance->setAnim(1);
			//table[nbTable].TriggerCharacterAction(KD_DANGER);
		}
		else if ( maxHeight*2 > table.getHeight() && maxHeight*4 < table.getHeight()*3  &&  characterMood != KD_MEDIUMMOOD)
		{
			characterMood = KD_MEDIUMMOOD;
		}
		else if ( maxHeight*2 < table.getHeight()  &&  characterMood != KD_GOODMOOD)
		{
			characterMood = KD_GOODMOOD;
		}

		// Test if the player has lost.
		if (maxHeight>table.getHeight())
		{
			table.prepareLose();
			timer->pauseTimer();
			timeOfNewState = Display::ticks;
			controllerState = KD_CSTATE_LOSE;
			
			// Unbinds the keys
			bindKeyDown(SDLK_LEFT,   KD_A_NOACTION);
			bindKeyDown(SDLK_RIGHT,  KD_A_NOACTION);
			bindKeyDown(SDLK_SPACE,  KD_A_QUITLOSE);
			bindKeyDown(SDLK_UP,     KD_A_QUITLOSE);
			bindKeyDown(SDLK_DOWN,   KD_A_QUITLOSE);
		}
	}

	if (table.getHasClashed() && table.getClashCount()>1)
	{
		clashCount++;

		comboEvent = new KD_TextEvent();
		comboEvent->setTextFont(Display::Slapstick);
		comboEvent->printFromRight();
		comboEvent->setText("%d combo hits!",table.getClashCount());
		comboEvent->setQuadraticMove(640,460,255,640,380,128,640,360,0,3);
		comboEvent->activateEvent();
	}

	if (table.getClashCount() > maxClashCount && table.getClashCount()!=1) 
		maxClashCount = table.getClashCount();

	Display::Slapstick->xycenteredprintf(565,150,"%d", clashCount);
	Display::Slapstick->xycenteredprintf(565,380,"%d", maxClashCount);
	Display::Slapstick->xycenteredprintf(70,130,"%d", table.getScore());
	Display::Slapstick->xycenteredprintf(70,290,"%d", currentLevel);

	return true;
}

bool KD_SurvivalController::displayLoseState()
{
	background->Display(0,0);
#ifdef DISPLAY_FPS
	Display::DisplayFramesPerSecond (12,42+2+2,20);
#endif   

	characterSpriteInstance->Display (1);

	table.DisplayOnLose();

	Display::Slapstick->xycenteredprintf(565,150,"%d", clashCount);
	Display::Slapstick->xycenteredprintf(565,380,"%d", maxClashCount);
	Display::Slapstick->xycenteredprintf(70,130,"%d", table.getScore());
	Display::Slapstick->xycenteredprintf(70,290,"%d", currentLevel);


	if(Display::ticks - timeOfNewState > 4000)
	{
		if (KD_HighScoresController::hst[0]->IsBetterHighScore(table.getScore()))
		{
			controllerState = KD_CSTATE_HIGHSCORE;

			nameBox = new KD_InputBox();
			nameBox->setTextFont(Display::Slapstick);
			nameBox->setTextCoordinates(250,270);
			nameBox->activateEvent();
		}
		else
			KD_Application::getApplication()->gotoController("highscores");
	}

	return true;
}

bool KD_SurvivalController::displayHighScoreState()
{
	background->Display(0,0);
#ifdef DISPLAY_FPS
	Display::DisplayFramesPerSecond (12,42+2+2,20);
#endif   

	characterSpriteInstance->Display (1);

	table.DisplayOnLose();

	Display::Slapstick->xycenteredprintf(565,150,"%d", clashCount);
	Display::Slapstick->xycenteredprintf(565,380,"%d", maxClashCount);
	Display::Slapstick->xycenteredprintf(70,130,"%d", table.getScore());
	Display::Slapstick->xycenteredprintf(70,290,"%d", currentLevel);
	Display::Slapstick->xycenteredprintf(SCR_HW,200,"Enter your name:");

	if (nameBox->getLength() == 3)
		Display::Slapstick->xycenteredprintf(SCR_HW,340,"Press Return");
		

	if (KD_Keyboard::getKeyboard()->getLastSDLKey() == SDLK_RETURN)
	{
		char *playerName = (char *) nameBox->getText().c_str();
		KD_HighScoresController::hst[0]->InsertHigherScore(playerName, table.getScore(), pl_chars[0]);
		KD_Application::getApplication()->gotoController("highscores");
	}

	return true;
}

bool KD_SurvivalController::quit()
{
	DELETE (characterSpriteInstance);
    DELETE (timer); 
    CLOSEMUSIC();
	unLoadSprites();

	table.deInit();
	table.desalloc();

    return KD_Controller::quit();
}
