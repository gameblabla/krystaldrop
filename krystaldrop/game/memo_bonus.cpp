#include "memo_bonus.h"

KD_MemoBonus::KD_MemoBonus()
{ memo= 0; 
}
  
void KD_MemoBonus::SaveSpecial (unsigned short bonus)
{ memo|= bonus; 
}

signed short KD_MemoBonus::GetNextSpecial()
{ signed short res;
  
  if (memo== 0) return 0;
    
  /* trick y= x & (-x) to get the least significant bit of x set to 1 */
  res= memo & (-memo);
  
  /* but if res= KD_BONUS_PA, maybe it's a KD_BONUS_DA */
  if (res== KD_BONUS_PA)
    if ((memo & KD_BONUS_DA)== KD_BONUS_DA)
      res= KD_BONUS_DA;
    
  /* remove res from memo and return */
  memo&= ~res;
  return res;
}