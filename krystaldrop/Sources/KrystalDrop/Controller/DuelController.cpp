#include "../global.h"

#include "../../KDpp/Controller/Application.h"
#include "DuelController.h"

#ifndef NO_SOUND
#include "../../KDpp/Sound/Sound.h"
#endif
//#include "../util/direct.h"
#include "../../KDpp/Video/Font.h"
#include "../Video/Gem.h"
#include "../../KDpp/Video/Sprite.h"
#include "../../KDpp/Video/Image.h"
#include "../../KDpp/Video/Display.h"
#include "../../KDpp/Video/SpriteInstance.h"
#include "../../KDpp/Video/Events/FountainEvent.h"
#include "../../KDpp/Video/Events/TextEvent.h"
#include "../../KDpp/Sound/Music.h"
#include "../../KDpp/Resources/GlobalResourceSet.h"

#define KD_A_NOACTION	0
#define KD_A_QUIT		1
#define KD_A_ADDLINE1	2
#define KD_A_TAKEGEM1	3
#define KD_A_DROPGEM1	4
#define KD_A_REMOVEGEM1	5
#define KD_A_LEFT1		6
#define KD_A_RIGHT1		7
#define KD_A_ADDLINE2	8
#define KD_A_TAKEGEM2	9
#define KD_A_DROPGEM2	10
#define KD_A_REMOVEGEM2	11
#define KD_A_LEFT2		12
#define KD_A_RIGHT2		13
#define KD_A_QUITLOSE	14
#define KD_A_DECREASECONTINUE	15
#define KD_A_CONTINUE			16

#define KD_CSTATE_READY 0
#define KD_CSTATE_PLAYING 1
#define KD_CSTATE_FINISH 2
#define KD_CSTATE_WIN 3
#define KD_CSTATE_CONTINUE 4

#define YTIME 182
#define YPLAYER1 310
#define YPLAYER2 440

KD_DuelController::KD_DuelController(): KD_Controller()
{
	background= 0;
}

KD_DuelController::~KD_DuelController()
{

}

bool KD_DuelController::Init()
{
	music = new KD_Music();

	
	return true;
}

bool KD_DuelController::InitRound()
{
	// Never use action 0 because it's the void action
	BindKeyDown(SDLK_LEFT,   KD_A_LEFT2);
	BindKeyDown(SDLK_RIGHT,  KD_A_RIGHT2);
	BindKeyDown(SDLK_RETURN, KD_A_ADDLINE2);
	BindKeyDown(SDLK_RSHIFT, KD_A_DROPGEM2);
	BindKeyDown(SDLK_RCTRL,  KD_A_TAKEGEM2);
	
	BindKeyDown(SDLK_x,   KD_A_LEFT1);
	BindKeyDown(SDLK_c,  KD_A_RIGHT1);
	BindKeyDown(SDLK_v,  KD_A_ADDLINE1);
	BindKeyDown(SDLK_LSHIFT,     KD_A_DROPGEM1);
	BindKeyDown(SDLK_LCTRL,   KD_A_TAKEGEM1);

	for (int i=0; i<KD_DUEL_NB_PLAYERS; i++)
	{
		table[i].addLine();
		table[i].addLine();
		table[i].addLine();

		hasWon[i]=true;
		characterMood[i] = KD_GOODMOOD;
	}

	last_line_added_time[0]=Display::getTicks();
	last_line_added_time[1]=Display::getTicks();

	timeOfNewState = Display::getTicks();

	return true;
}

bool KD_DuelController::InitReadyState()
{
	// Should empty each table.

	// speed of line dropping.
	currentTimeBetweenLines = 7000;

	timeOfNewState = Display::getTicks();
	
	clashCount[0]=0;
	clashCount[1]=0;

	for (int i=0; i<KD_DUEL_NB_PLAYERS; i++)
	{
		table[i].resetTable();
	}

	KD_TextEvent *ready = new KD_TextEvent();
	ready->SetTextFont(main_font);
	ready->PrintFromCenter();
	ready->SetText("Ready?");
	ready->setQuadraticMove(320,-50,255,255,255,255,0.5f,0.5f,0,
							320,240,255,255,255,250,1.0f,1.0f,0,
							320,240,255,255,255,0  ,2.0f,2.0f,0,3);
	ready->ActivateEvent();
	AddEvent(ready);

	readyGoSound->PlaySound();

	return true;
}

