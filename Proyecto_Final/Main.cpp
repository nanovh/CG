#include "Main.h"
#include "3ds.h"

HDC			hDC=NULL;		// Dispositivo de contexto GDI
HGLRC		hRC=NULL;		// Contexto de renderizado
HWND		hWnd=NULL;		// Manejador de ventana
HINSTANCE	hInstance;		// Instancia de la aplicacion

bool	keys[256];			// Arreglo para el manejo de teclado
bool	active=TRUE;		// Bandera de ventana activa
float s=1;
int glWidth;
int glHeight;
float rota=0.0;
//Variable para cambiar entre render sólido y alambrado
int renderModo;

//variables para la cámara
CVector PosCam;
CVector ObjCam;

//Variables para iluminacion
GLfloat LightPos[] = {0.0f, 30.0f, 50.0f, 1.0f};		// Posición de la luz
GLfloat LightAmb[] = { 0.7f,  0.7f, 0.7f, 1.0f};		// Valores de la componente ambiente
GLfloat LightDif[] = { 1.0f,  1.0f, 1.0f, 1.0f};		// Valores de la componente difusa
GLfloat LightSpc[] = { 0.8f,  0.8f, 0.8f, 1.0f};		// Valores de la componente especular
CVector lightPosition;

//Apuntador para primitivas de cuadricas
GLUquadricObj	*e;

//Contenedor de texturas para el escenario
CTga textura[26];

//Nombre y ubicación de los modelos
#define FILE_NAME1p  "Modelos/torso2.3ds"
#define FILE_NAME2p  "Modelos/cabecilla.3ds"
#define FILE_NAME3p  "Modelos/pierna_a1.3ds"
#define FILE_NAME4p  "Modelos/pierna_b1.3ds"
#define FILE_NAME5p  "Modelos/pie.3ds"
#define FILE_NAME6p  "Modelos/brazo der_a1.3ds"
#define FILE_NAME7p  "Modelos/brazo der_b.3ds"
#define FILE_NAME8p  "Modelos/brazo izq_a1.3ds"
#define FILE_NAME9p  "Modelos/brazo izq_b.3ds"

//Contenedores de texturas de cada modelo
CTga textureModel1[10];
CTga textureModel2[10];
CTga textureModel3[10];
CTga textureModel4[10];
CTga textureModel5[10];
CTga textureModel6[10];
CTga textureModel7[10];
CTga textureModel8[10];
CTga textureModel9[10];
CTga textureModel10[10];

//Objeto que da acceso a las funciones del cargador 3ds
CLoad3DS g_Load3ds;

//Instancias de la estructura que almacenaran los datos de cada modelo
t3DModel g_3DModel1;
t3DModel g_3DModel2;
t3DModel g_3DModel3;
t3DModel g_3DModel4;
t3DModel g_3DModel5;
t3DModel g_3DModel6;
t3DModel g_3DModel7;
t3DModel g_3DModel8;
t3DModel g_3DModel9;
t3DModel g_3DModel10;

//Variables del personaje (para modelado jerárquico)
jerarquiaModelo player1modelo;	//Acceso a la estructura con las variables de cada pieza del modelo

bool disparando=false;

int estadoElevador=0; //0: abajo, 1: arriba, 2: subiendo, 3:bajando
float despElevador=0.0f;

//Objeto para acceder a las variables de control del personaje
paramObjCam player1;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaracion de WndProc (Procedimiento de ventana)

//Variables de animación de los personajes
const int maxKF1=3; //Num. total de KeyFrames para la secuencia 1 (caminar)
FRAME KeyFrame1p1[maxKF1]; //Contenedor para almacenar cada keyframe de la secuencia 1 del player 1
varsAnim varsAnimP1; //datos de para animacion del player 1


GLvoid ReDimensionaEscenaGL(GLsizei width, GLsizei height)	// Redimensiona e inicializa la ventana
{
	if (height==0)							// Para que no se presente una division por cero
	{
		height=1;							// la altura se iguala a 1
	}

	glViewport(0,0,width,height);					// Resetea el puerto de vista

	glMatrixMode(GL_PROJECTION);					// Selecciona la Matriz de Proyeccion
	glLoadIdentity();								// Resetea la Matriz de Proyeccion

	// Calcula el radio de aspecto o proporcion de medidas de la ventana
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,2000.0f);
	
	glMatrixMode(GL_MODELVIEW);							// Selecciona la Matriz de Vista de Modelo
	glLoadIdentity();									// Resetea la Matriz de Vista de Modelo

	glWidth=width;
	glHeight=height;
}

void CargaTexturas()
{
	textura[0].LoadTGA("Texturas/madera1.tga"); //madera para bancas
	textura[1].LoadTGA("Texturas/barandal.tga"); //madera para bancas
	textura[2].LoadTGA("Texturas/piso_madera.tga"); //madera para bancas
}

void DescargaTexturas()
{
	textura[0].Elimina();
}

int CargaModelos()
{

	if(!g_Load3ds.Load3DSFile(FILE_NAME1p, &g_3DModel1, textureModel1))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME2p, &g_3DModel2, textureModel2))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME3p, &g_3DModel3, textureModel3))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME4p, &g_3DModel4, textureModel4))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME5p, &g_3DModel5, textureModel9))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME6p, &g_3DModel6, textureModel6))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME7p, &g_3DModel7, textureModel9))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME8p, &g_3DModel8, textureModel8))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME9p, &g_3DModel9, textureModel9))
		return 0;
	
	return TRUE;
}

void DescargaModelos()
{
	g_Load3ds.UnLoad3DSFile(&g_3DModel1, textureModel1);
	g_Load3ds.UnLoad3DSFile(&g_3DModel2, textureModel2);
	g_Load3ds.UnLoad3DSFile(&g_3DModel3, textureModel3);
	g_Load3ds.UnLoad3DSFile(&g_3DModel4, textureModel4);
	g_Load3ds.UnLoad3DSFile(&g_3DModel5, textureModel5);
	g_Load3ds.UnLoad3DSFile(&g_3DModel6, textureModel6);
	g_Load3ds.UnLoad3DSFile(&g_3DModel7, textureModel7);
	g_Load3ds.UnLoad3DSFile(&g_3DModel8, textureModel8);
	g_Load3ds.UnLoad3DSFile(&g_3DModel9, textureModel9);
	g_Load3ds.UnLoad3DSFile(&g_3DModel10, textureModel10);
}

void inicializaCamara()
{
	PosCam=CVector(0.0f, 10.0f, 60.0f);
	ObjCam=CVector(0.0f, 12.0f, 0.0f);
}

void iniPersonaje(int iD)
{
	if(iD == 1)//player 1
	{
		player1.PosicionObj=CVector(0.0f,0.0f,12.0f);
		player1.VelocidadObj=0.5f;
		player1.AngObj=90.0f;

		player1.escalaX=2.1f;
		player1.escalaY=2.1f;
		player1.escalaZ=2.1f;
		player1.visible=true;
	}
}

void movimientoPersonaje(int dir)
{
	if(dir == 1) //derecha
	{
		player1.PosicionObj.x+=player1.VelocidadObj;
		PosCam.x+=player1.VelocidadObj;
		ObjCam.x+=player1.VelocidadObj;
		player1.AngObj=90.0f;
		
	}
	else if(dir == 2) //izquierda
	{
		player1.PosicionObj.x-=player1.VelocidadObj;
		PosCam.x-=player1.VelocidadObj;
		ObjCam.x-=player1.VelocidadObj;
		player1.AngObj=-90.0f;
	}
}

