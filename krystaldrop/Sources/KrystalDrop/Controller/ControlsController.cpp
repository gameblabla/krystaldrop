#include <assert.h>
#include "ControlsController.h"
#include "../Controller/KDApplication.h"
#include "../Names.h"
#include "../Video/Event/AnimTextEvent.h"
#include "../../KDpp/Resources/GlobalResourceSet.h"
#include "../../KDpp/Tools/FilePath.h"
#include "../../KDpp/Sound/Music.h"
#include "../../KDpp/Video/Font.h"
#include "../../KDpp/Controller/UserInterface/Keyboard.h"
#include "../../KDpp/Controller/UserInterface/Frame.h"
#include "../../KDpp/Controller/UserInterface/Button.h"
#include "../../KDpp/Controller/UserInterface/StaticText.h"
#include "../../KDpp/Controller/UserInterface/CheckBox.h"
#include "../../KDpp/Controller/UserInterface/EditField.h"

#define BUTTON_P1LEFT 1
#define BUTTON_P1RIGHT 2
#define BUTTON_P1TAKE 3
#define BUTTON_P1DROP 4
#define BUTTON_P1ADDLINE 5
#define BUTTON_P2LEFT 6
#define BUTTON_P2RIGHT 7
#define BUTTON_P2TAKE 8
#define BUTTON_P2DROP 9
#define BUTTON_P2ADDLINE 10

#define BUTTON_OK 11


KD_ControlsController::KD_ControlsController() : KD_WidgetController()
{
}

KD_ControlsController::~KD_ControlsController()
{
}

bool KD_ControlsController::Init()
{
    return true;
}

bool KD_ControlsController::ProcessEvent(int value)
{
    // If there is an unregistered event (not a button, so we hope: a key)
    // There's going to be problems with the joystick here!
    if (isWaitingKeyPress)
    {
        // if a button has been pressed, this is not a key pressed...
        if (value)
            return false;

        int key = (int) KD_Keyboard::GetKeyboard()->GetLastSDLKey();
        KD_ControlsConfig *config = KD_ControlsConfig::GetSingleton();

        config->SetKeyCode(keyPressed, key);
        ((KD_Button *)(((KD_WidgetContainer*)GetWidget("frame1"))->GetWidget(GetButtonNameFromKey(keyPressed))))->SetText(KD_Keyboard::GetKeyName(key));

        isWaitingKeyPress = false;

        if (pressAnyKeyText)
        {
            delete pressAnyKeyText;
            pressAnyKeyText = 0;
        }

        return true;
    }


    switch (value)
    {
    case BUTTON_P1LEFT:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p1left;
        break;
    case BUTTON_P1RIGHT:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p1right;
        break;
    case BUTTON_P1TAKE:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p1down;
        break;
    case BUTTON_P1DROP:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p1up;
        break;
    case BUTTON_P1ADDLINE:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p1extra;
        break;
    case BUTTON_P2LEFT:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p2left;
        break;
    case BUTTON_P2RIGHT:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p2right;
        break;
    case BUTTON_P2TAKE:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p2down;
        break;
    case BUTTON_P2DROP:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p2up;
        break;
    case BUTTON_P2ADDLINE:
        isWaitingKeyPress = true;
        keyPressed = KD_ControlsConfig::p2extra;
        break;
    case BUTTON_OK:
        KD_Application::GetApplication()->DisableController (this);
        KD_Application::GetApplication()->EnableController ("TitleController");
        break;
    }

    if (isWaitingKeyPress)
    {
        AskForKey();
    }

    return false;
}

bool KD_ControlsController::Quit()
{
    return true;
}