void KD_DuelController::LoadSprites()
{	
	main_font = (KD_Font *)KD_GlobalResourceSet::GetGlobalResource()->GetResource("big font"); 

	LoadResourceFile("art/survival/survival.txt");
	border[KD_HORIZONTAL_BAR] = (KD_Sprite *)GetResource("horizontalbar");
	border[KD_VERTICAL_BAR] = (KD_Sprite *)GetResource("verticalbar");
	border[KD_UPPER_LEFT_BAR] = (KD_Sprite *)GetResource("upleftcorner");
	border[KD_UPPER_RIGHT_BAR] = (KD_Sprite *)GetResource("uprightcorner");
	border[KD_LEFTDOOR] = (KD_Sprite *)GetResource("doorl");
	border[KD_RIGHTDOOR] = (KD_Sprite *)GetResource("doorr");
	border[KD_BOTTOM_BAR] = (KD_Sprite *)GetResource("bottombar");
	background = (KD_Image *)GetResource("terrainMulti");
	background->disableAlpha();

	LoadResourceFile("art/gems/gems.txt");
    for (short gem_index= 0; gem_index< KD_GEM_NB_KINDS; gem_index++)
	{ 
		gem[gem_index] = (KD_Sprite *)GetResource(GEM_ANIM_NAME[gem_index]);
	}

	/* character images */
	for (int i=0; i<2; i++)
	{
		string res = "art/characters/";
		res += CHAR_ANIM_NAME[pl_chars[i]];
		res += "/";
		res += CHAR_ANIM_NAME[pl_chars[i]];
		res += ".txt";
		string res2 = "art/characters/";
		res2 += CHAR_ANIM_NAME[pl_chars[i]];
		res2 += "/actions.xml";
		
		table[i].LoadCharacter(res,res2);
	}

	LoadResourceFile("art/cup/cup.txt");
	cupSprite = (KD_Sprite *)GetResource("cup");
	
	LoadResourceFile("art/star/star.txt");
	particle = (KD_Sprite *)GetResource("star");

	LoadResourceFile("art/line/line.txt");
	lineSprite = (KD_Sprite *)GetResource("line");


#ifndef NO_SOUND
	LoadResourceFile("art/sound/sound.txt");
	plopSound = (KD_Sound*) GetResource("clapSound");
	gemsDownSound = (KD_Sound*) GetResource("gemsDownSound");
	gemsUpSound = (KD_Sound*) GetResource("gemsUpSound");
	gemsDownSound->SetVolume(50);
	gemsUpSound->SetVolume(50);
	chocSound = (KD_Sound*) GetResource("clapSound");
	for (int i=0; i<KD_SND_NBCLASHSOUND; i++)
	{
		clashSound[i] = (KD_Sound*) GetResource(CHAR_CLASHSOUND_NAME[i]);
	}
	readyGoSound = (KD_Sound*) GetResource("readyGoSound");
#endif
}


void KD_DuelController::UnloadSprites()
{ 
	ReleaseResource("horizontalbar");
	ReleaseResource("verticalbar");
	ReleaseResource("upleftcorner");
	ReleaseResource("uprightcorner");
	ReleaseResource("doorl");
	ReleaseResource("doorr");
	ReleaseResource("bottombar");
	ReleaseResource("terrainMulti");

	for (short gem_index= 0; gem_index< KD_GEM_NB_KINDS; gem_index++)
	{ 
		ReleaseResource(GEM_ANIM_NAME[gem_index]);
	}

	for (int i=0; i<2; i++)
	{
		table[i].UnloadCharacter();
	}

	ReleaseResource("star");
	ReleaseResource("line");
	ReleaseResource("cup");

	KD_GlobalResourceSet::GetGlobalResource()->ReleaseResource("big font");


	#ifndef NO_SOUND
	ReleaseResource("clapSound");
	ReleaseResource("gemsDownSound");
	ReleaseResource("gemsUpSound");
	ReleaseResource("clapSound");
	for (int i=0; i<KD_SND_NBCLASHSOUND; i++)
	{
		ReleaseResource(CHAR_CLASHSOUND_NAME[i]);
	}
	#endif  
	ReleaseResource("readyGoSound");
}