void InicializaAnim()
{
	//player 1
	//Se inicializan las variables de la secuencia 1
	for(int i=0; i<maxKF1; i++)
	{
		KeyFrame1p1[i].Angtx=0.0f;
		KeyFrame1p1[i].Angty=0.0f;
		KeyFrame1p1[i].Angcx=0.0f;
		KeyFrame1p1[i].Angcy=0.0f;
		KeyFrame1p1[i].Angbi1x=0.0f;
		KeyFrame1p1[i].Angbi1z=0.0f;
		KeyFrame1p1[i].Angbi2x=0.0f;
		KeyFrame1p1[i].Angbi2z=0.0f;
		KeyFrame1p1[i].Angbd1x=0.0f;
		KeyFrame1p1[i].Angbd1z=0.0f;
		KeyFrame1p1[i].Angbd2x=0.0f;
		KeyFrame1p1[i].Angbd2z=0.0f;
		KeyFrame1p1[i].Angpizq1x=0.0f;
		KeyFrame1p1[i].Angpizq1z=0.0f;
		KeyFrame1p1[i].Angpizq2x=0.0f;
		KeyFrame1p1[i].Angpizq2z=0.0f;
		KeyFrame1p1[i].Angpder1x=0.0f;
		KeyFrame1p1[i].Angpder1z=0.0f;
		KeyFrame1p1[i].Angpder2x=0.0f;
		KeyFrame1p1[i].Angpder2z=0.0f;
		KeyFrame1p1[i].Angpi=0.0f;
		KeyFrame1p1[i].Angpd=0.0f;
		KeyFrame1p1[i].Xtor=0.0f;
		KeyFrame1p1[i].Ytor=0.0f;
		KeyFrame1p1[i].Ztor=0.0f;

		KeyFrame1p1[i].incAngtx=0.0f;
		KeyFrame1p1[i].incAngty=0.0f;
		KeyFrame1p1[i].incAngcx=0.0f;
		KeyFrame1p1[i].incAngcy=0.0f;
		KeyFrame1p1[i].incAngbi1x=0.0f;
		KeyFrame1p1[i].incAngbi1z=0.0f;
		KeyFrame1p1[i].incAngbi2x=0.0f;
		KeyFrame1p1[i].incAngbi2z=0.0f;
		KeyFrame1p1[i].incAngbd1x=0.0f;
		KeyFrame1p1[i].incAngbd1z=0.0f;
		KeyFrame1p1[i].incAngbd2x=0.0f;
		KeyFrame1p1[i].incAngbd2z=0.0f;
		KeyFrame1p1[i].incAngpizq1x=0.0f;
		KeyFrame1p1[i].incAngpizq1z=0.0f;
		KeyFrame1p1[i].incAngpizq2x=0.0f;
		KeyFrame1p1[i].incAngpizq2z=0.0f;
		KeyFrame1p1[i].incAngpder1x=0.0f;
		KeyFrame1p1[i].incAngpder1z=0.0f;
		KeyFrame1p1[i].incAngpder2x=0.0f;
		KeyFrame1p1[i].incAngpder2z=0.0f;
		KeyFrame1p1[i].incAngpi=0.0f;
		KeyFrame1p1[i].incAngpd=0.0f;
		KeyFrame1p1[i].incXtor=0.0f;
		KeyFrame1p1[i].incYtor=0.0f;
		KeyFrame1p1[i].incZtor=0.0f;
	}

	varsAnimP1.play=false; //inicialmente sin animación (se activa al presionar la tecla para avanzar)
	varsAnimP1.playIndex=0;//inicia desde el primer keyframe
	varsAnimP1.tipoAnim=1;
	varsAnimP1.loop=0;     //sin repetición ya que se activa mediante el teclado
}

void DatosAnimacion()
{
	//player 1
	//Secuencia 1 (caminar)
	KeyFrame1p1[0].Angtx=0.0f;
	KeyFrame1p1[0].Angty=0.0f;
	KeyFrame1p1[0].Angcx=0.0f;
	KeyFrame1p1[0].Angcy=0.0f;
	KeyFrame1p1[0].Angbi1x=15.0f;
	KeyFrame1p1[0].Angbi1z=0.0f;
	KeyFrame1p1[0].Angbi2x=-10.0f;
	KeyFrame1p1[0].Angbi2z=0.0f;
	KeyFrame1p1[0].Angbd1x=-15.0f;
	KeyFrame1p1[0].Angbd1z=0.0f;
	KeyFrame1p1[0].Angbd2x=-10.0f;
	KeyFrame1p1[0].Angbd2z=0.0f;
	KeyFrame1p1[0].Angpizq1x=-27.0f;
	KeyFrame1p1[0].Angpizq1z=0.0f;
	KeyFrame1p1[0].Angpizq2x=23.0f;
	KeyFrame1p1[0].Angpizq2z=0.0f;
	KeyFrame1p1[0].Angpder1x=17.0f;
	KeyFrame1p1[0].Angpder1z=0.0f;
	KeyFrame1p1[0].Angpder2x=25.0f;
	KeyFrame1p1[0].Angpder2z=0.0f;
	KeyFrame1p1[0].Angpi=-27.0f;
	KeyFrame1p1[0].Angpd=17.0f;
	KeyFrame1p1[0].Xtor=0.0f;
	KeyFrame1p1[0].Ytor=0.0f;
	KeyFrame1p1[0].Ztor=0.0f;

	KeyFrame1p1[1].Angtx=0.0f;
	KeyFrame1p1[1].Angty=0.0f;
	KeyFrame1p1[1].Angcx=-10.0f;
	KeyFrame1p1[1].Angcy=0.0f;
	KeyFrame1p1[1].Angbi1x=-15.0f;
	KeyFrame1p1[1].Angbi1z=0.0f;
	KeyFrame1p1[1].Angbi2x=-10.0f;
	KeyFrame1p1[1].Angbi2z=0.0f;
	KeyFrame1p1[1].Angbd1x=15.0f;
	KeyFrame1p1[1].Angbd1z=0.0f;
	KeyFrame1p1[1].Angbd2x=-10.0f;
	KeyFrame1p1[1].Angbd2z=0.0f;
	KeyFrame1p1[1].Angpizq1x=17.0f;
	KeyFrame1p1[1].Angpizq1z=0.0f;
	KeyFrame1p1[1].Angpizq2x=25.0f;
	KeyFrame1p1[1].Angpizq2z=0.0f;
	KeyFrame1p1[1].Angpder1x=-23.0f;
	KeyFrame1p1[1].Angpder1z=0.0f;
	KeyFrame1p1[1].Angpder2x=27.0f;
	KeyFrame1p1[1].Angpder2z=0.0f;
	KeyFrame1p1[1].Angpi=17.0f;
	KeyFrame1p1[1].Angpd=-23.0f;
	KeyFrame1p1[1].Xtor=0.0f;
	KeyFrame1p1[1].Ytor=0.0f;
	KeyFrame1p1[1].Ztor=0.0f;

	KeyFrame1p1[2].Angtx=0.0f;
	KeyFrame1p1[2].Angty=0.0f;
	KeyFrame1p1[2].Angcx=0.0f;
	KeyFrame1p1[2].Angcy=0.0f;
	KeyFrame1p1[2].Angbi1x=15.0f;
	KeyFrame1p1[2].Angbi1z=0.0f;
	KeyFrame1p1[2].Angbi2x=-10.0f;
	KeyFrame1p1[2].Angbi2z=0.0f;
	KeyFrame1p1[2].Angbd1x=-15.0f;
	KeyFrame1p1[2].Angbd1z=0.0f;
	KeyFrame1p1[2].Angbd2x=-10.0f;
	KeyFrame1p1[2].Angbd2z=0.0f;
	KeyFrame1p1[2].Angpizq1x=-27.0f;
	KeyFrame1p1[2].Angpizq1z=0.0f;
	KeyFrame1p1[2].Angpizq2x=23.0f;
	KeyFrame1p1[2].Angpizq2z=0.0f;
	KeyFrame1p1[2].Angpder1x=17.0f;
	KeyFrame1p1[2].Angpder1z=0.0f;
	KeyFrame1p1[2].Angpder2x=25.0f;
	KeyFrame1p1[2].Angpder2z=0.0f;
	KeyFrame1p1[2].Angpi=-27.0f;
	KeyFrame1p1[2].Angpd=17.0f;
	KeyFrame1p1[2].Xtor=0.0f;
	KeyFrame1p1[2].Ytor=0.0f;
	KeyFrame1p1[2].Ztor=0.0f;
	
}

int IniGL(GLvoid)										// Aqui se configuran los parametros iniciales de OpenGL
{
	glShadeModel(GL_SMOOTH);							// Activa Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Fondo negro
	glClearDepth(1.0f);									// Valor para el Depth Buffer
	glEnable(GL_DEPTH_TEST);							// Activa Depth Testing
	glDepthFunc(GL_LEQUAL);								// Tipo de Depth Testing a usar
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Correccion de cálculos de perspectiva
	inicializaCamara();
	glCullFace(GL_BACK);								// Configurado para eliminar caras traseras
	glEnable(GL_CULL_FACE);								// Activa eliminacion de caras ocultas
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Posicion de la luz0
	glLightfv(GL_LIGHT0, GL_AMBIENT,  LightAmb);		// Componente ambiente
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  LightDif);		// Componente difusa
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Componente especular
	InicializaAnim();
	DatosAnimacion();
	glEnable(GL_LIGHT0);					// Activa luz0
	//glEnable(GL_LIGHTING);					// Habilita la iluminación
	//glDisable(GL_LIGHTING);
	iniPersonaje(1);
	e=gluNewQuadric();
	CargaTexturas();
	CargaModelos();
	return TRUE;										
}

