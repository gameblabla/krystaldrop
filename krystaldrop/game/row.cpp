#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "parameter.h"
#include "row.h"
#include "set.h"
#include "../video/gem.h"


#ifdef DEBUG
#include <stdio.h>
#endif

KD_Row::KD_Row()
{ content= NULL;
  content_size= 0;
  hand= NULL;
  set= NULL;
  param= NULL;
  height_in_gem= 0;
}


KD_Row::KD_Row (short Height_In_Gems, KD_Hand* Hand, KD_Parameters* Param)
{ assert (Height_In_Gems);
  height_in_gem= Height_In_Gems;
  
  assert (Hand);
  hand= Hand;
  
  assert (Param);
  param= Param;
  
  set= NULL; // must be set later by SetSet. (yes, i know it's a stupid name)
  
  // worst case: each block is made of one gem, 1 pixel afar from another block
  // one more block for the end marker.
  // nb+ speed+ accel+ array of KD_Gem*
#define GEMBLOCK_HEADER_SIZE 3 // in short's
   
  content_size= (Height_In_Gems+ 1)* (GEMBLOCK_HEADER_SIZE* sizeof(short)+ sizeof(KD_Gem*));
  content= (short*) malloc( content_size);
  assert (content);
  if (content!= NULL) 
    memset (content, 0, content_size);
    
  content_browse= NULL;
  content_browse_rest= 0;
}


KD_Row::~KD_Row()
{ 
  if (content!= NULL)
  { free (content);
    content= NULL;
    content_size= 0;
  }
  
  set= NULL;
  hand= NULL;
  param= NULL;
  height_in_gem= 0;
}


void KD_Row::SetSet (KD_Set* Set)
{
  assert (Set);
  set= Set;
}


void KD_Row::SetParam (KD_Parameters* Param)
{ assert (Param);
  param= Param;
}


#define GEM_PTR_SIZE (sizeof(KD_Gem*)/sizeof(short))
#define B_GEM_PTR(p_block,n)   ( p_block+ GEMBLOCK_HEADER_SIZE+ n*GEM_PTR_SIZE ) 

#define B_READ_NB(p_block)     ( *(p_block+0) )
#define B_READ_SPEED(p_block)  ( *(p_block+1) )
#define B_READ_ACCEL(p_block)  ( *(p_block+2) )
#define B_READ_GEM(p_block,n)  *( (KD_Gem**) B_GEM_PTR(p_block,n))

#define B_WRITE_NB(p_block,x)    p_block[0]= x;
#define B_WRITE_SPEED(p_block,y) p_block[1]= y;
#define B_WRITE_ACCEL(p_block,z) p_block[2]= z;
#define B_WRITE_GEM(p_block,n,p) *(KD_Gem**) B_GEM_PTR(p_block,n)= p;


#define B_NEXT_BLOCK(p_block) ( B_GEM_PTR(p_block,B_READ_NB(p_block)) )
#define B_IS_LAST_BLOCK(p_block) ( *(B_NEXT_BLOCK(p_block))== 0 )


short KD_Row::CountGems()
{ assert (content);
  if (content== NULL) return 0;
 
  short i= 0;
  short* p= content;
  char nb= 0;
  
  do 
  { nb= B_READ_NB(p);
    if (nb== 0) break;
    
    i+= nb;
    p= B_NEXT_BLOCK(p);
  } while (1);
  
  return i;
}


