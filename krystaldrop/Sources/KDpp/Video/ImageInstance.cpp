#include "ImageInstance.h"

#include "Image.h"

KD_ImageInstance::KD_ImageInstance(KD_Image *image) : KD_DisplayableInstance(image)
{

}

KD_ImageInstance::~KD_ImageInstance()
{
}

void KD_ImageInstance::Display(int x, int y)
{
	((KD_Image*)resource)->Display(x,y);
}

void KD_ImageInstance::DisplayAlpha(int x, int y, int alpha)
{
	((KD_Image*)resource)->DisplayAlpha(x,y,alpha);
}

/*void KD_ImageInstance::DisplayRotateX(int x, int y, float angle)
{
	((KD_Image*)resource)->DisplayRotateX(x,y,angle);
}
    
void KD_ImageInstance::DisplayRotateY(int x, int y, float angle)
{
	((KD_Image*)resource)->DisplayRotateY(x,y,angle);
}*/

void KD_ImageInstance::DisplayColorZoom(int x, int y, int r, int g, int b, int alpha, float resizeX, float resizeY)
{
	((KD_Image*)resource)->DisplayColorZoom(x,y,r,g,b,alpha,resizeX,resizeY);
}

void KD_ImageInstance::DisplayColorZoomRotate(int x, int y, int r, int g, int b, int alpha, float resizeX, float resizeY, int rotX, int rotY, float angle)
{
	((KD_Image*)resource)->DisplayColorZoomRotate(x,y,r,g,b,alpha,resizeX,resizeY,rotX,rotY,angle);
}

void KD_ImageInstance::DisplayAlphaPart(int x, int y, int alpha, int xPart, int yPart, int widthPart, int heightPart)
{
	((KD_Image*)resource)->DisplayAlphaPart(x,y,alpha,xPart,yPart,widthPart,heightPart);
}

void KD_ImageInstance::DisplayFullParameters(int x1, int y1, int r1, int g1, int b1, int alpha1, int x2, int y2, int r2, int g2, int b2, int alpha2, int x3, int y3, int r3, int g3, int b3, int alpha3, int x4, int y4, int r4, int g4, int b4, int alpha4)
{
	((KD_Image*)resource)->DisplayFullParameters(x1,y1,r1,g1,b1,alpha1,x2,y2,r2,g2,b2,alpha2,x3,y3,r3,g3,b3,alpha3,x4,y4,r4,g4,b4,alpha4);
}

void KD_ImageInstance::DisplayFullParameters(int x1, int y1, const KD_Color &c1, int alpha1, int x2, int y2, const KD_Color &c2, int alpha2, int x3, int y3, const KD_Color &c3, int alpha3, int x4, int y4, const KD_Color &c4, int alpha4)
{
	((KD_Image*)resource)->DisplayFullParameters(x1,y1,(int)(c1.r*255),(int)(c1.g*255),(int)(c1.b*255),alpha1,x2,y2,(int)(c2.r*255),(int)(c2.g*255),(int)(c2.b*255),alpha2,x3,y3,(int)(c3.r*255),(int)(c3.g*255),(int)(c3.b*255),alpha3,x4,y4,(int)(c4.r*255),(int)(c4.g*255),(int)(c4.b*255),alpha4);
}


int KD_ImageInstance::getHeight()
{
	return 	((KD_Image*)resource)->getHeight();
}

int KD_ImageInstance::getWidth()
{
	return 	((KD_Image*)resource)->getWidth();
}