void animacion(jerarquiaModelo *varsMod, FRAME *KeyFrame, int maxKF , int frames, varsAnim *anim)
{
	if(anim->play)
	{		
		if((fabs(KeyFrame[anim->playIndex+1].Angtx-varsMod->Angtx))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angty-varsMod->Angty))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angcx-varsMod->Angcx))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angcy-varsMod->Angcy))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbi1x-varsMod->Angbi1x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbi1z-varsMod->Angbi1z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbi2x-varsMod->Angbi2x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbi2z-varsMod->Angbi2z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbd1x-varsMod->Angbd1x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbd1z-varsMod->Angbd1z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbd2x-varsMod->Angbd2x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angbd2z-varsMod->Angbd2z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpizq1x-varsMod->Angpizq1x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpizq1z-varsMod->Angpizq1z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpizq2x-varsMod->Angpizq2x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpizq2z-varsMod->Angpizq2z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpder1x-varsMod->Angpder1x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpder1z-varsMod->Angpder1z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpder2x-varsMod->Angpder2x))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpder2z-varsMod->Angpder2z))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpi-varsMod->Angpi))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Angpd-varsMod->Angpd))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Xtor-varsMod->Xtor))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Ytor-varsMod->Ytor))<0.1f &&
		   (fabs(KeyFrame[anim->playIndex+1].Ztor-varsMod->Ztor))<0.1f)
		{			
			anim->playIndex++;			
			if(anim->playIndex > maxKF-2)
			{
				anim->playIndex=0;
				if(anim->loop == 0)
				{
					anim->play=false;
					anim->tipoAnim=0;
				}
				else
				{
					anim->play=true;
					varsMod->Angtx     = KeyFrame[anim->playIndex].Angtx;
					varsMod->Angty     = KeyFrame[anim->playIndex].Angty;
					varsMod->Angcx     = KeyFrame[anim->playIndex].Angcx;
					varsMod->Angcy     = KeyFrame[anim->playIndex].Angcy;
					varsMod->Angbi1x   = KeyFrame[anim->playIndex].Angbi1x;
					varsMod->Angbi1z   = KeyFrame[anim->playIndex].Angbi1z;
					varsMod->Angbi2x   = KeyFrame[anim->playIndex].Angbi2x;
					varsMod->Angbi2z   = KeyFrame[anim->playIndex].Angbi2z;
					varsMod->Angbd1x   = KeyFrame[anim->playIndex].Angbd1x;
					varsMod->Angbd1z   = KeyFrame[anim->playIndex].Angbd1z;
					varsMod->Angbd2x   = KeyFrame[anim->playIndex].Angbd2x;
					varsMod->Angbd2z   = KeyFrame[anim->playIndex].Angbd2z;
					varsMod->Angpizq1x = KeyFrame[anim->playIndex].Angpizq1x;
					varsMod->Angpizq1z = KeyFrame[anim->playIndex].Angpizq1z;
					varsMod->Angpizq2x = KeyFrame[anim->playIndex].Angpizq2x;
					varsMod->Angpizq2z = KeyFrame[anim->playIndex].Angpizq2z;
					varsMod->Angpder1x = KeyFrame[anim->playIndex].Angpder1x;
					varsMod->Angpder1z = KeyFrame[anim->playIndex].Angpder1z;
					varsMod->Angpder2x = KeyFrame[anim->playIndex].Angpder2x;
					varsMod->Angpder2z = KeyFrame[anim->playIndex].Angpder2z;
					varsMod->Angpi     = KeyFrame[anim->playIndex].Angpi;
					varsMod->Angpd     = KeyFrame[anim->playIndex].Angpd;
					varsMod->Xtor      = KeyFrame[anim->playIndex].Xtor;
					varsMod->Ytor      = KeyFrame[anim->playIndex].Ytor;
					varsMod->Ztor      = KeyFrame[anim->playIndex].Ztor;
				}
			}
		}
		else
		{

			KeyFrame[anim->playIndex].incAngtx		= (KeyFrame[anim->playIndex+1].Angtx-KeyFrame[anim->playIndex].Angtx)/frames;
			KeyFrame[anim->playIndex].incAngty		= (KeyFrame[anim->playIndex+1].Angty-KeyFrame[anim->playIndex].Angty)/frames;
			KeyFrame[anim->playIndex].incAngcx		= (KeyFrame[anim->playIndex+1].Angcx-KeyFrame[anim->playIndex].Angcx)/frames;
			KeyFrame[anim->playIndex].incAngcy		= (KeyFrame[anim->playIndex+1].Angcy-KeyFrame[anim->playIndex].Angcy)/frames;
			KeyFrame[anim->playIndex].incAngbi1x	= (KeyFrame[anim->playIndex+1].Angbi1x-KeyFrame[anim->playIndex].Angbi1x)/frames;
			KeyFrame[anim->playIndex].incAngbi1z	= (KeyFrame[anim->playIndex+1].Angbi1z-KeyFrame[anim->playIndex].Angbi1z)/frames;
			KeyFrame[anim->playIndex].incAngbi2x	= (KeyFrame[anim->playIndex+1].Angbi2x-KeyFrame[anim->playIndex].Angbi2x)/frames;
			KeyFrame[anim->playIndex].incAngbi2z	= (KeyFrame[anim->playIndex+1].Angbi2z-KeyFrame[anim->playIndex].Angbi2z)/frames;
			KeyFrame[anim->playIndex].incAngbd1x	= (KeyFrame[anim->playIndex+1].Angbd1x-KeyFrame[anim->playIndex].Angbd1x)/frames;
			KeyFrame[anim->playIndex].incAngbd1z	= (KeyFrame[anim->playIndex+1].Angbd1z-KeyFrame[anim->playIndex].Angbd1z)/frames;
			KeyFrame[anim->playIndex].incAngbd2x	= (KeyFrame[anim->playIndex+1].Angbd2x-KeyFrame[anim->playIndex].Angbd2x)/frames;
			KeyFrame[anim->playIndex].incAngbd2z	= (KeyFrame[anim->playIndex+1].Angbd2z-KeyFrame[anim->playIndex].Angbd2z)/frames;
			KeyFrame[anim->playIndex].incAngpizq1x	= (KeyFrame[anim->playIndex+1].Angpizq1x-KeyFrame[anim->playIndex].Angpizq1x)/frames;
			KeyFrame[anim->playIndex].incAngpizq1z	= (KeyFrame[anim->playIndex+1].Angpizq1z-KeyFrame[anim->playIndex].Angpizq1z)/frames;
			KeyFrame[anim->playIndex].incAngpizq2x	= (KeyFrame[anim->playIndex+1].Angpizq2x-KeyFrame[anim->playIndex].Angpizq2x)/frames;
			KeyFrame[anim->playIndex].incAngpizq2z	= (KeyFrame[anim->playIndex+1].Angpizq2z-KeyFrame[anim->playIndex].Angpizq2z)/frames;
			KeyFrame[anim->playIndex].incAngpder1x	= (KeyFrame[anim->playIndex+1].Angpder1x-KeyFrame[anim->playIndex].Angpder1x)/frames;
			KeyFrame[anim->playIndex].incAngpder1z	= (KeyFrame[anim->playIndex+1].Angpder1z-KeyFrame[anim->playIndex].Angpder1z)/frames;
			KeyFrame[anim->playIndex].incAngpder2x	= (KeyFrame[anim->playIndex+1].Angpder2x-KeyFrame[anim->playIndex].Angpder2x)/frames;
			KeyFrame[anim->playIndex].incAngpder2z	= (KeyFrame[anim->playIndex+1].Angpder2z-KeyFrame[anim->playIndex].Angpder2z)/frames;
			KeyFrame[anim->playIndex].incAngpi		= (KeyFrame[anim->playIndex+1].Angpi-KeyFrame[anim->playIndex].Angpi)/frames;
			KeyFrame[anim->playIndex].incAngpd		= (KeyFrame[anim->playIndex+1].Angpd-KeyFrame[anim->playIndex].Angpd)/frames;
			KeyFrame[anim->playIndex].incXtor		= (KeyFrame[anim->playIndex+1].Xtor-KeyFrame[anim->playIndex].Xtor)/frames;
			KeyFrame[anim->playIndex].incYtor		= (KeyFrame[anim->playIndex+1].Ytor-KeyFrame[anim->playIndex].Ytor)/frames;
			KeyFrame[anim->playIndex].incZtor		= (KeyFrame[anim->playIndex+1].Ztor-KeyFrame[anim->playIndex].Ztor)/frames;
			

			varsMod->Angtx     += KeyFrame[anim->playIndex].incAngtx;
			varsMod->Angty     += KeyFrame[anim->playIndex].incAngty;
			varsMod->Angcx     += KeyFrame[anim->playIndex].incAngcx;
			varsMod->Angcy     += KeyFrame[anim->playIndex].incAngcy;
			varsMod->Angbi1x   += KeyFrame[anim->playIndex].incAngbi1x;
			varsMod->Angbi1z   += KeyFrame[anim->playIndex].incAngbi1z;
			varsMod->Angbi2x   += KeyFrame[anim->playIndex].incAngbi2x;
			varsMod->Angbi2z   += KeyFrame[anim->playIndex].incAngbi2z;
			varsMod->Angbd1x   += KeyFrame[anim->playIndex].incAngbd1x;
			varsMod->Angbd1z   += KeyFrame[anim->playIndex].incAngbd1z;
			varsMod->Angbd2x   += KeyFrame[anim->playIndex].incAngbd2x;
			varsMod->Angbd2z   += KeyFrame[anim->playIndex].incAngbd2z;
			varsMod->Angpizq1x += KeyFrame[anim->playIndex].incAngpizq1x;
			varsMod->Angpizq1z += KeyFrame[anim->playIndex].incAngpizq1z;
			varsMod->Angpizq2x += KeyFrame[anim->playIndex].incAngpizq2x;
			varsMod->Angpizq2z += KeyFrame[anim->playIndex].incAngpizq2z;
			varsMod->Angpder1x += KeyFrame[anim->playIndex].incAngpder1x;
			varsMod->Angpder1z += KeyFrame[anim->playIndex].incAngpder1z;
			varsMod->Angpder2x += KeyFrame[anim->playIndex].incAngpder2x;
			varsMod->Angpder2z += KeyFrame[anim->playIndex].incAngpder2z;
			varsMod->Angpi     += KeyFrame[anim->playIndex].incAngpi;
			varsMod->Angpd     += KeyFrame[anim->playIndex].incAngpd;
			varsMod->Xtor      += KeyFrame[anim->playIndex].incXtor;
			varsMod->Ytor      += KeyFrame[anim->playIndex].incYtor;
			varsMod->Ztor      += KeyFrame[anim->playIndex].incZtor;
		
		}
	}
}

