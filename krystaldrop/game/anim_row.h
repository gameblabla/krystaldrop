#ifndef ANIM_ROW_H
#define ANIM_ROW_H

#include "../video/gem.h"
#include "row.h"

class KD_GenericSet;

class KD_AnimatedRow: public KD_Row
{ protected:
   short is_taking_gem;  /* if param->IsTakeHand() && is_taking_gem, then it's this
                            row whose gems are begin taken. */
   KD_Hand* hand;
   KD_Memo* set_memo;    /* set->memo */
   KD_Memo* remove_memo; /* which gems we should remove after the update */    
  
   void UpdateBlocks();
  public:
   KD_AnimatedRow::KD_AnimatedRow (short Height_In_Gems, short x_Offset, 
                                   KD_Hand* Hand, KD_Parameters* Param, KD_Memo* Memo);
  ~KD_AnimatedRow();
 
  static signed CanClash (short type1, short type2);

  signed IsLineDown(); /* is there a gem going down as part of a line down ? */
  
  /* movement */ 
  void Update();
  
  signed TakeFromBottom();
  signed DropAtBottom();
  
  signed HaveGemsToRemove();
  signed RemoveGemsInFirstBlock();
 
  friend class KD_GenericSet; 
};

#endif