bool KD_DuelController::ProcessEvent(int value)
{ 
	switch(value)
	{
		case KD_A_QUIT:
			KD_Application::getApplication()->sendStopEvent();
			return true;
		case KD_A_LEFT1:
			table[0].MoveLeft();
			return true;
		case KD_A_RIGHT1:
			table[0].MoveRight();
			return true;
		case KD_A_TAKEGEM1:
			table[0].takeGems();
			return true;
		case KD_A_DROPGEM1:
			table[0].dropGems();
			return true;
		case KD_A_ADDLINE1:
			// Pas efficace: si on appuie tr�s vite sur le bouton faire tomber des lignes, les lignes s'empilent dans la stack et on peut quand m�me mourrir en s'ajoutant des lignes.
			if (table[0].getMaxHeight()<table[0].getHeight())
			{
				table[0].addLine();
				last_line_added_time[0] = SDL_GetTicks();
			}
			return true;
		case KD_A_LEFT2:
			table[1].MoveLeft();
			return true;
		case KD_A_RIGHT2:
			table[1].MoveRight();
			return true;
		case KD_A_TAKEGEM2:
			table[1].takeGems();
			return true;
		case KD_A_DROPGEM2:
			table[1].dropGems();
			return true;
		case KD_A_ADDLINE2:
			// Pas efficace: si on appuie tr�s vite sur le bouton faire tomber des lignes, les lignes s'empilent dans la stack et on peut quand m�me mourrir en s'ajoutant des lignes.
			if (table[1].getMaxHeight()<table[1].getHeight())
			{
				table[1].addLine();
				last_line_added_time[1] = SDL_GetTicks();
			}
			return true;
		case KD_A_QUITLOSE:
			//KD_Application::getApplication()->gotoController(???);
			return true;
		
		case KD_A_DECREASECONTINUE:
			timeOfNewState -= 1000;
			return true;
		case KD_A_CONTINUE:
			//KD_Application::getApplication()->gotoController ("charsel2");
			KD_Application::getApplication()->disableController(this);
			KD_Application::getApplication()->enableController("Background", KD_LAST_POS);
			KD_Application::getApplication()->enableController("Charsel2", KD_FRONT_POS);
			return true;
	}

	return false;
}

bool KD_DuelController::Process()
{ 
	return false;	
}

bool KD_DuelController::Display()
{ 
#ifdef DISPLAY_FPS    
	Display::DisplayFramesPerSecond (12,42+2+2,20);
#endif
  
	switch (controllerState)
	{
	case KD_CSTATE_READY:
		return DisplayReadyState();
	case KD_CSTATE_PLAYING:
		return DisplayPlayingState();
	case KD_CSTATE_FINISH:
		return DisplayFinishState();
	case KD_CSTATE_CONTINUE:
		return DisplayContinueState();
		break;
	}
	return false;
}

