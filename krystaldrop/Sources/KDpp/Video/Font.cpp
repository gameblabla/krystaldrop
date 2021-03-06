#include <SDL/SDL_image.h>
#include "SDL_rotozoom.h"
#include <SDL/SDL_ttf.h>

#include "Display.h"
#include "Font.h"
#include "../Resources/ResourceManager.h"
#include "Image.h"
#include "SdlImage.h"
#include "../Tools/Textfile.h"
#include "../Tools/Logfile.h"


KD_Font::KD_Font () : KD_Resource()
{
    for (int i=0; i<256; i++)
    {
        letters[i]=0;
        spacing[i]=0;
    }

    isColorKey = false;
}

KD_Font::KD_Font (const KD_FilePath &fileName, const string font_dir): KD_Resource()
{
    for (int i=0; i<256; i++)
    {
        letters[i]=0;
        spacing[i]=0;
    }

    Load(fileName, font_dir);
    isColorKey = false;
}

KD_Font::KD_Font (const KD_FilePath &fileName, int ptsize, int style, int r, int g, int b, const string font_dir): KD_Resource()
{
    for (int i=0; i<256; i++)
    {
        letters[i]=0;
        spacing[i]=0;
    }

    LoadTTF(fileName,ptsize,style,r,g,b,font_dir);
    isColorKey = false;
}

KD_Font::~KD_Font ()
{
    free();
}

void KD_Font::free()
{
    for (int i=0; i<256; i++)
    {
        if (letters[i]==0)
            continue;

        // Set the pointers of all the duplicated values to 0 to avoid freeing an already freed memory block.
        for (int j=i+1; j<256; j++)
        {

            if (letters[i]==letters[j])
            {
                letters[j]=0;
            }
        }
        KD_ResourceManager::GetResourceManager()->DeleteUnreferencedImage(letters[i]);
        letters[i]=0;
    }
}

bool KD_Font::Load (const KD_FilePath &fileName, const string font_dir)
{
    KD_TextFile file;

  KD_FilePath filename= fileName;
  if (font_dir!= "") filename.PrefixDirectory (font_dir);
    file.Load(filename);

    char buf[256];
    int res = sscanf(file.GetPosition(), "%s", buf);
    if (!res)
    {
        KD_LogFile::printf2("Warning, no font-file loaded!");
        return false;
    }
    file.JumpLine();

  string font_file= buf;
  if (font_dir!= "") font_file= font_dir+ font_file;

    KD_Image *img = KD_ResourceManager::GetResourceManager()->LoadImage2((char*)font_file.c_str(), false);
    SDL_Surface *surf = img->getSDL_Surface();

    int nb, x1, x2, y1, y2;
    SDL_PixelFormat *fmt = Display::screen->format;

    // Number of pixels for the space and the return character.
    sscanf(file.GetPosition(), "%d %d", &spaceSize, &returnSize);
    file.JumpLine();

    // Sets the array of letters to the NULL pointer
    for (int i=0; i<256; i++)
        letters[i]=0;

    while (1)
    {
        if (file.IsEOF())
            break;

        res = sscanf(file.GetPosition(), "%d %d %d %d %d", &nb, &x1, &x2, &y1, &y2);
        file.JumpLine();
        if (res != 5)
        {
            continue;
        }

        if (x2-x1<=0 || y2-y1<=0)
        {
            printf("Warning, unvalid font coordinates passed to the program: x1=%d y1=%d x2=%d y2=%d for character %c\n",x1,y1,x2,y2,nb);
            KD_LogFile::printf("Warning, unvalid font coordinates passed to the program: x1=%d y1=%d x2=%d y2=%d for character %c\n",x1,y1,x2,y2,nb);
        }

        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            int amask = 0x000000ff;
        #else
            int amask = 0xff000000;
        #endif

        SDL_Surface *surfLetter = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, x2-x1, y2-y1, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, amask);

        SDL_LockSurface(surf);
        SDL_LockSurface(surfLetter);

        // WE MUST BLIT THE ALPHA CHANNEL TOO!
        for (int j=0; j<y2-y1 ; j++)
            for (int i=0; i<x2-x1 ; i++)
            {
                ((unsigned int *)surfLetter->pixels)[i+j*(surfLetter->pitch>>2)] = ((unsigned int *)surf->pixels)[i+x1+(j+y1)*(surf->pitch>>2)];
            }

        SDL_UnlockSurface(surf);
        SDL_UnlockSurface(surfLetter);

        KD_Image *letterImg = KD_ResourceManager::GetResourceManager()->NewUnreferencedImage();
        letterImg->makeImageFromSDLSurface(surfLetter);

        // If default value for non existing characters
        if (nb==0)
        {
            for (int i=0; i<256; i++)
                if (letters[i]==0)
                {
                    letters[i]=letterImg;
                    spacing[i]=letterImg->GetWidth();
                }
        }
        else
        {
            letters[nb]=letterImg;
            spacing[nb]=letterImg->GetWidth();
        }
    }

    // Typically, fontHeight is the height of a A:
    fontHeight = letters[(unsigned char)'A']->GetHeight();

    //KD_ResourceManager::GetResourceManager()->ReleaseResource(buf);
  KD_ResourceManager::GetResourceManager()->ReleaseResource((char*)font_file.c_str());

    return true;
}