bool KD_ControlsController::OnEnable()
{
    isWaitingKeyPress = false;
    pressAnyKeyText = 0;

    KD_KDApplication* pApplication = KD_KDApplication::GetApplication();

#ifndef NO_MUSIC
    music = new KD_Music();
    music->Load(pApplication->GetArtFile(MUSIC_NAME[KD_MUS_HIGHSCORES]).c_str());
    music->PlayMusic();
#endif

    LoadResourceFile(pApplication->GetArtFile("UI.zip/window/window_resources.txt"));
    LoadResourceFile(pApplication->GetArtFile("UI.zip/button/button_resources.txt"));

    RegisterResource("UI font", new KD_Font(pApplication->GetArtFile("fonts.acc/OLDRRG__.TTF"),24));

    KD_FilePath fontName = GetFileName("UI font");
    RegisterResource("button_font", fontName);
    RegisterResource("button_over_font", fontName);
    RegisterResource("button_pressed_font", fontName);
    RegisterResource("statictext_font", fontName);
    //RegisterResource("checkbox_font", fontName);
    //RegisterResource("editfield_font", fontName);

    AddWidget("frame1", new KD_Frame(50,50,540,380));
    GetWidget("frame1")->SetAlpha(225);

    KD_ControlsConfig *config = KD_ControlsConfig::GetSingleton();

    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1leftText", new KD_StaticText(70,150,"Player 1 Left:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1leftButton", new KD_Button(197,125,120,30,config->GetControlName(KD_ControlsConfig::p1left) ,BUTTON_P1LEFT));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1rightText", new KD_StaticText(70,200,"Player 1 Right:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1rightButton", new KD_Button(197,175,120,30,config->GetControlName(KD_ControlsConfig::p1right) ,BUTTON_P1RIGHT));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1takeText", new KD_StaticText(70,250,"Player 1 Take:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1takeButton", new KD_Button(197,225,120,30,config->GetControlName(KD_ControlsConfig::p1down) ,BUTTON_P1TAKE));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1dropText", new KD_StaticText(70,300,"Player 1 Drop:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1dropButton", new KD_Button(197,275,120,30,config->GetControlName(KD_ControlsConfig::p1up) ,BUTTON_P1DROP));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1addlineText", new KD_StaticText(70,350,"Player 1 AddLine:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p1addlineButton", new KD_Button(197,325,120,30,config->GetControlName(KD_ControlsConfig::p1extra) ,BUTTON_P1ADDLINE));

    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2leftText", new KD_StaticText(320,150,"Player 2 Left:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2leftButton", new KD_Button(452,125,120,30,config->GetControlName(KD_ControlsConfig::p2left) ,BUTTON_P2LEFT));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2rightText", new KD_StaticText(320,200,"Player 2 Right:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2rightButton", new KD_Button(452,175,120,30,config->GetControlName(KD_ControlsConfig::p2right) ,BUTTON_P2RIGHT));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2takeText", new KD_StaticText(320,250,"Player 2 Take:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2takeButton", new KD_Button(452,225,120,30,config->GetControlName(KD_ControlsConfig::p2down) ,BUTTON_P2TAKE));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2dropText", new KD_StaticText(320,300,"Player 2 Drop:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2dropButton", new KD_Button(452,275,120,30,config->GetControlName(KD_ControlsConfig::p2up) ,BUTTON_P2DROP));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2addlineText", new KD_StaticText(320,350,"Player 2 AddLine:"));
    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("p2addlineButton", new KD_Button(452,325,120,30,config->GetControlName(KD_ControlsConfig::p2extra) ,BUTTON_P2ADDLINE));

    ((KD_WidgetContainer*)GetWidget("frame1"))->AddWidget("buttonOK", new KD_Button(250,365,140,40,"Ok!",BUTTON_OK));

    main_font = (KD_Font *) KD_GlobalResourceSet::GetGlobalResource()->GetResource("big font");

    return KD_WidgetController::OnEnable();
}

bool KD_ControlsController::OnDisable()
{
    KD_GlobalResourceSet::GetGlobalResource()->ReleaseResource("big font");

#ifndef NO_MUSIC
    music->StopMusic();
    delete music;
#endif

    if (pressAnyKeyText)
    {
        delete pressAnyKeyText;
        pressAnyKeyText = 0;
    }

    RemoveAll();

    UnRegisterResource("UI font");
    UnRegisterResource("button_font");
    UnRegisterResource("button_over_font");
    UnRegisterResource("button_pressed_font");
    UnRegisterResource("statictext_font");

    return KD_WidgetController::OnDisable();
}

void KD_ControlsController::AskForKey()
{
    pressAnyKeyText = new KD_BouncingText ("Press Any Key!", main_font, 320, 240);
    pressAnyKeyText->ActivateEvent();
    AddEvent(pressAnyKeyText);

}

string KD_ControlsController::GetButtonNameFromKey(KD_ControlsConfig::KD_Keys key)
{
    string buttonName;

  switch  (key)
    {
    case KD_ControlsConfig::p1left:
        return "p1leftButton";
    case KD_ControlsConfig::p1right:
        return "p1rightButton";
    case KD_ControlsConfig::p1down:
        return "p1takeButton";
    case KD_ControlsConfig::p1up:
        return "p1dropButton";
    case KD_ControlsConfig::p1extra:
        return "p1addlineButton";
    case KD_ControlsConfig::p2left:
        return "p2leftButton";
    case KD_ControlsConfig::p2right:
        return "p2rightButton";
    case KD_ControlsConfig::p2down:
        return "p2takeButton";
    case KD_ControlsConfig::p2up:
        return "p2dropButton";
    case KD_ControlsConfig::p2extra:
        return "p2addlineButton";
    default:
        assert(false);
        return "ERROR!";
    }
}