bool KD_DuelController::DisplayPlayingState()
{
	background->Display(0,0);

	DisplayTable(0);
	DisplayTable(1);

	int timeRemaining = 90-(Display::getTicks()-timeOfNewState)/1000;
	main_font->xycenteredprintf (SCR_HW,YTIME,"%d", timeRemaining);
	main_font->xycenteredprintf (SCR_HW,YPLAYER1,"%d", 150-table[0].getNbGemsDropped());
	main_font->xycenteredprintf (SCR_HW,YPLAYER2,"%d", 150-table[1].getNbGemsDropped());

	for (int i=0; i<nbRounds*KD_DUEL_NB_PLAYERS; i++)
	{
		// Yeah! That line could nearly be in Goto++!
        cup[i]->Display((i<nbRounds)? 32+7*32-(nbRounds-i)*32 : 384+(i-nbRounds)*32, 442);
	}

	if (timeRemaining <=0)
	{
		controllerState = KD_CSTATE_FINISH;
			
		if (table[0].getNbGemsDropped() == table[1].getNbGemsDropped())
		{
			hasWon[0] = false;
			hasWon[1] = false;
		}
		else if (table[0].getNbGemsDropped() < table[1].getNbGemsDropped())
		{
			hasWon[0] = false;
			hasWon[1] = true;
		}
		else if (table[0].getNbGemsDropped() > table[1].getNbGemsDropped())
		{
			hasWon[0] = true;
			hasWon[1] = false;
		}
	}

	if (table[0].getNbGemsDropped() >= 150 && table[1].getNbGemsDropped()<150)
	{
		controllerState = KD_CSTATE_FINISH;
		hasWon[0] = true;
		hasWon[1] = false;
	}
	else if (table[1].getNbGemsDropped() >= 150 && table[0].getNbGemsDropped()<150)
	{
		controllerState = KD_CSTATE_FINISH;
		hasWon[0] = false;
		hasWon[1] = true;
	}
	else if (table[0].getNbGemsDropped() >= 150 && table[1].getNbGemsDropped()>=150)
	{
		controllerState = KD_CSTATE_FINISH;
		hasWon[0] = false;
		hasWon[1] = false;
	}

	// if we are going to step into win/lose screen next frame then, resolves the scores.
	if (controllerState == KD_CSTATE_FINISH)
	{
		timeRemainingWhenFinished = 90-(Display::getTicks()-timeOfNewState)/1000;

		timeOfNewState = Display::getTicks();

		//timer->PauseTimer();

		if (hasWon[0]==false && hasWon[1]==true)
		{
			cup[nbRounds+nbWon[1]]->setAnim(1);
			nbWon[1]++;
		}
		else if (hasWon[0]==true && hasWon[1]==false)
		{
			cup[nbWon[0]]->setAnim(1);
			nbWon[0]++;
		}

		for (int i=0; i<2; i++)
		{
			if (hasWon[i]==true)
			{
				table[i].prepareWin();
				KD_TextEvent *win = new KD_TextEvent();
				win->SetTextFont(main_font);
				win->PrintFromCenter();
				win->SetText("Victory!");
				int xPos = (i==0) ? 32 + 7*32/2 : 384 + 7*32/2;
				win->setQuadraticMove(  xPos,240,255,255,255,0  ,0.33f,0.33f, 0,
										xPos,240,255,255,255,200,0.66f,-0.66f,0,
										xPos,240,255,255,255,255,1.0f,1.0f, 0,2);
				win->ActivateEvent();
				AddEvent(win);

				KD_FountainEvent *fount= new KD_FountainEvent();
				fount= new KD_FountainEvent();
				fount->setCoordinates((i==0) ? 32 : 384,50+12*32);
				fount->setTimeToLive(2);
				//fount->setParticle(0.5f,-2.5f,20.0f/180.0f*3.14f, 0.2f, 0.02f,particle,20);
				fount->setParticle(50.0f,-250.0f,20.0f/180.0f*3.14f, 0.2f, 90.0f,particle,20);
				fount->setParticleColors(255,255,255,255,255,0,0,160);
				fount->ActivateEvent();
				AddEvent(fount);

				fount= new KD_FountainEvent();
				fount->setCoordinates((i==0) ? 32 + 7*32 : 384 + 7*32,50+12*32);
				fount->setTimeToLive(2);
				//fount->setParticle(-0.5f,-2.5f,20.0f/180.0f*3.14f, 0.2f, 0.02f,particle,20);
				fount->setParticle(-50.0f,-250.0f,20.0f/180.0f*3.14f, 0.2f, 90.0f,particle,20);
				fount->setParticleColors(255,255,255,255,255,0,0,160);
				fount->ActivateEvent();
				AddEvent(fount);

				table[i].TriggerCharacterAction(KD_WINNING);
			}
			else
			{
				table[i].prepareLose();
				table[i].TriggerCharacterAction(KD_LOOSING);
			}
		}
	}

	return true;
}

