#include "sdlimage.h"

#include <assert.h>

#include "../util/logfile.h"
#include "Display.h"

#include "SDL/SDL_image.h"
#include "SDL_rotozoom.h"



KD_SDLImage::KD_SDLImage() : image(0)
{
	countRef=0;
}

KD_SDLImage::~KD_SDLImage()
{
	if (image!=0)
		SDL_FreeSurface(image);
}

void KD_SDLImage::makeImageFromSDLSurface(SDL_Surface *surf)
{
	image = SDL_DisplayFormatAlpha(surf);
	SDL_FreeSurface(surf);
}


void KD_SDLImage::Load(char *fileName)
{
	char tmp[512];
	snprintf(tmp, sizeof(tmp), "art/img/%s", fileName);
	// Load the surface
	SDL_Surface *surfLoaded = IMG_Load(tmp);

	assert(surfLoaded);

	// Converts the surface to the display format
	image = SDL_DisplayFormatAlpha(surfLoaded);

	// Free the old surface
	SDL_FreeSurface(surfLoaded);
}

void KD_SDLImage::Display(int x, int y)
{
	SDL_Rect rect ;
	rect.x = x;
	rect.y = y;
	assert (image);
  
	SDL_BlitSurface(image, 0, Display::screen, &rect);
}

void KD_SDLImage::DisplayAlpha(int x, int y, int alpha)
{
	SDL_Rect rect ;
	rect.x = x;
	rect.y = y;
	assert (image);
  
	SDL_SetAlpha(image, SDL_RLEACCEL | SDL_SRCALPHA , alpha);
	SDL_BlitSurface(image, 0, Display::screen, &rect);
	SDL_SetAlpha(image, SDL_RLEACCEL , 0);
}

void KD_SDLImage::DisplayRotateX (int x, int y, float alpha)
{ Display (x, y); }

void KD_SDLImage::DisplayRotateY (int x, int y, float alpha)
{ Display (x, y); }

void KD_SDLImage::DisplayColorZoom(int x, int y, int r, int g, int b, int alpha, float resizeX, float resizeY)
{
	DisplayAlpha(x,y,alpha);
}

void KD_SDLImage::DisplayColorZoomRotate(int x, int y, int r, int g, int b, int alpha, float resizeX, float resizeY, int rotX, int rotY, float angle)
{
	DisplayAlpha(x,y,alpha);
}

void KD_SDLImage::setColorKey(Uint32 key)
{
	SDL_SetColorKey(image, SDL_SRCCOLORKEY | SDL_RLEACCEL, key);
}

void KD_SDLImage::setColorKey(Uint8 r, Uint8 g, Uint8 b)
{
	setColorKey(SDL_MapRGB(Display::screen->format, r, g, b));
}

SDL_Surface *KD_SDLImage::getSDL_Surface()
{
	return image;
}

int KD_SDLImage::getHeight()
{
	return image->h;
}

int KD_SDLImage::getWidth()
{
	return image->w;
}

void KD_SDLImage::disableAlpha()
{
	SDL_SetAlpha(image, SDL_RLEACCEL, 0);
}

void KD_SDLImage::enableAlpha()
{
	SDL_SetAlpha(image, SDL_SRCALPHA | SDL_RLEACCEL, 0);
}


void KD_SDLImage::convertToColorKey(Uint8 r, Uint8 g, Uint8 b, int alphaTrigger)
{
	unsigned int key = SDL_MapRGB(Display::screen->format, r, g, b);
	convertToColorKey(key, alphaTrigger);
}

void KD_SDLImage::convertToColorKey(unsigned int key, int alphaTrigger)
{
	if (image->format->Amask == 0) return;

	SDL_Surface *surf = SDL_CreateRGBSurface(SDL_HWSURFACE, image->w, image->h, image->format->BitsPerPixel, image->format->Rmask, image->format->Gmask, image->format->Bmask, 0);

	SDL_LockSurface(image);
	SDL_LockSurface(surf);

	for (int j=0; j<image->h ; j++)
		for (int i=0; i<image->w ; i++)
		{
			int alpha;
			int pixel=((unsigned int *)image->pixels)[i+j*(image->pitch>>2)];
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

	SDL_UnlockSurface(image);
	SDL_UnlockSurface(surf);

	SDL_FreeSurface(image);
	image=surf;

	disableAlpha();
	setColorKey(key);
}

bool KD_SDLImage::resize(float ratio)
{
	SDL_Surface *newImage;

	// If resized image is not wide enough to be seen, do not resize it.
	if (image->w*ratio < 1 || image->h*ratio < 1)
		return false;
	
	newImage=zoomSurface(image, ratio, ratio, SMOOTHING_OFF);

	SDL_FreeSurface(image);
	image=newImage;
	
	return true;
}

KD_Image *KD_SDLImage::copy()
{
	KD_SDLImage *copy = new KD_SDLImage();

	copy->image = SDL_DisplayFormatAlpha(image);

	return copy;
}

KD_Image *KD_SDLImage::copy(int x, int y, int width, int height)
{
	// First, let's correct any impossible entry data asked.
	// width and height must be at least 1.
	if (width<=0) width=1;
	if (height<=0) height=1;

	int x2=x+width;
	int y2=y+height;

	if (x<0) x=0;
	if (x>=getWidth()) x=getWidth()-1;
	if (y<0) y=0;
	if (y>=getHeight()) y=getHeight()-1;
	if (x2<=0) x2=1;
	if (x2>getWidth()) x2=getWidth();
	if (y2<=0) y2=1;
	if (y2>getHeight()) y2=getHeight();

	width = x2-x;
	height = y2-y;

	KD_SDLImage *copy = new KD_SDLImage();

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		int amask = 0x000000ff;
	#else
		int amask = 0xff000000;
	#endif

	SDL_Surface *imageCopy = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, width, height, image->format->BitsPerPixel, image->format->Rmask, image->format->Gmask, image->format->Bmask, amask);

	SDL_LockSurface(image);
	SDL_LockSurface(imageCopy);

	// WE MUST BLIT THE ALPHA CHANNEL TOO!
	for (int j=0; j<height ; j++)
		for (int i=0; i<width ; i++)
		{
			((unsigned int *)imageCopy->pixels)[i+j*(imageCopy->pitch>>2)] = ((unsigned int *)image->pixels)[i+x+(j+y)*(image->pitch>>2)];
		}

	SDL_UnlockSurface(image);
	SDL_UnlockSurface(imageCopy);

	copy->image = imageCopy;
	return copy;
}
