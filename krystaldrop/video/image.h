#ifndef Image_H
#define Image_H

#include <SDL/SDL.h>

class TACCRes;
class KD_ImageManager;

/**
	Class containing an image (SDL_Surfaces).
	These images are used by ImageManager
  */
class KD_Image
{
	friend class KD_ImageManager;
private:
	/**
		The constructor is made private, it can only be called by ImageManager which is a friend class of Image.
	*/
	KD_Image();

	/**
		The destructor is made private, it can only be called by ImageManager which is a friend class of Image.
	*/
	~KD_Image();

	/**
		The SDL_image.
	*/
	SDL_Surface *image;

	/**
		The reference count for that given Image
	*/
	int countRef;

public:
	/**
		Loads an image from the file fileName.
	*/
	void Load(char *fileName);

	/**
		Loads an image from the file fileName which stands in the accFile accFile.
		If accFile == 0, attempts to load the image from the real file fileName.
	*/
	void Load(TACCRes *accFile, char *fileName);

	/**
		Displays the image in (x,y).
	*/
	void Display(int x, int y);

	/**
		Sets the colorkey of the image.
	*/
	void setColorKey(Uint32 key);
	void setColorKey(Uint8 r, Uint8 g, Uint8 b);

	/**
		Returns the SDL_Surface to do dirty things on it.
	*/
	SDL_Surface *getSDL_Surface();

	/**
		Returns the height of the surface.
	*/
	int getHeight();

	/**
		Returns the height of the surface.
	*/
	int getWidth();
	
	/**
		Disable the alpha transparency blitting on the surface.
	*/
	void disableAlpha();

	//{
	/**
		Enable the alpha transparency blitting on the surface.
	*/
	void enableAlpha();
	void enableAlpha(int alpha);
	//}

	/**
		Converts the alpha surface to a surface without alpha transparency, but with a color-key.
		If the alpha channel is less than "alphaTrigger", the pixel will be converted to colorKey,
		else, the pixels will kept.
	*/
	void convertToColorKey(Uint8 r, Uint8 g, Uint8 b, int alphaTrigger);

	/**
		Resizes the current image with the specified ratio.
		Returns true on success.
		Will fail only if the ratio makes a SDL_Surface with a length less than 1 or a height less than 1.
	*/
	bool resize(float ratio);
};

#endif