bool KD_DuelController::DisplayTable(short nbTable)
{
	/// ADD DE LIGNES TEMPORAIRE
	if ((signed)(SDL_GetTicks()-last_line_added_time[nbTable])> currentTimeBetweenLines)
	{
		last_line_added_time[nbTable] = SDL_GetTicks();
		table[nbTable].addLine();
	}

	table[nbTable].Display();


	if (table[nbTable].isTestMaxHeightNeeded())
	{
		// Test what is the maximum height of the field. If not enough, add new gems.
		int maxHeight = table[nbTable].getMaxHeight();
		if (maxHeight <= 3 && table[nbTable].isAddingGems()==false && table[nbTable].getIsHoldingGems()==false && table[nbTable].getClashCount()==0)
        { 
			table[nbTable].addLine();
			last_line_added_time[nbTable] = SDL_GetTicks();
		}

		// If 3/4 of the screen is filled
		if ( maxHeight*4 > table[nbTable].getHeight()*3  &&  characterMood[nbTable] != KD_STRESSED)
		{
			characterMood[nbTable] = KD_STRESSED;
			table[nbTable].TriggerCharacterAction(KD_DANGER);
		}
		else if ( maxHeight*2 > table[nbTable].getHeight() && maxHeight*4 < table[nbTable].getHeight()*3  &&  characterMood[nbTable] != KD_MEDIUMMOOD)
		{
			characterMood[nbTable] = KD_MEDIUMMOOD;
		}
		else if ( maxHeight*2 < table[nbTable].getHeight()  &&  characterMood[nbTable] != KD_GOODMOOD)
		{
			characterMood[nbTable] = KD_GOODMOOD;
		}

		// Test if the player has lost.
		if (maxHeight>table[nbTable].getHeight())
		{
			controllerState = KD_CSTATE_FINISH;
			
			// By default, hasWon = true, we put it to false.
			hasWon[nbTable] = false;

			// Unbinds the keys
			BindKeyDown(SDLK_LEFT,   KD_A_NOACTION);
			BindKeyDown(SDLK_RIGHT,  KD_A_NOACTION);
			BindKeyDown(SDLK_RETURN, KD_A_QUITLOSE);
			BindKeyDown(SDLK_RSHIFT, KD_A_QUITLOSE);
			BindKeyDown(SDLK_RCTRL,  KD_A_QUITLOSE);
	
			BindKeyDown(SDLK_x,		 KD_A_NOACTION);
			BindKeyDown(SDLK_c,		 KD_A_NOACTION);
			BindKeyDown(SDLK_v,		 KD_A_QUITLOSE);
			BindKeyDown(SDLK_LSHIFT, KD_A_QUITLOSE);
			BindKeyDown(SDLK_LCTRL,  KD_A_QUITLOSE);
		}
	}

	if (table[nbTable].getHasClashed() && table[nbTable].getClashCount()>1)
	{
//		clashCount++;
		KD_TextEvent *comboEvent;
		comboEvent = new KD_TextEvent();
		comboEvent->SetTextFont(main_font);
		comboEvent->PrintFromRight();
		comboEvent->SetText("%d combo hits!",table[nbTable].getClashCount());
		if (nbTable==0)
		{
			comboEvent->PrintFromLeft();
			comboEvent->setQuadraticMove(-320,40,255,0,40,150,20,40,0,3);
		}
		else if (nbTable==1)
		{
			comboEvent->PrintFromRight();
			comboEvent->setQuadraticMove(960,40,255,640,40,150,620,40,0,3);
		}
		comboEvent->ActivateEvent();
		AddEvent(comboEvent);

		// First Combo: this is an ATTACK
		if ( table[nbTable].getClashCount() == 2 )
			table[nbTable].TriggerCharacterAction(KD_ATTACK);
		// 4 Hit combo, this is a Strong Attack
		else if ( table[nbTable].getClashCount() == 4 )
			table[nbTable].TriggerCharacterAction(KD_STRONGATTACK);
	}

	if (table[nbTable].getClashCountFinished() > 1)
	{
		for (int i=0; i<table[nbTable].getClashCountFinished(); i++)
			table[1-nbTable].addLine();

		last_line_added_time[1-nbTable] = SDL_GetTicks();

		KD_TextEvent *warningEvent;
		warningEvent = new KD_TextEvent();
		warningEvent->SetTextFont(main_font);
		warningEvent->SetText("Warning!\n%d lines\ncoming!",table[nbTable].getClashCountFinished());

		if (nbTable==1)
		{
			warningEvent->PrintFromLeft();
			warningEvent->setQuadraticMove(20,80,255,20,80,200,20,80,0,3);
		}
		else if (nbTable==0)
		{
			warningEvent->PrintFromRight();
			warningEvent->setQuadraticMove(620,80,255,620,80,200,620,80,0,3);
		}
		warningEvent->setBlinking(0.2f,0.2f);
		warningEvent->ActivateEvent();
		AddEvent(warningEvent);

		// if more than 3 lines is send, the character is "attacked"
		if ( table[nbTable].getClashCountFinished() >= 3 )
			table[1-nbTable].TriggerCharacterAction(KD_ATTACKED);
	}

	//if (table.getClashCount() > maxClashCount && table.getClashCount()!=1) 
	//	maxClashCount = table.getClashCount();

	return true;
}
 