signed KD_Row::SplitLastBlockAt (short* last_block, short index)
{ if (index== 0) return 0; // nothing to do

  /* split a block will increase the buffer usage by 3*2 bytes */
  /* check if there is an overflow */
  if ( ((long)B_NEXT_BLOCK(last_block)- (long)content)+ GEMBLOCK_HEADER_SIZE* sizeof(short) > content_size)
    return KD_E_BUFOVERFLOW; // content overflows
  
  short nb_gem_before= B_READ_NB(last_block);
  if (index>= nb_gem_before) return KD_E_GEMINDEXINVALID;

  /* memory content, in shorts */
  /* before:    nb  sp ac p1 p1 p2 p2 .. pindex pindex .. pend pend 0 */
  /* after:   index sp ac p1 p1 .. pindex-1 pindex-1 (nb-index) ?? ?? pindex pindex.. pend pend 0 */
  { short* to_be_moved= (short*) B_GEM_PTR(last_block, index);
    memmove ( to_be_moved+ GEMBLOCK_HEADER_SIZE,
              to_be_moved,
              (nb_gem_before- index)* GEM_PTR_SIZE* sizeof(short)+ 2); /* +2 for the ending 0 */
  }
        
  last_block[0]= index; // last_block is in fact no longer the last_block
  B_NEXT_BLOCK(last_block)[0]= nb_gem_before- index;
  
  return 0;
}


signed KD_Row::AddAtTop (KD_Gem* Gem)
{ assert (content);
// assert (Gem);

/* # when is it impossible to add ? */

// set->IsLineDown() return KD_E_ADD_IMPOSSIBLENOW;
 /* if (!B_IS_LAST_BLOCK(content))
  { short* last_block= B_NEXT_BLOCK(content);
    if (!B_IS_LAST_BLOCK(last_block)) return KD_E_ADDIMPOSSIBLENOW;
  }*/

  short* first_block= content;
  short nb= B_READ_NB(first_block);
  
  if (nb!= 0) /* the row is not empty, we must make space in the buffer for the new gem */
  { if (CountGems()>= height_in_gem) return KD_E_ROWFULL;
  
    short* last_block= content;
    while (!B_IS_LAST_BLOCK(last_block)) last_block= B_NEXT_BLOCK(last_block);

    /* create a 4-bytes space before the first gem pointer */
    long how_many= ((long) B_NEXT_BLOCK(last_block))+ 2- /* points after on the final 0 */
                   (long) B_GEM_PTR(first_block,0);

    short* to_be_moved= (short*) B_GEM_PTR(first_block,0);
    memmove ( to_be_moved+ GEM_PTR_SIZE,
              to_be_moved,
              how_many* sizeof(short));
    /* content buffer should not overflow, content_size should have been chosen correctly */
  }    

  B_WRITE_NB(first_block,nb+ 1);
  B_WRITE_SPEED(first_block,speed_line_down);
  B_WRITE_ACCEL(first_block,accel_line_down);
  B_WRITE_GEM(first_block,0,Gem);
  // B_READ_GEM(first_block,0)->SetY ( -gem_height_in_pixel);
  
  return 0;
}


void KD_Row::PrintRow ()
{
  short* p= content;
  int i;
  printf ("***\n");
  while (*p!= 0)
  {  printf ("block [%d, (%d, %d)]\n", B_READ_NB(p), B_READ_SPEED(p), B_READ_ACCEL(p));
     for (i= 0; i< B_READ_NB(p); i++)
         printf ("  Gem %p\n", B_READ_GEM(p,i));
     p= B_NEXT_BLOCK(p);
  }
}


signed KD_Row::Update()
{
    
  return 0;
}


signed KD_Row::GetFirstY()
{ assert (content);

  content_browse= content;
  content_browse_rest= B_READ_NB(content_browse);
  if (content_browse_rest== 0) return KD_E_NOMOREGEM;
  
  return (B_READ_GEM(content_browse,content_browse_rest))->y;
}


signed KD_Row::GetNextY()
{ assert (content_browse_rest);
  assert (content_browse);
  assert (content);
  
  content_browse_rest--;
  if (content_browse_rest== 0) 
  { if (B_IS_LAST_BLOCK(content_browse)) return KD_E_NOMOREGEM;
    content_browse= B_NEXT_BLOCK(content_browse);
  }
  
  return (B_READ_GEM(content_browse,content_browse_rest))->y; 
  /* it's not ordered, but it's not important for the drawing */
}