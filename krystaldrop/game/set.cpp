#include <assert.h>
#include <stdlib.h>

#include "rowmacros.h"
#include "set.h"

#ifdef DEBUG
#include "stdio.h"
#endif

KD_GenericSet::KD_GenericSet (int Width, int Height, int max_in_hand, KD_Parameters* Param)
{ int i;

  field= (KD_Row**) malloc (Height* sizeof(KD_Row*));
  assert (field);

  memo= new KD_Memo();
  assert (memo);
  
  assert (max_in_hand);
  hand= new KD_Hand(max_in_hand);
  assert (hand);

  assert (Param);
  param= Param;
	
  assert (Width> 0);
  width= Width;
  for (i= 0; i< width; i++)
  { field[i]= new KD_Row(Height, i* param->Get_Width_Gem_In_Pixel(), 
                         hand, param);
	assert (field[i]);
    field[i]->SetMemo (memo);
  }
  
  enum_row= 0;
  pos= 0;
}


KD_GenericSet::~KD_GenericSet ()
{ if (field!= NULL)
  { int i;
	for (i= 0; i< height; i++)
	if (field[i]!= NULL) delete field[i];
    free (field);
	field= NULL;
	}
  
  if (hand!= NULL)
  { delete hand;
	hand= NULL;
  }
}


signed KD_GenericSet::MoveLeft()
{ if (pos== 0) return KD_E_CANTMOVE;
  pos--;
  
  return 0;
}


signed KD_GenericSet::MoveRight()
{ if (pos== width- 1) return KD_E_CANTMOVE;
  pos++;
  
  return 0;
}


signed KD_GenericSet::TakeGems()
{ assert (field[pos]);
  signed status;
  
  status= field[pos]->TakeFromBottom();
  field[0]->PrintRow();
  field[1]->PrintRow();
  /* check if all the gems have is_gem_down when a line is going down */
  /* if that's so, then the line going down has completely disappeared */
  /* this is quite improbable even width> 3, or the player is moving 
     extremely fast :-) */
  /* ## piece of code not tested */
  /* ## after some test, this piece of code seems buggy :/ */
  if (status== KD_S_LINEDOWNBROKEN)
  { signed index;

    for (index= 0; index< width; index++)
    { assert (field[index]);
      if (field[index]->is_gem_down== 1) break;
    }
    if (index== width)
    { /* the unlikely case has occurred. */
      param->ClearLineDown();
    }
    status= 0;
  }
  
  return status;
}


signed KD_GenericSet::DropGems()
{ assert (field[pos]);
  
  return field[pos]->DropAtBottom();
}


signed KD_GenericSet::AddLineAtTop (KD_Gem** Gem)
{ assert (Gem);
  assert (param);
  int index;
  int status;
  
  if (param->IsLineDown())
    return KD_E_ADDIMPOSSIBLE;
  
  for (index= 0; index< width; index++)
  { assert (Gem[index]);
    assert (field[index]);

    status= field[index]->AddAtTop (Gem[index]);
#ifdef DEBUG
    if (status== KD_E_ROWFULL) printf ("Row full has occured in KD_GenericSet::AddLineAtTop()\n");
#endif      
    if (status!= 0 &&
        status!= KD_E_ROWFULL) /* this is not too serious, the player will lose soon. */
                               /* # maybe it could create a memory leak though. */
                               /* # one solution could be to write NULL in Gem
                                    when a gem has been successfully added. 
                                    When the function returns, the non-NULL 
                                    pointers have to be freed. */
    { /* let's hope that if there is an error, it's at the beginning of the line */
      /* well, for now, it is impossible to have an error other than KD_E_ROWFULL. */
      assert (index== 0);
      param->SetLineDown();       // ## ??
      return status;
    }
  }

  /* update the bit field */  
  param->SetLineDown();

  return 0;
}


signed KD_GenericSet::RemoveGems()
{ //assert (remove_memo);
  signed row;
  signed index;
  KD_Gem* Gem;

  param->ClearRemoving();
  
  for (index= 0; index< width; index++)
    field[index]->RemoveGemsInFirstBlock();
  
/*  while (remove_memo->GetSize()!= 0)
  { Gem= remove_memo->GetGem(0);
    row= Gem->x/ param->Get_Width_Gem_In_Pixel();  
printf ("remove %p\n", Gem);     
    assert (field[row]);
    field[row]->RemoveGem (Gem);
  }  */
}

void KD_GenericSet::MarkAsToBeRemoved (KD_Gem* Gem)
{ signed row;
  
  row= Gem->x/ param->Get_Width_Gem_In_Pixel();  
  assert (field);
  assert (field[row]);
  field[row]->remove_memo->Remember (Gem);
}


void KD_GenericSet::Update()
{ assert (field);
  signed index;
 // signed status;
  
  for (index= 0; index< width; index++)
  { assert (field[index]);
    /*status=*/ field[index]->Update();
    /* check clash bit */
    /* check line broken ended */
  }
  
  //return 0;
}


KD_Gem* KD_GenericSet::GetFirstGem()
{ KD_Gem* gem= field[0]->GetFirstGem();
  
  enum_row= 0;

  while (gem== NULL)
  { enum_row++;
    if (enum_row== width) return NULL;
    gem= field[enum_row]->GetFirstGem();
  }  
  
  return gem;
}