void DibujaEjes()
{
	glBegin(GL_LINES);
		//Eje X
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);

		//Eje Y
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f,  100.0f, 0.0f);

		//Eje Z
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f,  100.0f);
	glEnd();

	glPointSize(10.0f);

	glBegin(GL_POINTS);
		//"Flecha" eje X
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);

		//"Flecha" eje Y
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,  100.0f, 0.0f);

		//"Flecha" eje Z
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f, 0.0f,  100.0f);
	glEnd();

	glPointSize(1.0f);

	glColor3f(1.0f,1.0f,1.0f);
}

void dibujaCaja()
{
	glEnable(GL_TEXTURE_2D);
	//Plano 1
	glBegin(GL_QUADS);
		glColor3f(0.5f, 0.5f, 0.8f);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f);glVertex3f(-3.0f, 0.0f, 3.0f);
		glTexCoord2f(1.0f, 0.0f);glVertex3f( 3.0f, 0.0f, 3.0f);
		glTexCoord2f(1.0f, 1.0f);glVertex3f( 3.0f, 6.0f, 3.0f);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(-3.0f, 6.0f, 3.0f);
	glEnd();

	//Plano 2
	glBegin(GL_QUADS);
		glColor3f(0.7f, 0.7f, 1.0f);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(-3.0f, 6.0f,  3.0f);
		glVertex3f( 3.0f, 6.0f,  3.0f);
		glVertex3f( 3.0f, 6.0f, -3.0f);
		glVertex3f(-3.0f, 6.0f, -3.0f);
	glEnd();

	//Plano 3
	glBegin(GL_QUADS);
		glColor3f(0.4f, 0.4f, 0.7f);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f);glVertex3f(3.0f, 0.0f,  3.0f);
		glTexCoord2f(1.0f, 0.0f);glVertex3f(3.0f, 0.0f, -3.0f);
		glTexCoord2f(1.0f, 1.0f);glVertex3f(3.0f, 6.0f, -3.0f);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(3.0f, 6.0f,  3.0f);
	glEnd();

	//Plano 4
	glBegin(GL_QUADS);
		glColor3f(0.3f, 0.3f, 0.6f);
		glNormal3f(1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f);glVertex3f(-3.0f, 0.0f, -3.0f);
		glTexCoord2f(1.0f, 0.0f);glVertex3f(-3.0f, 0.0f,  3.0f);
		glTexCoord2f(1.0f, 1.0f);glVertex3f(-3.0f, 6.0f,  3.0f);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(-3.0f, 6.0f, -3.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_TEXTURE_2D);
}

void prisma (float altura, float largo, float profundidad, int text)  //Funcion creacion prisma
{
	GLfloat vertice [8][3] = {
				{0.5*largo ,-0.5*altura, 0.5*profundidad},    //Coordenadas Vértice 1 V1
				{-0.5*largo ,-0.5*altura , 0.5*profundidad},    //Coordenadas Vértice 2 V2
				{-0.5*largo ,-0.5*altura , -0.5*profundidad},    //Coordenadas Vértice 3 V3
				{0.5*largo ,-0.5*altura , -0.5*profundidad},    //Coordenadas Vértice 4 V4
				{0.5*largo ,0.5*altura , 0.5*profundidad},    //Coordenadas Vértice 5 V5
				{0.5*largo ,0.5*altura , -0.5*profundidad},    //Coordenadas Vértice 6 V6
				{-0.5*largo ,0.5*altura , -0.5*profundidad},    //Coordenadas Vértice 7 V7
				{-0.5*largo ,0.5*altura , 0.5*profundidad},    //Coordenadas Vértice 8 V8
				};

		glBindTexture(GL_TEXTURE_2D, textura[text].texID);
		glBegin(GL_POLYGON);	//Front
			//glColor3f(0.0,0.0,1.0);
			glNormal3f( 0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertice[0]);
			glTexCoord2f(0.0f, 1.0f); glVertex3fv(vertice[4]);
			glTexCoord2f(1.0f, 1.0f); glVertex3fv(vertice[7]);
			glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertice[1]);
		glEnd();

		glBegin(GL_POLYGON);	//Right
			//glColor3f(0.0,0.0,1.0);
			glNormal3f( 1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertice[0]);
			glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertice[3]);
			glTexCoord2f(1.0f, 1.0f); glVertex3fv(vertice[5]);
			glTexCoord2f(0.0f, 1.0f); glVertex3fv(vertice[4]);
		glEnd();

		glBegin(GL_POLYGON);	//Back
			//glColor3f(0.0,1.0,0.0);
			glNormal3f( 0.0f, 0.0f,-1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3fv(vertice[6]);
			glTexCoord2f(1.0f, 1.0f); glVertex3fv(vertice[5]);
			glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertice[3]);
			glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertice[2]);
		glEnd();

		glBegin(GL_POLYGON);  //Left
			//glColor3f(1.0,1.0,1.0);
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertice[1]);
			glTexCoord2f(1.0f, 1.0f); glVertex3fv(vertice[7]);
			glTexCoord2f(0.0f, 1.0f); glVertex3fv(vertice[6]);
			glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertice[2]);
		glEnd();

		glBegin(GL_POLYGON);  //Bottom
			//glColor3f(0.4,0.2,0.6);
			glNormal3f( 0.0f,-1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertice[0]);
			glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertice[1]);
			glTexCoord2f(1.0f, 1.0f); glVertex3fv(vertice[2]);
			glTexCoord2f(0.0f, 1.0f); glVertex3fv(vertice[3]);
		glEnd();

		glBegin(GL_POLYGON);  //Top
			//glColor3f(0.8,0.2,0.4);
			glNormal3f( 0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertice[4]);
			glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertice[5]);
			glTexCoord2f(1.0f, 1.0f); glVertex3fv(vertice[6]);
			glTexCoord2f(0.0f, 1.0f); glVertex3fv(vertice[7]);
		glEnd();
}

