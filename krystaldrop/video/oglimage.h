#ifndef OGLImage_H
#define OGLImage_H

#include <SDL/SDL.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "image.h"

class TACCRes;
class KD_ImageManager;

/**
	Class containing an image (OpenGL Texture).
	These images are used by ImageManager
  */
class KD_OGLImage : public KD_Image
{
	friend class KD_ImageManager;
protected:
	/**
		The constructor is made private, it can only be called by ImageManager which is a friend class of Image.
	*/
	KD_OGLImage();

	/**
		The destructor is made private, it can only be called by ImageManager which is a friend class of Image.
	*/
	virtual ~KD_OGLImage();

	/**
		The OpenGL Texture.
	*/
	GLuint texture;

	/**
		Does the texture possesses an alpha channel?
	*/
	bool hasAlphaChannel;

	int width;
	int height;
	/// Texture coordinates of right side
	float tx;
	/// Texture coordinates of bottom side
	float ty;

public:
	/**
		Makes a KD_Image from a SDL_Surface
		The SDL_Surface must NOT be used afterwards and must NOT be deleted.
		KD_Image will delete the image for the user.
	*/
	virtual void makeImageFromSDLSurface(SDL_Surface *surf);

	/**
		Loads an image from the file fileName.
	*/
	virtual void Load(char *fileName);

	/**
		Loads an image from the file fileName which stands in the accFile accFile.
		If accFile == 0, attempts to load the image from the real file fileName.
	*/
	virtual void Load(TACCRes *accFile, char *fileName);

	/**
		Displays the image in (x,y).
	*/
	virtual void Display(int x, int y);

	/**
		Displays the image in (x,y) with some alpha blending
	*/
	virtual void DisplayAlpha(int x, int y, int alpha);

	/**
		Displays the image in (x,y) with some alpha blending, zooming and coloring.
		This is only supported by the OpenGL renderer.
		The SDL renderer won't perform zoom nor coloring
	*/
	virtual void DisplayColorZoom(int x, int y, int r, int g, int b, int alpha, float resizeX, float resizeY);

	/**
		Displays the image with some alpha blending, zooming and coloring and rotation.
		The rotation center must be given and the angle too. The sprite will be displayed usually in (x,y), resized and then rotated.
		This is only supported by the OpenGL renderer.
		The SDL renderer won't perform zoom nor coloring
	*/
	virtual void DisplayColorZoomRotate(int x, int y, int r, int g, int b, int alpha, float resizeX, float resizeY, int rotX, int rotY, float angle);

	/**
		Sets the colorkey of the image.
	*/
	virtual void setColorKey(Uint32 key);
	virtual void setColorKey(Uint8 r, Uint8 g, Uint8 b);

	/**
		Returns the SDL_Surface to do dirty things on it.
	*/
	virtual SDL_Surface *getSDL_Surface();

	/**
		Returns the height of the surface.
	*/
	virtual int getHeight();

	/**
		Returns the height of the surface.
	*/
	virtual int getWidth();
	
	/**
		Disable the alpha transparency blitting on the surface.
	*/
	virtual void disableAlpha();

	/**
		Enable the alpha transparency blitting on the surface.
		But alpha is always enabled in OpenGL....
	*/
	virtual void enableAlpha();
	
	//{
	/**
		Converts the alpha surface to a surface without alpha transparency, but with a color-key.
		This function has no meaning here, since OpenGL does handle only alpha surfaces, and it does it well.
		So we will put a stub instead.
	*/
	virtual void convertToColorKey(Uint8 r, Uint8 g, Uint8 b, int alphaTrigger);
	virtual void convertToColorKey(unsigned int key, int alphaTrigger);
	//}

	/**
		Resizes the current image with the specified ratio.
		Returns true on success.
		Will fail only if the ratio makes a SDL_Surface with a length less than 1 or a height less than 1.
	*/
	virtual bool resize(float ratio);

	/**
		Copy the KD_Image to another similar image.
		This new copy won't be referenced in the KD_ImageManager!
	*/
	virtual KD_Image *copy();
};

#endif