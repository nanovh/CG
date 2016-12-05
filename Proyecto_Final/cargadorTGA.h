#ifndef __CARGADORTGA_H__
#define __CARGADORTGA_H__

#include "Main.h"	

class CTga
{
	public:
		CTga();
		~CTga(); 
		
		int LoadTGA(char *filename);
		int freeData();
		void Elimina();
		
		unsigned char *imageData;
		int bpp,width,height;

		GLuint texID;
	private:
		
};

#endif