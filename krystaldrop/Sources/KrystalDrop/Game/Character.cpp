#include "Character.h"

#include "../../KDpp/Video/Sprite.h"
#include "../../KDpp/Video/SpriteInstance.h"
#include "XMLCharacterParser.h"

KD_CharacAnim::KD_CharacAnim(const KD_CharacAnim &that)
{
	animNumber = that.animNumber;
	voice = that.voice;
	proba = that.proba;
}

KD_CharacAnim& KD_CharacAnim::operator = (const KD_CharacAnim& that)
{
	if (&that != this)
	{
		this->KD_CharacAnim::~KD_CharacAnim();
		::new (this) KD_CharacAnim(that);
	}
	return *this;
}

//*************************************************************

KD_Character::KD_Character()
{
	backgroundCharac = 0;
	backgroundCharacInst = 0;

	chibi = 0;
	chibiInst = 0;

	for (int i=0; i<KD_NB_ANIMTYPE; i++)
		probaSum[i]=0;
}

KD_Character::~KD_Character()
{

}

void KD_Character::Load(const KD_FilePath &resourceFilePath, const KD_FilePath &actionFilePath)
{
	LoadResourceFile(resourceFilePath);
	
	backgroundCharac = (KD_Sprite *) GetResource("backgroundCharacter");
	chibi = (KD_Sprite *) GetResource("chibi");

	backgroundCharacInst = (KD_SpriteInstance *) backgroundCharac->createInstance();
	chibiInst = (KD_SpriteInstance *) chibi->createInstance();

	KD_XMLCharacterParser myParser(actionFilePath, this);
	myParser.Parse();
}

void KD_Character::Unload()
{
	backgroundCharac->deleteInstance(backgroundCharacInst);
	chibi->deleteInstance(chibiInst);
	backgroundCharacInst = 0;
	chibiInst = 0;

	ReleaseResource("backgroundCharacter");
	ReleaseResource("chibi");
}

void KD_Character::setBackGroundPos(int xBackground, int yBackground)
{
	this->xBackground = xBackground;
	this->yBackground = yBackground;
}

void KD_Character::DisplayBackground()
{
	backgroundCharacInst->Display(xBackground, yBackground);
}