void silla1() {
	glPushMatrix();
		//-----------------------Asiento
		glPushMatrix();
			glTranslatef(0, 0, -10);
			glPushMatrix(); //--------Patas
				glTranslatef(3, -3, 3);
				//glRotatef(90, 0, 1, 0);
				prisma(6, 0.7, 0.7, 0);
			glPopMatrix();
			glPushMatrix(); //--------Patas
				glTranslatef(3, -3, -3);
				//glRotatef(90, 0, 1, 0);
				prisma(6, 0.7, 0.7, 0);
			glPopMatrix();
			glPushMatrix(); //--------Patas
				glTranslatef(-3, -3, -3);
				//glRotatef(90, 0, 1, 0);
				prisma(6, 0.7, 0.7, 0);
			glPopMatrix();
			glPushMatrix(); //--------Patas
				glTranslatef(-3, -3, 3);
				//glRotatef(90, 0, 1, 0);
				prisma(6, 0.7, 0.7, 0);
			glPopMatrix();
			glPushMatrix(); //--------Respaldo (poste)
				glTranslatef(-3, 3.5, -3);
				//glRotatef(90, 0, 1, 0);
				prisma(7, 0.7, 0.7, 0);
			glPopMatrix();
			glPushMatrix(); //--------Respaldo (poste)
				glTranslatef(-3, 3.5, 3);
				//glRotatef(90, 0, 1, 0);
				prisma(7, 0.7, 0.7, 0);
			glPopMatrix();
			glPushMatrix(); //--------Respaldo (medio)
				glTranslatef(-3, 6.0, 0);
				//glRotatef(90, 0, 1, 0);
				prisma(1, 0.7, 6.3, 0);
			glPopMatrix();
			glPushMatrix(); //--------Respaldo (medio)
				glTranslatef(-3, 4.5, 0);
				//glRotatef(90, 0, 1, 0);
				prisma(0.7, 0.7, 6.3, 0);
			glPopMatrix();
			glPushMatrix(); //--------Respaldo (medio)
				glTranslatef(-3, 3.5, 0);
				//glRotatef(90, 0, 1, 0);
				prisma(0.7, 0.7, 6.3, 0);
			glPopMatrix();
			glPushMatrix(); //--------Respaldo (medio)
				glTranslatef(-3, 2.5, 0);
				//glRotatef(90, 0, 1, 0);
				prisma(0.7, 0.7, 6.3, 0);
			glPopMatrix();
			glPushMatrix(); //--------Respaldo (medio)
				glTranslatef(-3, 1.5, 0);
				//glRotatef(90, 0, 1, 0);
				prisma(0.7, 0.7, 6.3, 0);
			glPopMatrix();
			prisma(1.0,7,7, 0);
		glPopMatrix();
	glPopMatrix();
}

void dibujaEscenario(int render)
{
	if(render == 1)//sólido
		glPolygonMode(GL_FRONT,GL_FILL);
	else if(render == 2)//alambrado
		glPolygonMode(GL_FRONT,GL_LINE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textura[2].texID);
	//OBJETO 15 (3 planos) ///////////////
	//Plano 1
	glBegin(GL_QUADS);
		glColor3f(0.5f, 0.5f, 0.4f);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f);glVertex3f( 0.0f, 0.0f, 24.0f);
		glTexCoord2f(10.0f, 0.0f);glVertex3f(250.0f, 0.0f, 24.0f);
		glTexCoord2f(10.0f, 1.0f);glVertex3f(250.0f, 2.0f, 24.0f);
		glTexCoord2f(0.0f, 1.0f);glVertex3f( 0.0f, 2.0f, 24.0f);
	glEnd();

	//Plano 2
	glBegin(GL_QUADS);
		glColor3f(0.6f, 0.6f, 0.5f);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f);glVertex3f( 0.0f, 2.0f, 24.0f);
		glTexCoord2f(10.0f, 0.0f);glVertex3f(250.0f, 2.0f, 24.0f);
		glTexCoord2f(10.0f, 1.0f);glVertex3f(250.0f, 2.0f,  0.0f);
		glTexCoord2f(0.0f, 1.0f);glVertex3f( 0.0f, 2.0f,  0.0f);
	glEnd();

	//Plano 3
	glBegin(GL_QUADS);
		glColor3f(0.4f, 0.4f, 0.4f);	
		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(250.0f,0.0f, 24.0f);
		glVertex3f( 0.0f, 0.0f, 24.0f);
		glVertex3f( 0.0f, 0.0f,  0.0f);
		glVertex3f(250.0f, 0.0f,  0.0f);
	glEnd();

	glPushMatrix();
	glTranslatef(10,10,10);
	glRotatef(-90,0,1,0);
	glScalef(0.8,0.5,2.4);
	silla1();
	glPopMatrix();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glBindTexture(GL_TEXTURE_2D, textura[1].texID);
	//Plano 3
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);glVertex3f(0.0f,0.0f, 24.0f);
		glTexCoord2f(1.0f, 0.0f);glVertex3f( 20.0f, 0.0f, 24.0f);
		glTexCoord2f(1.0f, 1.0f);glVertex3f( 20.0f, 10.0f,  24.0f);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(0.0f, 10.0f,  24.0f);
	glEnd();
	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
}

void dibujaPersonaje()
{
	glPushMatrix();//Torso
	    glRotatef(rota, 0.0f, 1.0f, 0.0f);
		glTranslatef(player1modelo.Xtor, player1modelo.Ytor, player1modelo.Ztor);
		glRotatef(player1modelo.Angty, 0.0f, 1.0f, 0.0f);
		glRotatef(player1modelo.Angtx, 1.0f, 0.0f, 0.0f);
		if(player1.saltando == true)
			glTranslatef(0.0f, -0.6f, 0.0f);

		//Cabeza
		glPushMatrix();
			glTranslatef(0.09f, 0.75f ,0.0f);
			glRotatef(player1modelo.Angcy, 0.0f, 1.0f, 0.0f);
			glRotatef(player1modelo.Angcx, 1.0f, 0.0f, 0.0f);
			g_Load3ds.Render3DSFile(&g_3DModel2, textureModel2, 1);
		glPopMatrix();

		
		glPushMatrix();//Pierna derecha_a
			glTranslatef(-0.28f, -0.5f ,0.0f);
			glRotatef(player1modelo.Angpder1x, 1.0f, 0.0f, 0.0f);
			glRotatef(player1modelo.Angpder1z, 0.0f, 0.0f, 1.0f);
			glPushMatrix();//Pierna derecha_b
				glTranslatef(0.0f, -0.9f ,0.0f);
				glRotatef(player1modelo.Angpder2x, 1.0f, 0.0f, 0.0f);
				glRotatef(player1modelo.Angpder2z, 0.0f, 0.0f, 1.0f);
				glPushMatrix();//Pie derecho
					glTranslatef(0.0f, -0.7f ,0.0f);
					glRotatef(player1modelo.Angpd, 0.0f, 0.0f, 1.0f);
					g_Load3ds.Render3DSFile(&g_3DModel5, textureModel5, 1);
				glPopMatrix();
				g_Load3ds.Render3DSFile(&g_3DModel4, textureModel4, 1);
			glPopMatrix();
			g_Load3ds.Render3DSFile(&g_3DModel3, textureModel3, 1);
		glPopMatrix();
				

		glPushMatrix();//Pierna izquierda_a
			glTranslatef(0.45f, -0.5f ,0.0f);
			glRotatef(player1modelo.Angpizq1x, 1.0f, 0.0f, 0.0f);
			glRotatef(player1modelo.Angpizq1z, 0.0f, 0.0f, 1.0f);
			glPushMatrix();//Pierna izquierda_b
				glTranslatef(0.0f, -0.9f ,0.0f);
				glRotatef(player1modelo.Angpizq2x, 1.0f, 0.0f, 0.0f);
				glRotatef(player1modelo.Angpizq2z, 0.0f, 0.0f, 1.0f);
				glPushMatrix();//Pie izquierdo
					glTranslatef(0.0f, -0.7f ,0.0f);
					glRotatef(player1modelo.Angpi, 0.0f, 0.0f, 1.0f);
					g_Load3ds.Render3DSFile(&g_3DModel5, textureModel5, 1);
				glPopMatrix();
				g_Load3ds.Render3DSFile(&g_3DModel4, textureModel4, 1);
			glPopMatrix();
			g_Load3ds.Render3DSFile(&g_3DModel3, textureModel3, 1);
		glPopMatrix();
				
		glPushMatrix();//Brazo derecho_a
			glTranslatef(-0.58f, 1.2f ,0.0f);
			glRotatef(player1modelo.Angbd1x, 1.0f, 0.0f, 0.0f);
			glRotatef(player1modelo.Angbd1z, 0.0f, 0.0f, 1.0f);
			glPushMatrix();//Brazo derecho_b
				glTranslatef(-0.43f, -0.68f ,0.0f);
				glRotatef(player1modelo.Angbd2x, 1.0f, 0.0f, 0.0f);
				glRotatef(player1modelo.Angbd2z, 0.0f, 0.0f, 1.0f);
				g_Load3ds.Render3DSFile(&g_3DModel7, textureModel7, 1);
			glPopMatrix();
			g_Load3ds.Render3DSFile(&g_3DModel6, textureModel6, 1);
		glPopMatrix();
		
		glPushMatrix();//Brazo izquierdo_a
			glTranslatef(1.2f, 1.2f ,-0.2f);
			glRotatef(player1modelo.Angbi1x, 1.0f, 0.0f, 0.0f);
			glRotatef(player1modelo.Angbi1z, 0.0f, 0.0f, 1.0f);
			glPushMatrix();//Brazo izquierdo_b
				glTranslatef(-0.1f, -0.6f ,0.08f);
				glRotatef(player1modelo.Angbi2x, 1.0f, 0.0f, 0.0f);
				glRotatef(player1modelo.Angbi2z, 0.0f, 0.0f, 1.0f);
				g_Load3ds.Render3DSFile(&g_3DModel9, textureModel9, 1);
			glPopMatrix();
			g_Load3ds.Render3DSFile(&g_3DModel8, textureModel8, 1);
		glPopMatrix();
		g_Load3ds.Render3DSFile(&g_3DModel1, textureModel1, 1);
	glPopMatrix();
}

