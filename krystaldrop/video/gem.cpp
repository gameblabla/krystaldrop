#include "gem.h"
#include "../game/set.h"
#include <assert.h>

KD_Gem::KD_Gem (KD_Set* Set, KD_Sprite* spr, short Type): KD_SpriteInstance(spr) 
{ assert (Set);
  set= Set;
  gem_type= Type;
  status= 0;
}

//KD_Gem::~KD_Gem() {}

void KD_Gem::SetNeedClashTest()
{ status|= KD_S_CHECKCLASH;
  
  assert (set);
  assert (set->memo);
  set->memo->Remember (this);
}

void KD_Gem::ClearNeedClashTest()
{ status&= ~KD_S_CHECKCLASH;
  
  assert (set);
  assert (set->memo);
  set->memo->Forget (this);
}

signed KD_Gem::NeedClashTest()
{ return (status & KD_S_CHECKCLASH);
}

void KD_Gem::SetVisited()
{ status|= KD_S_VISITED;
}

void KD_Gem::ClearVisited()
{ status&= ~KD_S_VISITED;
}

signed KD_Gem::HasBeenVisited()
{ return (status & KD_S_VISITED); 
}

void KD_Gem::SetRemoving()
{ status|= KD_S_G_REMOVING;
}

signed KD_Gem::IsRemoving()
{ return (status & KD_S_G_REMOVING);
}

void KD_Gem::LaunchBurst()
{ setAnim (1); 
  SetRemoving();
}

void KD_Gem::onFinishAnim (int animNo)
{ assert (set);
  if (animNo== 1)
  { /* if we are here, that means a gem (or a group of gems) which were blowing have
       just finished their bursting animation. Now we can test another clashes. */
    set->MarkAsToBeRemoved (this);
  }
}
