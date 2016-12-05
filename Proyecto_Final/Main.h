#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>		// Archivo de cabecera para Windows
#include <math.h>			// Archivo de cabecera para Funciones Matem�ticas
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdlib.h>			// Header File For Standard Library
#include <fstream>
#include <vector>
#include <crtdbg.h>
#include "GL/gl.h"			// Archivo de cabecera para la libreria OpenGL32
#include "GL/glu.h"			// Archivo de cabecera para la libreria GLu32
#include "GL/glaux.h"		// Archivo de cabecera para la libreria Glaux
#include "GL/glext.h"		// Archivo de cabecera para la libreria Glext
#include "Vector.h"
#include "cargadorTGA.h"

int  ManejaTeclado();

struct paramObjCam
{
	
	bool disparando;
	CVector PosicionObj;	//La posici�n (x,y,z) del objeto
	CVector PosicionObjAnt;	//La posici�n anterior (x,y,z) del objeto
	CVector Direccion;		//La direcci�n en que se dirige el objeto en forma de vector=(x,y,z)
	CVector DireccionCam;   //La direcci�n que define la posici�n de la c�mara respecto al personaje
	CVector PosicionCam;    //La posici�n de la c�mara que sigue al objeto
	CVector ObjetivoCam;	//El punto (x,y,z) que est� viendo la c�mara; en este caso al objeto

	float VelocidadObj;		//La velocidad a la que se mueve el objeto
	float DistanciaCam;		//La distancia que la c�mara est� separada del objeto
	float AngDir;			//Se usa para llevar control del angulo para el c�lculo del vector de direcci�n
	float AngDirCam;		//Se usa para llevar control del angulo para el c�lculo de posici�n de la c�mara
	float AngObj;			//Se usa para llevar control del �ngulo de rotaci�n para el modelo del objeto

	float CamaraPosAlt;		//Se usa para definir y cambiar si es necesario la altura de la c�mara
	float CamaraObjAlt;		//Se usa para definir y cambiar si es necesario la altura del objetivo de la c�mara

	float escalaX;
	float escalaY;
	float escalaZ;

	bool visible;
	bool caminando;
	bool saltando;
		
};

struct jerarquiaModelo
{
	float Angtx;	//Rot. en x
	float Angty;	//Rot. en y
	float Angcx;	//Rot. en x
	float Angcy;	//Rot. en y
	float Angbi1x;	//Rot. en X
	float Angbi1z;	//Rot. en Z
	float Angbi2x;  //Rot. en X
	float Angbi2z;  //Rot. en Z
	float Angbd1x;
	float Angbd1z;
	float Angbd2x;
	float Angbd2z;
	float Angpistoplay;
	float Angpizq1x;
	float Angpizq1z;
	float Angpizq2x;
	float Angpizq2z;
	float Angpder1x;
	float Angpder1z;
	float Angpder2x;
	float Angpder2z;
	float Angpi;	//Rot. en X
	float Angpd;	//Rot. en X

	float Xtor;
	float Ytor;
	float Ztor;
};

struct FRAME
{
	float Angtx;	 //Rot. en x
	float Angty;	 //Rot. en y
	float Angcx;	 //Rot. en x
	float Angcy;	 //Rot. en y
	float Angbi1x;	 //Rot. en X
	float Angbi1z;	 //Rot. en Z
	float Angbi2x;   //Rot. en X
	float Angbi2z;   //Rot. en Z
	float Angbd1x;
	float Angbd1z;
	float Angbd2x;
	float Angbd2z;
	float Angpizq1x;
	float Angpizq1z;
	float Angpizq2x;
	float Angpizq2z;
	float Angpder1x;
	float Angpder1z;
	float Angpder2x;
	float Angpder2z;
	float Angpi;	 //Rot. en X
	float Angpd;	 //Rot. en X

	float Xtor;
	float Ytor;
	float Ztor;

	float incAngtx;	 //Rot. en x
	float incAngty;	 //Rot. en y
	float incAngcx;	 //Rot. en x
	float incAngcy;	 //Rot. en y
	float incAngbi1x;//Rot. en X
	float incAngbi1z;//Rot. en Z
	float incAngbi2x;//Rot. en X
	float incAngbi2z;//Rot. en Z
	float incAngbd1x;
	float incAngbd1z;
	float incAngbd2x;
	float incAngbd2z;
	float incAngpizq1x;
	float incAngpizq1z;
	float incAngpizq2x;
	float incAngpizq2z;
	float incAngpder1x;
	float incAngpder1z;
	float incAngpder2x;
	float incAngpder2z;
	float incAngpi;	 //Rot. en X
	float incAngpd;	 //Rot. en X

	float incXtor;
	float incYtor;
	float incZtor;

};

struct varsAnim
{
	bool play;      //Bandera para iniciar la animaci�n
	int  playIndex; //Auxiliar para leer la informaci�n del contenedor de keyframes
	int  tipoAnim;  //Indicador del tipo de animaci�n
	int  loop;
};




#endif 