void DibujaLuz(CVector l)
{
	//Dibuja una esfera que representa la fuente luminosa
	glDisable(GL_LIGHTING);				// Deshabilita iluminación
	
	glPushMatrix();
		glTranslatef(l.x, l.y, l.z);	// Traslada a la posicion de la luz

		glColor3f(1.0f, 1.0f, 0.0f);	// Color amarillo
		gluSphere(e, 1.0f, 16, 8);		// Dibuja la esfera
	glPopMatrix();


	glEnable(GL_LIGHTING);				// Habilita Iluminación
	glColor3f(1.0f, 1.0f, 1.0f);
}

void ActualizaLuzParam()
{
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Posicion de la luz0
	glLightfv(GL_LIGHT0, GL_AMBIENT,  LightAmb);		// Componente ambiente
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  LightDif);		// Componente difusa
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Componente especular

	lightPosition.x=LightPos[0];
	lightPosition.y=LightPos[1];
	lightPosition.z=LightPos[2];
}

int RenderizaEscena(GLvoid)								// Aqui se dibuja todo lo que aparecera en la ventana
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int iNumViewPort=0; iNumViewPort<2; iNumViewPort++)
	{
		if(iNumViewPort == 0)//Se crea el puerto de vista 1 (Escenario)
		{
			glViewport(0,glHeight*0.25f,glWidth,glHeight*0.75f);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45.0f,(GLfloat)glWidth/(GLfloat)(glHeight*0.75f),1.0f,2000.0f);
		}
		if(iNumViewPort == 1)//Se crea el puerto de vista 2 (Indicadores de estado del personaje)
		{
			glViewport(0,0,glWidth,glHeight*0.25f);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45.0f,(GLfloat)glWidth/(GLfloat)(glHeight*0.25f),1.0f,2000.0f);
		}
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClear(GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
				
		if(iNumViewPort == 0) //Se dibuja en el puerto de vista 1
		{
			gluLookAt(PosCam.x, PosCam.y, PosCam.z, ObjCam.x, ObjCam.y, ObjCam.z, 0, 1, 0);
			ActualizaLuzParam();
			DibujaLuz(lightPosition);
			if(player1.visible)
			{
				if(player1.PosicionObj.x > 0.0f && player1.PosicionObj.x < 20.0f
					&& player1.PosicionObj.y >= 40.0f && player1.PosicionObj.y <= 80.0f)
					player1.PosicionObj.y=40.0f+despElevador;
				
				glPushMatrix();
					glTranslatef(player1.PosicionObj.x,player1.PosicionObj.y+3.0f,player1.PosicionObj.z);
					glRotatef(player1.AngObj,0.0f,1.0f,0.0f);
					glScalef(player1.escalaX,player1.escalaY,player1.escalaZ);
					dibujaPersonaje();
				glPopMatrix();

			}
			DibujaEjes();
			dibujaEscenario(renderModo);
		}

		if(iNumViewPort == 1) //Se dibuja en el puerto de vista 2
		{

		}
	}

	return TRUE;
}

