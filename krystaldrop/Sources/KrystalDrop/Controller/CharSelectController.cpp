#include "../global.h"
#include "../Game/ControlsConfig.h"

#include "../../KDpp/Controller/Application.h"
#include "CharSelectController.h"
#include "BackgroundController.h"
#include "../../KDpp/Resources/GlobalResourceSet.h"
#include "../../KDpp/Controller/EventManager.h"
//#include "../util/direct.h"
#include "../Video/Event/AnimTextEvent.h"
#include "../Video/Background.h"
#include "../../KDpp/Video/Display.h"

#include "../../KDpp/Video/Image.h"
#include "../../KDpp/Sound/Music.h"
//#include "../video/imagemanager.h"

#define ANIM_SIZE 150

#define GET_SEL_CHAR(sel_char) ((10+ (10- (sel_char))% 10)% 10)

/* character select controller */
KD_CharSelectController::KD_CharSelectController(): KD_Controller(), KD_ResourceSet()
{ unsigned i;

  for (i= 0; i< KD_CSC_NB_SPRI; i++) spri[i]= NULL;
  for (i= 0; i< KD_CSC_NB_FONT; i++) font[i]= NULL;
  
  first_tick= SDL_GetTicks();
  
  //GETBACK (back);
  
  srand (SDL_GetTicks());
  sel_char= rand()% KD_NB_CHAR;
  angle= sel_char* (2* 3.14159/ KD_NB_CHAR)+ 3.14159;
  
  Title= NULL;
  Name1= NULL;
  
  //spr= new KD_Sprite;
  //CHECK_ALLOC (spr);
}
  

KD_CharSelectController::~KD_CharSelectController()
{ 
  //DELETE (spr); 
}


#define KD_CSC_CENTER_X1 380
#define KD_CSC_CENTER_Y1 300
#define KD_CSC_CENTER_R1 120
#define KD_CSC_E1 1.3
#define KD_CSC_CENTER_X2 845
#define KD_CSC_CENTER_Y2 350
#define KD_CSC_CENTER_R2 900
#define KD_CSC_E2 1
#define KD_CSC_ANGLE -0.35

void KD_CharSelectController::DisplayChars()
{ short i;
  float cur_angle;
  float incr= (2* 3.14159f)/ KD_NB_CHAR;
  float wanted_angle= sel_char* incr+ 3.14159;
  float speed;
  float inc= (Display::getTimeElapsed())* 100;
  
  if (fabs(wanted_angle- angle)< 0.001) speed= 0; else
  if (fabs(wanted_angle- angle)< 0.03) speed= (wanted_angle- angle)< 0 ? -0.0006 : 0.0006; else
      speed= (wanted_angle- angle)* 0.03;
    
  angle+= speed* inc;
  cur_angle= angle- KD_CSC_ANGLE;
  for (i= 0; i< KD_NB_CHAR; i++)
  { img[i]->Display (cos(cur_angle)* KD_CSC_CENTER_R2* KD_CSC_E2+ KD_CSC_CENTER_X2, 
                     sin(cur_angle)* KD_CSC_CENTER_R2+ KD_CSC_CENTER_Y2);    
    cur_angle+= incr;    
  }  
  
  cur_angle= angle;
  for (i= 0; i< KD_NB_CHAR; i++)
  { img[i+ KD_NB_CHAR]->Display (cos(cur_angle)* KD_CSC_CENTER_R1* KD_CSC_E1+ KD_CSC_CENTER_X1, 
                                 sin(cur_angle)* KD_CSC_CENTER_R1+ KD_CSC_CENTER_Y1);
    cur_angle+= incr;
  }  
}