bool KD_Font::LoadTTF(const KD_FilePath &fileName, int ptsize, int style, int r, int g, int b, const string font_dir)
{
    free();

    // Initialisation
    if (TTF_Init()!= 0)
    {
        KD_LogFile::printf2("Warning, unable to initialize the truetype-font engine\n");
        return false;
    }

    TTF_Font *ttfFont;
    const char* p_data;
    unsigned long size;

    if (fileName.IsArchived())
    {
        KD_ArchiveManager::FetchResource (fileName.GetArchiveName(), fileName.GetPath(), &p_data, &size);
        ttfFont = TTF_OpenFontRW(SDL_RWFromMem((void*)p_data, size), 0, ptsize);
    }
    else
    {
        ttfFont = TTF_OpenFont(fileName.GetPath().c_str(), ptsize);
    }
    if (ttfFont== NULL)
    {
        KD_LogFile::printf2("Warning, could not load ttf file \"%s\".\n",fileName.GetPath().c_str());
        TTF_Quit();
        return false;
    }

    TTF_SetFontStyle(ttfFont, style);

    returnSize = TTF_FontLineSkip(ttfFont);
    fontHeight = TTF_FontHeight(ttfFont);

    int minx,maxx,miny,maxy,advance;
    TTF_GlyphMetrics(ttfFont, ' ', &minx, &maxx, &miny, &maxy, &spaceSize);

    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;

    letters[0] = NULL;

    for (int i=1; i<256; i++)
    {
        char text[2];
        text[0]=i;
        text[1]='\0';
        SDL_Surface *surfLetter = TTF_RenderText_Blended(ttfFont, text, color);

        if (surfLetter == NULL)
        {
            KD_LogFile::printf2("Warning, letter %s (character number %d) from font \"%s\" could not be generated by TTF engine\n",text,(unsigned char)text[0],fileName.GetPath().c_str());
            letters[i] = NULL;
            continue;
        }

        letters[i] = KD_ResourceManager::GetResourceManager()->NewUnreferencedImage();
        letters[i]->makeImageFromSDLSurface(surfLetter);


        TTF_GlyphMetrics(ttfFont, i, &minx, &maxx, &miny, &maxy, &advance);
        spacing[i] = advance;
    }

    TTF_Quit();

    if (fileName.IsArchived())
        KD_ArchiveManager::FreeResource (fileName.GetArchiveName(), fileName.GetPath());


    return true;
}

