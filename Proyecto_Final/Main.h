#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>		// Archivo de cabecera para Windows
#include <math.h>			// Archivo de cabecera para Funciones Matemáticas
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
	CVector PosicionObj;	//La posición (x,y,z) del objeto
	CVector PosicionObjAnt;	//La posición anterior (x,y,z) del objeto
	CVector Direccion;		//La dirección en que se dirige el objeto en forma de vector=(x,y,z)
	CVector DireccionCam;   //La dirección que define la posición de la cámara respecto al personaje
	CVector PosicionCam;    //La posición de la cámara que sigue al objeto
	CVector ObjetivoCam;	//El punto (x,y,z) que está viendo la cámara; en este caso al objeto

	float VelocidadObj;		//La velocidad a la que se mueve el objeto
	float DistanciaCam;		//La distancia que la cámara está separada del objeto
	float AngDir;			//Se usa para llevar control del angulo para el cálculo del vector de dirección
	float AngDirCam;		//Se usa para llevar control del angulo para el cálculo de posición de la cámara
	float AngObj;			//Se usa para llevar control del ángulo de rotación para el modelo del objeto

	float CamaraPosAlt;		//Se usa para definir y cambiar si es necesario la altura de la cámara
	float CamaraObjAlt;		//Se usa para definir y cambiar si es necesario la altura del objetivo de la cámara

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
	bool play;      //Bandera para iniciar la animación
	int  playIndex; //Auxiliar para leer la información del contenedor de keyframes
	int  tipoAnim;  //Indicador del tipo de animación
	int  loop;
};




#endif 