bool KD_CharSelectController::Init()
{ 
  /* load the graphics */
  LoadResourceFile("art/characters/characters.txt");
  /*TACCRes* acc= new TACCRes();
  assert (acc);
  if (acc== NULL) return false;

  signed res;
  res= acc->LoadACC ("art/charsel.acc");
  assert (res== 0);
  if (res< 0) return false;
   
  KD_ImageManager* image_manager= KD_ImageManager::getImageManager();
  assert (image_manager);
  if (image_manager== NULL) return false;*/

  for (short i= 0; i< KD_CSC_NB_IMG; i++)
  { /*bool b= image_manager->Load (acc, CHAR_IMG_NAME[i]);
    assert (b);
    if (b== false) return false;
    img[i]= image_manager->getImage(CHAR_IMG_NAME[i]);*/
    img[i]= (KD_Image *)GetResource(CHAR_IMG_NAME[i]);
  }

  LoadResourceFile("art/menu/menu.txt");
  /*res= acc->LoadACC ("art/menu.acc");
  assert (res== 0);
  if (res< 0) return false;    */
  
  //b= spr[0].Load(acc,"ar_l.txt"); assert (b); if (b== false) return false;
  spr = (KD_Sprite *) GetResource("leftarrow");
  
  //spri[0]= new KD_SpriteInstance (&spr[0]); assert (spri[0]);
  spri[0] = (KD_SpriteInstance *)spr->createInstance();
//  spri[0]->x= (int) (KD_CSC_CENTER_X1- KD_CSC_CENTER_R1* 0.8);
//  spri[0]->y= KD_CSC_CENTER_Y1+ 12;
  spri[0]->setAnim(0);
  
  //DELETE (acc);

  font[0] = (KD_Font *)KD_GlobalResourceSet::GetGlobalResource()->GetResource("big font");
  font[1] = (KD_Font *)KD_GlobalResourceSet::GetGlobalResource()->GetResource("main font");
	
  //font[0]= Display::Slapstick;
  //font[1]= Display::Slapstick->resize(0.5);

  //PLAYMUSIC (MUSIC_NAME[KD_MUS_CHARSELECT]);
  music = new KD_Music();

  //KD_Config* Config= KD_Config::GetConfig();
  //assert (Config);

  KD_ControlsConfig *config = KD_ControlsConfig::getSingleton();
  
  // default bindings:
  BindKeyDown(SDLK_ESCAPE, 1);
  BindKeyDown(SDLK_SPACE, 2); 
  BindKeyDown(SDLK_RETURN, 2);
  
  // custom bindings:
  BindInput (config->getControlKind(KD_ControlsConfig::p1left) ,   config->getControlCode(KD_ControlsConfig::p1left),   3);
  BindInput (config->getControlKind(KD_ControlsConfig::p1right) ,   config->getControlCode(KD_ControlsConfig::p1right),   4);
  BindInput (config->getControlKind(KD_ControlsConfig::p1extra) ,   config->getControlCode(KD_ControlsConfig::p1extra),   2);
  BindInput (config->getControlKind(KD_ControlsConfig::p1up) ,   config->getControlCode(KD_ControlsConfig::p1up),   2);
  BindInput (config->getControlKind(KD_ControlsConfig::p1down) ,   config->getControlCode(KD_ControlsConfig::p1down),   2);

  return true;
}


bool KD_CharSelectController::ProcessEvent(int value)
{ switch (value)
  { case 1: KD_Application::getApplication()->sendStopEvent();
            return true;
    case 2: 
			KD_Application::getApplication()->disableController ("Background");
			KD_Application::getApplication()->disableController (this);
			KD_Application::getApplication()->enableController ("Survival");
            return true;
    case 3: sel_char--; 
            Name1->RemoveText();
            Name1= new KD_MessageText(CHAR_NAME[GET_SEL_CHAR(sel_char)], 
                                      font[1], 138, 120); 
            Name1->ActivateEvent();
			AddEvent(Name1);
            return true;
    case 4: sel_char++; 
            Name1->RemoveText(); 
            Name1= new KD_MessageText(CHAR_NAME[GET_SEL_CHAR(sel_char)], 
                                      font[1], 138, 120);
            Name1->ActivateEvent();
			AddEvent(Name1);
            return true;
  }

  return false;
}

bool KD_CharSelectController::Process()
{
	return false;
}

bool KD_CharSelectController::Display()
{
  //Display::clearScreen();

  //assert (back);
  //back->Display();

#ifdef DISPLAY_FPS
	Display::DisplayFramesPerSecond (12,42+2+2,20);
#endif    
  
  DisplayChars();
  Title->Display();
  Name1->Display();
  spri[0]->Display((int) (KD_CSC_CENTER_X1- KD_CSC_CENTER_R1* 0.8), KD_CSC_CENTER_Y1+ 12);
  
  return true;
}


bool KD_CharSelectController::Quit()
{ 

  //DELETE (font[1]);
  KD_GlobalResourceSet::GetGlobalResource()->ReleaseResource("big font");
  KD_GlobalResourceSet::GetGlobalResource()->ReleaseResource("main font");

  DELETE (spri[0]);  

  //CLOSEMUSIC();
  delete music;
  
  /*KD_ImageManager* image_manager= KD_ImageManager::getImageManager();
  assert (image_manager);
  if (image_manager== NULL) return false;*/
    
  for (short i= 0; i< KD_CSC_NB_IMG; i++)
	  ReleaseResource(CHAR_IMG_NAME[i]);
    //image_manager->releaseImage (CHAR_IMG_NAME[i]);

  ReleaseResource("leftarrow");

  return true;  
}

bool KD_CharSelectController::OnEnable()
{
	music->Load(MUSIC_NAME[KD_MUS_CHARSELECT]);
	music->PlayMusic();

	Title= new KD_BouncingText ("Character select", font[0], 320, 90);
	CHECK_ALLOC (Title);
	Title->ActivateEvent();
	AddEvent(Title);

	Name1= new KD_MessageText (CHAR_NAME[GET_SEL_CHAR(sel_char)], font[1], 138, 120);
	CHECK_ALLOC (Name1);
	Name1->ActivateEvent();
	AddEvent(Name1);

	//Display::flip();
	//Display::flip(); /* Init() takes time, the double flip is to be sure 
	//                    the flash is visible, by resetting timeElapsed */
	//Display::Flash();
	((KD_BackgroundController*)(KD_Application::getApplication()->getController("Background")))->Flash();


	return true;
}

bool KD_CharSelectController::OnDisable()
{
	music->StopMusic();
	music->CloseMusic();

	DELETE (Title);
//	DeleteAllEvents();

	/* Player 0 has selected his character */
	pl_chars[0]= GET_SEL_CHAR (sel_char);
	
	return true;
}