bool KD_DuelController::DisplayFinishState()
{
	background->Display(0,0);

	for (int i=0; i<KD_DUEL_NB_PLAYERS; i++)
	{
		if (hasWon[i])
		{
			table[i].DisplayOnWin();

			if(Display::getTicks() - timeOfNewState > 2000)
			{
				// Print this only when the event is finished.
				if (Display::getIsOpenGL())
					main_font->xycoloralpharotozoomcenteredprintf(255,255,255,255, 1.5f,1.5f, (i==0) ? 32 + 7*32.0f/2 : 384.0f + 7*32/2,240, -70*3.14f/180, (i==0) ? 32 + 7*32.0f/2 : 384.0f + 7*32/2,240, "Victory!");
				else
					main_font->xycenteredprintf((i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, "Victory!");
			}
		}
		else
		{
			table[i].DisplayOnLose();
			if (Display::getIsOpenGL())
				main_font->xycoloralpharotozoomcenteredprintf(255,255,255,255, 1.5f,1.5f, (i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, -70*3.14f/180, (i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, "You lose!");
			else
				main_font->xycenteredprintf((i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, "You lose!");
		}
	}
	
	//Display::DisplayFramesPerSecond (12,42+2+2,20);

	main_font->xycenteredprintf (SCR_HW,YTIME,"%d", timeRemainingWhenFinished);
	main_font->xycenteredprintf (SCR_HW,YPLAYER1,"%d", max(150-table[0].getNbGemsDropped(),0));
	main_font->xycenteredprintf (SCR_HW,YPLAYER2,"%d", max(150-table[1].getNbGemsDropped(),0));

	for (int i=0; i<nbRounds*KD_DUEL_NB_PLAYERS; i++)
	{
		// Yeah! That line could nearly be in Goto++!
        cup[i]->Display((i<nbRounds)? 32+7*32-(nbRounds-i)*32 : 384+(i-nbRounds)*32, 442);
	}

	if (Display::getTicks() - timeOfNewState > 7000)
	{
		bool isGoingToContinue = false;
		for (int i=0; i<KD_DUEL_NB_PLAYERS; i++)
		{
			if (nbWon[i]==nbRounds)
			{
				controllerState = KD_CSTATE_CONTINUE;
				timeOfNewState = Display::getTicks();
				PLAYMUSIC ("art/puzzlelose.ogg");
				isGoingToContinue = true;

				if (i==0)
				{
					BindKeyDown(SDLK_LEFT,   KD_A_DECREASECONTINUE);
					BindKeyDown(SDLK_RIGHT,  KD_A_DECREASECONTINUE);
					BindKeyDown(SDLK_RETURN, KD_A_NOACTION);
					BindKeyDown(SDLK_RSHIFT, KD_A_CONTINUE);
					BindKeyDown(SDLK_RCTRL,  KD_A_CONTINUE);
				}
				else if (i==1)
				{
					BindKeyDown(SDLK_x,   KD_A_DECREASECONTINUE);
					BindKeyDown(SDLK_c,  KD_A_DECREASECONTINUE);
					BindKeyDown(SDLK_v, KD_A_NOACTION);
					BindKeyDown(SDLK_LSHIFT, KD_A_CONTINUE);
					BindKeyDown(SDLK_LCTRL,  KD_A_CONTINUE);
				}
				 
			}
		}
		if (isGoingToContinue == false)
		{
			InitReadyState();
			controllerState = KD_CSTATE_READY;
		}
	}

	return true;
}

bool KD_DuelController::DisplayReadyState()
{
	background->Display(0,0);

	for (int i=0; i<KD_DUEL_NB_PLAYERS; i++)
	{
		table[i].Display();
	}
	
	//Display::DisplayFramesPerSecond (12,42+2+2,20);

	main_font->xycenteredprintf (SCR_HW,YTIME,"%d", 90);
	main_font->xycenteredprintf (SCR_HW,YPLAYER1,"%d", 150-table[0].getNbGemsDropped());
	main_font->xycenteredprintf (SCR_HW,YPLAYER2,"%d", 150-table[1].getNbGemsDropped());

	for (int i=0; i<nbRounds*KD_DUEL_NB_PLAYERS; i++)
	{
		// Yeah! That line could nearly be in Goto++!
        cup[i]->Display((i<nbRounds)? 32+7*32-(nbRounds-i)*32 : 384+(i-nbRounds)*32, 442);
	}

	if (Display::getTicks() - timeOfNewState > 3000)
	{
		InitRound();

		KD_TextEvent *goText = new KD_TextEvent();
		goText->SetTextFont(main_font);
		goText->PrintFromCenter();
		goText->SetText("GO!");
		goText->setQuadraticMove(320,240,255,255,255,255,0.5f,0.5f,0,
								320,/*150*/240,255,255,255,/*128*/250,1.0f,1.0f,0,
								320,/*150*/240,255,255,255,0,2.0f,2.0f,0,1);
		goText->ActivateEvent();
		AddEvent(goText);

 
		controllerState = KD_CSTATE_PLAYING;
	}

	return true;
}

bool KD_DuelController::DisplayContinueState()
{
	background->Display(0,0);

	for (int i=0; i<KD_DUEL_NB_PLAYERS; i++)
	{
		if (hasWon[i])
		{
			table[i].DisplayOnWin();

			if (Display::getIsOpenGL())
				main_font->xycoloralpharotozoomcenteredprintf(255,255,255,255, 1.5f,1.5f, (i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, -70*3.14f/180, (i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, "Victory!");
			else
				main_font->xycenteredprintf((i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, "Victory!");
		}
		else
		{
			table[i].DisplayOnLose();

			main_font->xycenteredprintf((i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,240, "Continue?");
			main_font->xycenteredprintf((i==0) ? 32.0f + 7*32/2 : 384.0f + 7*32/2,290, "%d",max((timeOfNewState+10000-Display::getTicks())/1000,0));
		}
	}

	main_font->xycenteredprintf (SCR_HW,YTIME,"%d", timeRemainingWhenFinished);
	main_font->xycenteredprintf (SCR_HW,YPLAYER1,"%d", max(150-table[0].getNbGemsDropped(),0));
	main_font->xycenteredprintf (SCR_HW,YPLAYER2,"%d", max(150-table[1].getNbGemsDropped(),0));

	for (int i=0; i<nbRounds*KD_DUEL_NB_PLAYERS; i++)
	{
		// Yeah! That line could nearly be in Goto++!
        cup[i]->Display((i<nbRounds)? 32+7*32-(nbRounds-i)*32 : 384+(i-nbRounds)*32, 442);
	}

	if (Display::getTicks() - timeOfNewState > 15000)
	{
		KD_Application::getApplication()->disableController(this);
		KD_Application::getApplication()->enableController("Background", KD_LAST_POS);
		KD_Application::getApplication()->enableController("TitleController", KD_FRONT_POS);
	}

	return true;
}

bool KD_DuelController::Quit()
{
	

	delete music;

	return true;  
}

bool KD_DuelController::OnEnable()
{
	LoadSprites();

	BindKeyDown(SDLK_ESCAPE, KD_A_QUIT);

	nbRounds=2;
	
	cup = new KD_SpriteInstancePtr[nbRounds*KD_DUEL_NB_PLAYERS];

	controllerState = KD_CSTATE_READY;
	

	for (int i=0; i<KD_DUEL_NB_PLAYERS; i++)
	{
		table[i].Init();

		table[i].setWidth(7);
		table[i].setHeight(12);
		table[i].setGemWidth(32);
		table[i].setGemHeight(32);

		table[i].setAllBorders(border);
		table[i].setLineSprite(lineSprite);
		table[i].activateDoors(false);
				
		table[i].setGems(gem);
		table[i].loadGemsToCome("art/tableDuel.txt");
		table[i].SetLoopGems(false);

        for (int gem_type= 0; gem_type< KD_GEM_NB_KINDS; gem_type++)
		  table[i].setGemProbability (gem_type, 12);

        table[i].setPosition (i==0?32:384, 50);
		table[i].InitSet();

		#ifndef NO_SOUND
			table[i].setPlopSound(plopSound);
			table[i].setGemsDownSound(gemsDownSound);
			table[i].setGemsUpSound(gemsUpSound);
			table[i].setClashSounds(clashSound);
		#endif
		
		nbWon[i]=0;

		for (int j=0; j<nbRounds; j++)
		{
			cup[j+i*nbRounds] = (KD_SpriteInstance*) cupSprite->createInstance();
		}

	}

	music->Load("art/survival.ogg");
	music->SetVolume(80);
	music->PlayMusic();

	InitReadyState();

	return true;
}

bool KD_DuelController::OnDisable()
{
	delete[] cup;

	UnloadSprites();

	table[0].deInit();
	table[0].desalloc();
	table[1].deInit();
	table[1].desalloc();

    music->StopMusic();
	music->CloseMusic();

	return true;
}
