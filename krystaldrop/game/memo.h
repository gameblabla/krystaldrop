#ifndef MEMO_H
#define MEMO_H

#ifdef WIN32
#pragma warning(disable:4786)
#endif
#include <deque>
using namespace std;

class KD_Gem;

class KD_Memo
{ private:
  deque <KD_Gem*> memo;

  public:
   short   GetSize  (             );  
   KD_Gem* GetGem   ( short  index);
   void    Remember (KD_Gem* p_Gem);  
   short   FindGem  (KD_Gem* p_Gem);
   void    Forget   ( short  index);
   void    Forget   (KD_Gem* p_Gem);
};

#endif