void KD_Font::xyprintf(float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    float xWork= x,yWork= y;
    int i=0;

    while (buf[i])
    {
        switch ((unsigned char) buf[i])
        {
        case ' ':
            xWork += spaceSize;
            break;

        case '\n':
            xWork = x;
            yWork += returnSize;
            break;

        default:
            letters[(unsigned char)buf[i]]->Display(xWork, yWork-letters[(unsigned char)buf[i]]->GetHeight());

            xWork += spacing[(unsigned char)buf[i]];
            break;
        }
        i++;
    }

}

void KD_Font::xyalphaprintf(int alpha, float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    float xWork= x,yWork= y;
    int i=0;

    while (buf[i])
    {
        switch ((unsigned char) buf[i])
        {
        case ' ':
            xWork += spaceSize;
            break;

        case '\n':
            xWork = x;
            yWork += returnSize;
            break;

        default:
            letters[(unsigned char)buf[i]]->DisplayAlpha(xWork, yWork-letters[(unsigned char)buf[i]]->GetHeight(),alpha);

            xWork += spacing[(unsigned char)buf[i]];
            break;
        }
        i++;
    }

}

void KD_Font::xycoloralpharotozoomprintf(int r, int g, int b, int alpha, float resizeX, float resizeY, float rotX, float rotY, float angle, float x, float y, char *str, ...)
{   unsigned char buf[PRINTF_BUF_SIZE];
    float xWorkf= x, yWorkf= y;
    int i= 0;

    va_list argptr;
    va_start (argptr, str);
    vsnprintf ((char*)buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    while (buf[i])
    { switch (buf[i])
      { case ' ': xWorkf+= spaceSize* resizeX;
                  break;
        case '\n':xWorkf= (float)x;
                  yWorkf+= returnSize* resizeY;
                  break;
        default: letters[buf[i]]->DisplayColorZoomRotate(
                   (int) xWorkf,
                   (int) yWorkf- letters[buf[i]]->GetHeight(),
                   r, g,b,alpha,resizeX, resizeY, rotX, rotY, angle);
                 xWorkf+= spacing[(unsigned char)buf[i]]* resizeX;
                 break;
      }
      i++;
    }
}

int KD_Font::ComputeLength(char *buf)
{   int length=0;
    int pastLength=0;
    int i=0;

    while (buf[i])
    {
        switch ((unsigned char) buf[i])
        {
        case ' ':
            length += spaceSize;
            break;

        case '\n':
            if (length>pastLength)
                pastLength = length;
            length = 0;
            break;

        default:
            length += spacing[(unsigned char)buf[i]];
            break;
        }
        i++;
    }

    if (pastLength>length) length=pastLength;

    return length;
}

void KD_Font::xyrightprintf(float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    int length = ComputeLength(buf);

    xyprintf(x-length, y, buf);
}

void KD_Font::xycoloralpharotozoomrightprintf(int r, int g, int b, int alpha, float resizeX, float resizeY, float rotX, float rotY, float angle, float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    int length = ComputeLength(buf);

    xycoloralpharotozoomprintf(r, g, b, alpha, resizeX, resizeY, rotX, rotY, angle, (int)(x-length*resizeX), y, buf);
}

void KD_Font::xyalpharightprintf(int alpha, float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    int length = ComputeLength(buf);

    xyalphaprintf(alpha, x-length, y, buf);
}

void KD_Font::xycenteredprintf(float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    int length = ComputeLength(buf);

    xyprintf(x-length/2, y, buf);
}

void KD_Font::xyalphacenteredprintf(int alpha, float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    int length = ComputeLength(buf);

    xyalphaprintf(alpha, x-length/2, y, buf);
}

void KD_Font::xycoloralpharotozoomcenteredprintf(int r, int g, int b, int alpha, float resizeX, float resizeY, float rotX, float rotY, float angle, float x, float y, char *str, ...)
{   char buf[PRINTF_BUF_SIZE];

    va_list argptr;
    va_start (argptr, str);
    vsnprintf (buf, PRINTF_BUF_SIZE, str, argptr);
    buf[PRINTF_BUF_SIZE- 1]= 0;
    va_end (argptr);

    int length = ComputeLength(buf);

    xycoloralpharotozoomprintf(r, g, b, alpha, resizeX, resizeY, rotX, rotY, angle, (int)(x-length*resizeX/2), y, buf);
}

KD_Font *KD_Font::resize(float ratio)
{
    KD_Font *newFont = new KD_Font();

    newFont->spaceSize = (int)(spaceSize*ratio);
    newFont->returnSize = (int)(returnSize*ratio);

    newFont->isColorKey = isColorKey;
    newFont->colorKey = colorKey;

    int i;

    for (i=0; i<256; i++)
    {
      newFont->letters[i]=0;
      newFont->spacing[i] = (int)(spacing[i]*ratio);
    }

    for (i=0; i<256; i++)
    {
        if (letters[i]==0)
            continue;

        if (newFont->letters[i]!=0)
            continue;

        if (letters[i]->GetWidth()*ratio < 1 || letters[i]->GetHeight()*ratio < 1)
            // If resized sprite is not wide enough to be seen, do not resize it.
            newFont->letters[i] = letters[i]->copy();
        else
        {
            // Else resize it.
            newFont->letters[i] = letters[i]->copy();
            newFont->letters[i]->resize(ratio);
        }

        if (isColorKey)
            newFont->letters[i]->SetColorKey(colorKey);

        for (int j=i+1; j<256; j++)
            if (letters[i]==letters[j])
              newFont->letters[j]=newFont->letters[i];
    }

    return newFont;
}

void KD_Font::ConvertToColorKey(unsigned int key, int alphaTrigger)
{
    isColorKey=true;
    colorKey = key;

    //int j;
    for (int k=0; k<255; k++)
    {
        if (letters[k]==0) continue;

        letters[k]->ConvertToColorKey(key, alphaTrigger);


    /*  if (letters[k]->format->Amask == 0) continue;

        SDL_Surface *surf = SDL_CreateRGBSurface(SDL_HWSURFACE, letters[k]->w, letters[k]->h, letters[k]->format->BitsPerPixel, letters[k]->format->Rmask, letters[k]->format->Gmask, letters[k]->format->Bmask, 0);

        SDL_LockSurface(letters[k]);
        SDL_LockSurface(surf);

        for (j=0; j<letters[k]->h ; j++)
            for (int i=0; i<letters[k]->w ; i++)
            {
                int alpha;
                int pixel=((unsigned int *)letters[k]->pixels)[i+j*(letters[k]->pitch>>2)];
            #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                alpha = pixel & 0x000000ff;
            #else
                alpha = (pixel & 0xff000000)>>24;
            #endif
                if (alpha<alphaTrigger)
                    ((unsigned int *)surf->pixels)[i+j*(surf->pitch>>2)] = key;
                else
                    ((unsigned int *)surf->pixels)[i+j*(surf->pitch>>2)] = pixel;

            }

        SDL_UnlockSurface(letters[k]);
        SDL_UnlockSurface(surf);

        SDL_FreeSurface(letters[k]);
        for (j=k+1; j<256; j++)
            if (letters[k]==letters[j])
            {
              letters[j]=surf;
            }

        letters[k]=surf;*/
        //SDL_SetAlpha(letters[k], SDL_RLEACCEL | SDL_SRCALPHA , 127);
    //SDL_SetColorKey(((KD_SDLImage *)(letters[k]))->image, SDL_SRCCOLORKEY | SDL_RLEACCEL, key);
    }
}

int KD_Font::GetReturnSize() const
{
    return returnSize;
}

int KD_Font::GetFontHeight() const
{
    return fontHeight;
}