GLvoid DestruyeVentanaOGL(GLvoid)						// Elimina la ventana apropiadamente
{
	if (hRC)											// Si existe un contexto de renderizado...
	{
		if (!wglMakeCurrent(NULL,NULL))					// Si no se pueden liberar los contextos DC y RC...
		{
			MessageBox(NULL,"Falla al liberar DC y RC.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Si no se puede eliminar el RC?
		{
			MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Se pone RC en NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Si no se puede eliminar el DC
	{
		MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Se pone DC en NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Si no se puede destruir la ventana
	{
		MessageBox(NULL,"No se pudo liberar hWnd.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Se pone hWnd en NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Si no se puede eliminar el registro de la clase
	{
		MessageBox(NULL,"No se pudo eliminar el registro de la clase.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Se pone hInstance en NULL
	}
}

//	Este código crea la ventana de OpenGL.  Parámetros:					
//	title			- Titulo en la parte superior de la ventana			
//	width			- Ancho de la ventana								
//	height			- Alto de la ventana								
//	bits			- Número de bits a usar para el color (8/16/24/32)	
  
BOOL CreaVentanaOGL(char* title, int width, int height, int bits)
{
	GLuint	PixelFormat;				// Guarda el resultado despues de determinar el formato a usar
	WNDCLASS	wc;						// Estructura de la clase ventana
	DWORD		dwExStyle;				// Estilo extendido de ventana
	DWORD		dwStyle;				// Estilo de ventana
	RECT		WindowRect;				// Guarda los valores Superior Izquierdo / Inferior Derecho del rectángulo
	WindowRect.left=(long)0;			// Inicia el valor Izquierdo a 0
	WindowRect.right=(long)width;		// Inicia el valor Derecho al ancho especificado
	WindowRect.top=(long)0;				// Inicia el valor Superior a 0
	WindowRect.bottom=(long)height;		// Inicia el valor Inferior al alto especificado

	hInstance			= GetModuleHandle(NULL);				// Guarda una instancia de la ventana
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redibuja el contenido de la ventana al redimensionarla
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// Maneja los mensajes para WndProc
	wc.cbClsExtra		= 0;									// Ningun dato extra para la clase
	wc.cbWndExtra		= 0;									// Ningun dato extra para la ventana
	wc.hInstance		= hInstance;							// Inicia la instancia
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Carga el ícono por defecto
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Carga el puntero de flecha
	wc.hbrBackground	= NULL;									// No se requiere ningun fondo
	wc.lpszMenuName		= NULL;									// No hay menú en la ventana
	wc.lpszClassName	= "OpenGL";								// Fija el nombre de la clase.

	if (!RegisterClass(&wc))									// Intenta registrar la clase de ventana
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											
	}
		
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;					// Estilo extendido de ventana
	dwStyle=WS_OVERLAPPEDWINDOW;									// Estilo de ventana

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Ajusta la ventana al tamaño especificado

	// Crea la ventana
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Estilo extendido para la ventana
								"OpenGL",							// Nombre de la clase
								title,								// Título de la ventana
								dwStyle |							// Definición del estilo de la ventana
								WS_CLIPSIBLINGS |					// Estilo requerido de la ventana
								WS_CLIPCHILDREN,					// Estilo requerido de la ventana
								0, 0,								// Posición de la ventana
								WindowRect.right-WindowRect.left,	// Calcula el ancho de la ventana
								WindowRect.bottom-WindowRect.top,	// Calcula el alto de la ventana
								NULL,								// No hay ventana superior
								NULL,								// No hay menú
								hInstance,							// Instancia
								NULL)))								// No se pasa nada a WM_CREATE
	{
		DestruyeVentanaOGL();										// Resetea el despliegue
		MessageBox(NULL,"Error al crear la ventana.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Si no se creo el contexto de dispositivo...
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede crear un contexto de dispositivo GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Si Windows no encontró un formato de pixel compatible
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede encontrar un formato de pixel compatible.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Si no se pudo habilitar el formato de pixel
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede usar el formato de pixel.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(hRC=wglCreateContext(hDC)))				// Si no se creo el contexto de renderizado
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede crear un contexto de renderizado GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!wglMakeCurrent(hDC,hRC))					// Si no se puede activar el contexto de renderizado
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede usar el contexto de renderizado GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	ShowWindow(hWnd,SW_SHOW);				// Muestra la ventana
	SetForegroundWindow(hWnd);				// Le da la prioridad mas alta
	SetFocus(hWnd);							// Pasa el foco del teclado a la ventana
	ReDimensionaEscenaGL(width, height);	// Inicia la perspectiva para la ventana OGL

	if (!IniGL())							// Si no se inicializa la ventana creada
	{
		DestruyeVentanaOGL();				// Resetea el despliegue
		MessageBox(NULL,"Falla en la inicialización.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	return TRUE;							// Todo correcto
}

LRESULT CALLBACK WndProc(	HWND	hWnd,	// Manejador para esta ventana
							UINT	uMsg,	// Mensaje para esta ventana
							WPARAM	wParam,	// Información adicional del mensaje
							LPARAM	lParam)	// Información adicional del mensaje
{
	switch (uMsg)							// Revisa los mensajes de la ventana
	{
		case WM_ACTIVATE:					// Revisa el mensaje de activación de ventana
		{
			if (!HIWORD(wParam))			// Revisa el estado de minimización
			{
				active=TRUE;				// El programa está activo
			}
			else
			{
				active=FALSE;				// El programa no está activo
			}

			return 0;						// Regresa al ciclo de mensajes
		}

		case WM_SYSCOMMAND:					// Intercepta comandos del sistema
		{
			switch (wParam)					// Revisa llamadas del sistema
			{
				case SC_SCREENSAVE:			// ¿Screensaver tratando de iniciar?
				case SC_MONITORPOWER:		// ¿Monitor tratando de entrar a modo de ahorro de energía?
				return 0;					// Evita que suceda
			}
			break;							// Sale del caso
		}

		case WM_CLOSE:						// Si se recibe un mensaje de cerrar...
		{
			PostQuitMessage(0);				// Se manda el mensaje de salida
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYDOWN:					// Si se está presionando una tecla...
		{
			keys[wParam] = TRUE;			// Si es así, se marca como TRUE
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYUP:						// ¿Se ha soltado una tecla?
		{
			keys[wParam] = FALSE;			// Si es así, se marca como FALSE
			return 0;						// y se regresa al ciclo
		}

		case WM_SIZE:						// Si se redimensiona la ventana...
		{
			ReDimensionaEscenaGL(LOWORD(lParam),HIWORD(lParam));  	// LoWord=Width, HiWord=Height
			return 0;						// y se regresa al ciclo
		}
	}

	// Pasa todos los mensajes no considerados a DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

// Este es el punto de entrada al programa; la función principal 
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instancia
					HINSTANCE	hPrevInstance,		// Instancia previa
					LPSTR		lpCmdLine,			// Parametros de la linea de comandos
					int			nCmdShow)			// Muestra el estado de la ventana
{
	MSG		msg;									// Estructura de mensajes de la ventana
	BOOL	done=FALSE;								// Variable booleana para salir del ciclo

	// Crea la ventana OpenGL
	if (!CreaVentanaOGL("Proyecto de Computación Gráfica",640,480,16))
	{
		return 0;									// Salir del programa si la ventana no fue creada
	}

	while(!done)									// Mientras done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Revisa si hay mensajes en espera
		{
			if (msg.message==WM_QUIT)				// Si se ha recibido el mensje de salir...
			{
				done=TRUE;							// Entonces done=TRUE
			}
			else									// Si no, Procesa los mensajes de la ventana
			{
				TranslateMessage(&msg);				// Traduce el mensaje
				DispatchMessage(&msg);				// Envia el mensaje
			}
		}
		else										// Si no hay mensajes...
		{
			// Dibuja la escena. 
			if (active)								// Si está activo el programa...
			{
				if (keys[VK_ESCAPE])				// Si se ha presionado ESC
				{
					done=TRUE;						// ESC indica el termino del programa
				}
				else								// De lo contrario, actualiza la pantalla
				{
					RenderizaEscena();				// Dibuja la escena
					SwapBuffers(hDC);				// Intercambia los Buffers (Double Buffering)
					if(varsAnimP1.play)
	{
		if(varsAnimP1.tipoAnim == 1) //caminar
			animacion(&player1modelo, KeyFrame1p1, maxKF1 , 15, &varsAnimP1);
	}
				}

				if(!ManejaTeclado()) return 0;
			}
			
		}
	}

	// Finalización del programa
	DescargaTexturas();
	DescargaModelos();
	DestruyeVentanaOGL();							// Destruye la ventana
	return (msg.wParam);							// Sale del programa
}

int ManejaTeclado()
{
	if(GetKeyState(VK_UP) & 0x80) //Si está presionada la tecla UP
	{
		PosCam.y+=0.5f;
		ObjCam.y+=0.5f;
		player1.PosicionObj.y+=0.5f;
	}

	if(GetKeyState('L') & 0x80) //Si está presionada la tecla DOWN
	{
		PosCam.y-=0.5f;
		ObjCam.y-=0.5f;
		player1.PosicionObj.y-=0.5f;
	}

	if(GetKeyState(VK_LEFT) & 0x80) //Si está presionada la tecla LEFT
	{
		movimientoPersonaje(2);
		player1.caminando=true;
		
		if(varsAnimP1.tipoAnim != 2 && varsAnimP1.play==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
		{
			if(player1.disparando == false)
			{
				//Se le asignan a las variables del personaje los 
				//valores almacenados en el primer keyframe para que
				//inicie desde ahí la animación.
				player1modelo.Angtx     = KeyFrame1p1[0].Angtx;
				player1modelo.Angty     = KeyFrame1p1[0].Angty;
				player1modelo.Angcx     = KeyFrame1p1[0].Angcx;
				player1modelo.Angcy     = KeyFrame1p1[0].Angcy;
				player1modelo.Angbi1x   = KeyFrame1p1[0].Angbi1x;
				player1modelo.Angbi1z   = KeyFrame1p1[0].Angbi1z;
				player1modelo.Angbi2x   = KeyFrame1p1[0].Angbi2x;
				player1modelo.Angbi2z   = KeyFrame1p1[0].Angbi2z;
				player1modelo.Angbd1x   = KeyFrame1p1[0].Angbd1x;
				player1modelo.Angbd1z   = KeyFrame1p1[0].Angbd1z;
				player1modelo.Angbd2x   = KeyFrame1p1[0].Angbd2x;
				player1modelo.Angbd2z   = KeyFrame1p1[0].Angbd2z;
				player1modelo.Angpizq1x = KeyFrame1p1[0].Angpizq1x;
				player1modelo.Angpizq1z = KeyFrame1p1[0].Angpizq1z;
				player1modelo.Angpizq2x = KeyFrame1p1[0].Angpizq2x;
				player1modelo.Angpizq2z = KeyFrame1p1[0].Angpizq2z;
				player1modelo.Angpder1x = KeyFrame1p1[0].Angpder1x;
				player1modelo.Angpder1z = KeyFrame1p1[0].Angpder1z;
				player1modelo.Angpder2x = KeyFrame1p1[0].Angpder2x;
				player1modelo.Angpder2z = KeyFrame1p1[0].Angpder2z;
				player1modelo.Angpi     = KeyFrame1p1[0].Angpi;
				player1modelo.Angpd     = KeyFrame1p1[0].Angpd;
				player1modelo.Xtor      = KeyFrame1p1[0].Xtor;
				player1modelo.Ytor      = KeyFrame1p1[0].Ytor;
				player1modelo.Ztor      = KeyFrame1p1[0].Ztor;

				varsAnimP1.play=true;
				varsAnimP1.playIndex=0;
				varsAnimP1.tipoAnim=1;
				varsAnimP1.loop=0;
			}
		}
	}

	if(GetKeyState(VK_RIGHT) & 0x80) //Si está presionada la tecla RIGHT
	{
		movimientoPersonaje(1);
		player1.caminando=true;
			
		if(varsAnimP1.tipoAnim != 2 && varsAnimP1.play==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
		{
			if(player1.disparando == false)
			{
				//Se le asignan a las variables del personaje los 
				//valores almacenados en el primer keyframe para que
				//inicie desde ahí la animación.
				player1modelo.Angtx     = KeyFrame1p1[0].Angtx;
				player1modelo.Angty     = KeyFrame1p1[0].Angty;
				player1modelo.Angcx     = KeyFrame1p1[0].Angcx;
				player1modelo.Angcy     = KeyFrame1p1[0].Angcy;
				player1modelo.Angbi1x   = KeyFrame1p1[0].Angbi1x;
				player1modelo.Angbi1z   = KeyFrame1p1[0].Angbi1z;
				player1modelo.Angbi2x   = KeyFrame1p1[0].Angbi2x;
				player1modelo.Angbi2z   = KeyFrame1p1[0].Angbi2z;
				player1modelo.Angbd1x   = KeyFrame1p1[0].Angbd1x;
				player1modelo.Angbd1z   = KeyFrame1p1[0].Angbd1z;
				player1modelo.Angbd2x   = KeyFrame1p1[0].Angbd2x;
				player1modelo.Angbd2z   = KeyFrame1p1[0].Angbd2z;
				player1modelo.Angpizq1x = KeyFrame1p1[0].Angpizq1x;
				player1modelo.Angpizq1z = KeyFrame1p1[0].Angpizq1z;
				player1modelo.Angpizq2x = KeyFrame1p1[0].Angpizq2x;
				player1modelo.Angpizq2z = KeyFrame1p1[0].Angpizq2z;
				player1modelo.Angpder1x = KeyFrame1p1[0].Angpder1x;
				player1modelo.Angpder1z = KeyFrame1p1[0].Angpder1z;
				player1modelo.Angpder2x = KeyFrame1p1[0].Angpder2x;
				player1modelo.Angpder2z = KeyFrame1p1[0].Angpder2z;
				player1modelo.Angpi     = KeyFrame1p1[0].Angpi;
				player1modelo.Angpd     = KeyFrame1p1[0].Angpd;
				player1modelo.Xtor      = KeyFrame1p1[0].Xtor;
				player1modelo.Ytor      = KeyFrame1p1[0].Ytor;
				player1modelo.Ztor      = KeyFrame1p1[0].Ztor;

				varsAnimP1.play=true;
				varsAnimP1.playIndex=0;
				varsAnimP1.tipoAnim=1;
				varsAnimP1.loop=0;
			}
		}
	}

	if(keys['X'])
	{
		if(varsAnimP1.play == false)
		{
		player1modelo.Angtx     = 0.0f;
		player1modelo.Angty     = 0.0f;
		player1modelo.Angcx     = 0.0f;
		player1modelo.Angcy     = -87.0f;
		player1modelo.Angbi1x   = 30.0f;
		player1modelo.Angbi1z   = 30.0f;
		player1modelo.Angbi2x   = -180.0f;
		player1modelo.Angbi2z   = 0.0f;
		player1modelo.Angbd1x   = 0.0f;
		player1modelo.Angbd1z   = -80.0f;
		player1modelo.Angbd2x   = -5.0f;
		player1modelo.Angbd2z   = 0.0f;
		player1modelo.Angpizq1x = 0.0f;
		player1modelo.Angpizq1z = 0.0f;
		player1modelo.Angpizq2x = 0.0f;
		player1modelo.Angpizq2z = 0.0f;
		player1modelo.Angpder1x = 0.0f;
		player1modelo.Angpder1z = 0.0f;
		player1modelo.Angpder2x = 0.0f;
		player1modelo.Angpder2z = 0.0f;
		player1modelo.Angpi     = 0.0f;
		player1modelo.Angpd     = 0.0f;
		player1modelo.Xtor      = 0.0f;
		player1modelo.Ytor      = 0.0f;
		player1modelo.Ztor      = 0.0f;

			disparando=true;
		}
	}
	else
	{
		if(varsAnimP1.play == false)
		{
			player1modelo.Angtx     = 0.0f;
			player1modelo.Angty     = 0.0f;
			player1modelo.Angcx     = 0.0f;
			player1modelo.Angcy     = 0.0f;
			player1modelo.Angbi1x   = 0.0f;
			player1modelo.Angbi1z   = 0.0f;
			player1modelo.Angbi2x   = 0.0f;
			player1modelo.Angbi2z   = 0.0f;
			player1modelo.Angbd1x   = 0.0f;
			player1modelo.Angbd1z   = 0.0f;
			player1modelo.Angbd2x   = 0.0f;
			player1modelo.Angbd2z   = 0.0f;
			player1modelo.Angpizq1x = 0.0f;
			player1modelo.Angpizq1z = 0.0f;
			player1modelo.Angpizq2x = 0.0f;
			player1modelo.Angpizq2z = 0.0f;
			player1modelo.Angpder1x = 0.0f;
			player1modelo.Angpder1z = 0.0f;
			player1modelo.Angpder2x = 0.0f;
			player1modelo.Angpder2z = 0.0f;
			player1modelo.Angpi     = 0.0f;
			player1modelo.Angpd     = 0.0f;
			player1modelo.Xtor      = 0.0f;
			player1modelo.Ytor      = 0.0f;
			player1modelo.Ztor      = 0.0f;

			disparando=false;
		}
	}

	//Para que al soltar la tecla presionada el personaje no quede en una posición
	//intermedia de la animación se asignan los valores originales a las variables
	if(!(GetKeyState(VK_UP) & 0x80 || GetKeyState(VK_DOWN) & 0x80
	|| GetKeyState(VK_LEFT) & 0x80 || GetKeyState(VK_RIGHT) & 0x80)) //Si no está presionada alguna de esas teclas
	{
		if(varsAnimP1.tipoAnim == 1)
		{
			varsAnimP1.play=false;
			player1modelo.Angtx     = 0.0f;
			player1modelo.Angty     = 0.0f;
			player1modelo.Angcx     = 0.0f;
			player1modelo.Angcy     = 0.0f;
			player1modelo.Angbi1x   = 0.0f;
			player1modelo.Angbi1z   = 0.0f;
			player1modelo.Angbi2x   = 0.0f;
			player1modelo.Angbi2z   = 0.0f;
			player1modelo.Angbd1x   = 0.0f;
			player1modelo.Angbd1z   = 0.0f;
			player1modelo.Angbd2x   = 0.0f;
			player1modelo.Angbd2z   = 0.0f;
			player1modelo.Angpizq1x = 0.0f;
			player1modelo.Angpizq1z = 0.0f;
			player1modelo.Angpizq2x = 0.0f;
			player1modelo.Angpizq2z = 0.0f;
			player1modelo.Angpder1x = 0.0f;
			player1modelo.Angpder1z = 0.0f;
			player1modelo.Angpder2x = 0.0f;
			player1modelo.Angpder2z = 0.0f;
			player1modelo.Angpi     = 0.0f;
			player1modelo.Angpd     = 0.0f;
			player1modelo.Xtor      = 0.0f;
			player1modelo.Ytor      = 0.0f;
			player1modelo.Ztor      = 0.0f;

			varsAnimP1.tipoAnim=0;
		}

		player1.caminando=false;

	}

	//Controles de la iluminación
	if (keys['Q'])
		LightPos[0] += 1.0f; //Hacia la derecha

	if (keys['W'])
		LightPos[0] -= 1.0f; //Hacia la izquierda

	if (keys['E'])
		LightPos[1] += 1.0f; //Hacia arriba

	if (keys['R'])
		LightPos[1] -= 1.0f; //Hacia abajo

	if (keys['T'])
		LightPos[2] += 1.0f; //Hacia adelante

	if (keys['Y'])
		LightPos[2] -= 1.0f; //Hacia atrás

	if (keys['H'])
		rota += 1.0f; //Hacia la derecha

	if (keys['1']) //Aumenta la componente ambiente
	{
		if(LightAmb[0] < 1.0f)
		{
			LightAmb[0]+=0.1f;
			LightAmb[1]+=0.1f;
			LightAmb[2]+=0.1f;
		}
	}
	
	if (keys['2']) //Disminuye la componente ambiente
	{
		if(LightAmb[0] > 0.0f)
		{
			LightAmb[0]-=0.1f;
			LightAmb[1]-=0.1f;
			LightAmb[2]-=0.1f;
		}
	}

	if (keys['3']) //Aumenta la componente difusa
	{
		if(LightDif[0] < 1.0f)
		{
			LightDif[0]+=0.1f;
			LightDif[1]+=0.1f;
			LightDif[2]+=0.1f;
		}
	}

	if (keys['4']) //Disminuye la componente difusa
	{
		if(LightDif[0] > 0.0f)
		{
			LightDif[0]-=0.1f;
			LightDif[1]-=0.1f;
			LightDif[2]-=0.1f;
		}
	}

	if (keys['5']) //Aumenta la componente especular
	{
		if(LightSpc[0] < 1.0f)
		{
			LightSpc[0]+=0.1f;
			LightSpc[1]+=0.1f;
			LightSpc[2]+=0.1f;
		}
	}

	if (keys['6']) //Disminuye la componente especular
	{
		if(LightSpc[0] > 0.0f)
		{
			LightSpc[0]-=0.1f;
			LightSpc[1]-=0.1f;
			LightSpc[2]-=0.1f;
		}
	}

	if(keys[VK_PRIOR])
	{
		if(estadoElevador == 0)
			estadoElevador=2;
	}
	if(keys[VK_NEXT])
	{
		if(estadoElevador == 1)
			estadoElevador=3;
	}

	if(keys[VK_HOME])
	{
		
	}

	if(keys[VK_END])
	{
		
	}

	if(keys['A'])
	{
		renderModo=1;
	}
	if(keys['S'])
	{
		renderModo=2;
	}

	return TRUE;
}
