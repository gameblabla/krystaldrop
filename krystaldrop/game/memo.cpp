#include "memo.h"
#include "stdio.h"
#include "stdlib.h"
#include <assert.h>
#include "../video/gem.h"
KD_Gem* KD_Memo::GetGem (short index)
{ 
/* c reparti la chasse au bug... */
  signed t= memo[index]->gem_type;
  memo[index]->gem_type= t;
  return memo[index];
}


short KD_Memo::GetSize()
{ return memo.size();
}


void KD_Memo::Remember (KD_Gem* p_Gem)
{ memo.push_back (p_Gem);
  KD_Gem* p= p_Gem;
  signed t= p->gem_type;
  p->gem_type= t;  
}


short KD_Memo::FindGem (KD_Gem* p_Gem)
{ short size= memo.size();
  short i;

  for (i= 0; i< size; i++)
   if (memo[i]== p_Gem)
     return i;

  return -1;
}


void KD_Memo::Forget (short index)
{ memo.erase(memo.begin()+ index);
}


void KD_Memo::Forget (KD_Gem* p_Gem)
{ short index= FindGem(p_Gem); 
  
  assert (index>= 0);
  Forget (index);
}
