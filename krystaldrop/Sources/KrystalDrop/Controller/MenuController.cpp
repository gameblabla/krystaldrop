#include "../global.h"

#include "../../KDpp/Controller/Application.h"
#include "../../KDpp/Controller/EventManager.h"
#include "../../KDpp/Resources/GlobalResourceSet.h"
#include "MenuController.h"
#include "../Names.h"
//#include "../util/direct.h"
#include "../Video/Event/AnimTextEvent.h"
#include "../Video/Background.h"
#include "../../KDpp/Video/Display.h"
#include "../../KDpp/Video/Font.h"
#include "../../KDpp/Video/Sprite.h"
#include "../../KDpp/Video/SpriteInstance.h"
#include "../../KDpp/Sound/Music.h"
//#include "../../KDpp/Video/imagemanager.h"

static char* DESCRIBE_TEXT[]= 
  { "Solo game",
    "2 players vs",
    "Options" };
    
short KD_MenuController::menu_type= KD_MENU_GAME;

KD_MenuController::KD_MenuController(): KD_Controller(), KD_ResourceSet()
{ //GETBACK(back);
}

KD_MenuController::~KD_MenuController()
{ }


bool KD_MenuController::Init()
{ 
  NEW (ar_r, KD_Sprite);

  BindKeyDown(SDLK_ESCAPE, 1);
  BindKeyDown(SDLK_SPACE, 2); 
  BindKeyDown(SDLK_RETURN, 2);
  BindKeyDown(SDLK_UP, 3);
  BindKeyDown(SDLK_DOWN, 4);

/*  main_font= Display::Slapstick;
  text_font= Display::Slapstick->resize(0.8);
  mini_font= Display::Slapstick->resize(0.5);
  assert (main_font);
  CHECK_ALLOC (text_font);
  CHECK_ALLOC (mini_font);*/
  main_font = (KD_Font *)KD_GlobalResourceSet::GetGlobalResource()->GetResource("big font");
  text_font = (KD_Font *)KD_GlobalResourceSet::GetGlobalResource()->GetResource("text font");
  mini_font = (KD_Font *)KD_GlobalResourceSet::GetGlobalResource()->GetResource("main font");

  //LoadResourceFile("art/title/titleRes.txt");
  //back = new KD_Background((KD_DisplayableResource *)GetResource("title1"));

  //PLAYMUSIC(MUSIC_NAME[KD_MUS_INTRO]);
  music = new KD_Music();
  
  /*TACCRes* acc= new TACCRes();
  assert (acc);
  if (acc== NULL) return false;
  
  signed res= acc->LoadACC ("art/menu.acc");
  assert (res== 0);
  if (res< 0) return false;*/
  LoadResourceFile("art/menu/menu.txt");
  
  ar_r=(KD_Sprite *)GetResource("rightarrow");
  //bool b= ar_r[0].Load(acc,"ar_r.txt"); assert (b); if (b== false) return false;
  
  
  //DELETE (acc); 
  
  return true;
}


bool KD_MenuController::ProcessEvent(int value)
{ switch(value)
  { case 1: KD_Application::GetApplication()->SendStopEvent(); return true;
    case 2: if (menu_type== KD_MENU_GAME) 
            { switch (pos)
              { case 0: 
					KD_Application::GetApplication()->DisableController(this);
					KD_Application::GetApplication()->EnableController ("Charsel");
					break;
                case 1: 
					KD_Application::GetApplication()->DisableController(this);
					KD_Application::GetApplication()->EnableController ("Charsel2");
					break;
				case 2:
					KD_Application::GetApplication()->DisableController(this);
					KD_Application::GetApplication()->EnableController ("Controls");
              }
              return true;
            } else assert (0);
    case 3: /* up */
            if (menu_type== KD_MENU_GAME && pos> 0)
            { pos--;
              ar_ry -= 60;
              UpdateDescription();
            }
            return true;
    case 4: /* down */
            if (menu_type== KD_MENU_GAME && pos< 2)
            { pos++;
              ar_ry+= 60;
              UpdateDescription();
            }
            return true;
  }
  
  return false;
}


bool KD_MenuController::Display()
{ 
#ifdef DISPLAY_FPS
	Display::DisplayFramesPerSecond (12,42+2+2,20);
#endif   
  
  //Display::clearScreen();
  //back->Display();
  
  if (menu_type== KD_MENU_GAME)
  { text_font->xycenteredprintf (SCR_HW, 220, "Survival");
    text_font->xycenteredprintf (SCR_HW, 280, "Double Duel");
	text_font->xycenteredprintf (SCR_HW, 340, "Options");
    ar_ri->Display(ar_rx,ar_ry);
  }

  return true;
}


bool KD_MenuController::Quit()
{
  //CLOSEMUSIC();
  //KD_EventManager::getEventManager()->DeleteAllEvents();
  DeleteAllEvents();
  
/*KD_ImageManager* image_manager= KD_ImageManager::getImageManager();
  assert (image_manager);
  if (image_manager== NULL) return false;
  */  
  //delete back;
  //ReleaseResource("title1");

  ReleaseResource("rightarrow");

  KD_GlobalResourceSet::GetGlobalResource()->ReleaseResource("big font");
  KD_GlobalResourceSet::GetGlobalResource()->ReleaseResource("text font");
  KD_GlobalResourceSet::GetGlobalResource()->ReleaseResource("main font");


  delete music;
  
//  delete main_font;
//  DELETE (mini_font);  
//  DELETE (text_font);
  /*printf ("%p\n", ar_ri);*/
  //DELETE (ar_ri);
  //DELETE (ar_r);
  //ar_r[0].~KD_Sprite();

  return true;
}


void KD_MenuController::SetMenuType (short menu_type_)
{ menu_type= menu_type_;
}


void KD_MenuController::UpdateDescription()
{ if (Description!= NULL) 
    Description->RemoveText(); 
  NEW (Description, KD_MessageText (DESCRIBE_TEXT[pos], mini_font, SCR_HW, 420));
  Description->ActivateEvent();
}

bool KD_MenuController::OnEnable()
{
	music->Load(MUSIC_NAME[KD_MUS_INTRO]);
	music->PlayMusic();

	ar_ri = (KD_SpriteInstance *)ar_r->createInstance();
  //NEW (ar_ri, KD_SpriteInstance (ar_r));
//  ar_ri->x= 120;
//  ar_ri->y= 190;
  ar_rx = 120;
  ar_ry = 190;
  ar_ri->setAnim(0);
  
  //DELETE (acc);
    
  Description= NULL;
  pos= 0;
  UpdateDescription();

    if (menu_type== KD_MENU_GAME)
    NEW (Title, KD_BouncingText ("Game select", main_font, SCR_HW, 90));
  
	Title->ActivateEvent();
	AddEvent(Title);
  



	return true;
}

bool KD_MenuController::OnDisable()
{
	music->StopMusic();
	music->CloseMusic();

	ar_r->deleteInstance(ar_ri);
    DELETE (Title);

	return true;
}

bool KD_MenuController::Process()
{
	return false;
}