KD_Gem* KD_GenericSet::GetNextGem()
{ KD_Gem* gem= field[enum_row]->GetNextGem(); 

  while (gem== NULL)
  { enum_row++;
    if (enum_row== width) return NULL;
    gem= field[enum_row]->GetFirstGem();
  }
  
  return gem;
}

/* temporary */
signed CanClash (short type1, short type2)
{ return type1==type2; }

//KD_GenericSet::KD_Gene
//KD_GenericSet::KD_GenericSet() { };
//KD_Set::KD_Set(): KD_GenericSet() { };
KD_Set::KD_Set(int Width, int Height, int max_in_hand, KD_Parameters* Param):
        KD_GenericSet (Width, Height, max_in_hand, Param) { };

signed KD_Set::TestBurstStart ()
{ assert (memo);
  
  KD_Gem* p_gem;
  short* p_block;  
  short size= memo->GetSize();
  short index;
  short gem_pos;
  short row;  
  short type;
  short nb;
  short index_min= 0;
  short index_max= 0; 

  if (size== 0) return 0; /* the player has taken back the gem while other were bursting. */
  
  for (index= 0; index< size; index++)
  { /* if the gem is not in the first block, then it should not start a burst. */
      /* Which row is being examined ? */
    p_gem= memo->GetGem(index);
    row= p_gem->x/ param->Get_Width_Gem_In_Pixel();
    assert (row>=0 && row< width);
      /* is the gem in the first block ? */
    
    gem_pos= field[row]->FindInFirstBlock (p_gem);
    printf ("gempos %d\n", gem_pos);
    if (gem_pos< 0) continue; /* not found -> do nothing */
      
    /* now, we look upwards and downwards the gem to find similar gems. */
    p_block= field[row]->GetFirstBlock();
    type= p_gem->GetType();    
    nb= B_READ_NB(p_block);
    index_min= gem_pos;
    index_max= gem_pos;

    while (index_max< nb- 1 && 
           CanClash (B_READ_GEM(p_block, index_max+ 1)->GetType(), type) )
             index_max++;
      
    while (index_min> 0 &&
           CanClash (B_READ_GEM(p_block, index_min- 1)->GetType(), type) )
             index_min--;

    if (index_max-index_min< 2) /* not enough to initiate a burst */
       continue;

    /* first, note the fact that gems are about to be removed */
    param->SetRemoving();
    
    /* first, mark the gems between index_min and index_max as visited
       and begin burst animation */
    for (gem_pos= index_min; gem_pos<= index_max; gem_pos++)
    { B_READ_GEM(p_block,gem_pos)->LaunchBurstAnimation();
      B_READ_GEM(p_block,gem_pos)->SetVisited();
    }
    
    for (gem_pos= index_min; gem_pos<= index_max; gem_pos++)
      RecurseBurst (row, gem_pos, type);
  }
  
  return 0;
}

void KD_Set::RecurseBurst (short row, short gem_pos, short type)
{ short* p_block;
  KD_Gem* p_gem;
 printf ("recurse %d %d\n", row, gem_pos); 
  /* left */
  if (row> 0)
  { p_block= field[row-1]->GetFirstBlock();
    if (B_READ_NB(p_block)> gem_pos)
    /* we can go left, note that B_READ_NB starts at 1 but not gem_pos */
    { p_gem= B_READ_GEM(p_block, gem_pos);
      assert (p_gem);
      if (!(p_gem->HasBeenVisited()) &&
          CanClash (p_gem->GetType(), type))
     { p_gem->LaunchBurstAnimation();
       p_gem->SetVisited();
       RecurseBurst (row- 1, gem_pos, type);
     }
    }
  }
  
  /* right */
  if (row< width- 1)
  { p_block= field[row+1]->GetFirstBlock();
    if (B_READ_NB(p_block)> gem_pos)
    { p_gem= B_READ_GEM(p_block, gem_pos);
      assert (p_gem);
      if (!(p_gem->HasBeenVisited()) &&
          CanClash (p_gem->GetType(), type))
     { p_gem->LaunchBurstAnimation();
       p_gem->SetVisited();
       RecurseBurst (row+ 1, gem_pos, type);
     }
    }
  }
  
  /* down */
  p_block= field[row]->GetFirstBlock();
  assert (p_block);
  if (gem_pos< B_READ_NB(p_block)- 1)
  { p_gem= B_READ_GEM(p_block, gem_pos+ 1);
    assert (p_gem);
    if (!(p_gem->HasBeenVisited()) &&
        CanClash (p_gem->GetType(), type))
    { p_gem->LaunchBurstAnimation();
      p_gem->SetVisited();
      RecurseBurst (row, gem_pos+ 1, type);
    }
  }

  /* up */
  if (gem_pos> 0)
  { p_gem= B_READ_GEM(p_block, gem_pos- 1);
    assert (p_gem);
    if (!(p_gem->HasBeenVisited()) &&
        CanClash (p_gem->GetType(), type))
    { p_gem->LaunchBurstAnimation();
      p_gem->SetVisited();
      RecurseBurst (row, gem_pos- 1, type);
    }
  }
}
