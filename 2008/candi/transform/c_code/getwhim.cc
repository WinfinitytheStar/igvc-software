#include "stdio.h"
#include "image.h"
#include "Buffer2D.h"
#include "PixelRGB.h"
#include "getwhim.h"


static Buffer2D<PixelRGB> whim;


static int numruns=0;
b2drgb& getwhim (b2drgb& im) {

	int r,g,b,orn;
	char R,G,B;
	whim.copyFrom (im);
	for (int i=0;i< (im.width) * (im.height);i++) {
		
		//get pixel data
		r=whim[i].r;
		g=whim[i].g;
		b=whim[i].b;
		orn=r-b;

		// Clear out image
		whim[i].r=0;
		whim[i].g=0;
		whim[i].b=0;
		R=G=B=0;

		//thresholding
		if (g!=0 && b>102 && 256*b/g>171 && 256*g/ (r+b) <154) {
			R=255;
			G=255;
			B=255;
		}

		//update image
		whim[i].r=R;
		whim[i].g=G;
		whim[i].b=B;
	}

	numruns++;

	//printf ("frame: %d\n",numruns);

	return whim;
}
