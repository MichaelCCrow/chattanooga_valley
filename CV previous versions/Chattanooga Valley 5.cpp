
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#ifdef __APPLE__
//#  include <GL/glew.h>
#include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
//#  include <GL/glew.h>
#include <GLUT/glut.h>
#  include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 
#endif

#include "getbmp.h"

#define PI 3.14159265358979324

using namespace std;

// Globals.
static unsigned int texture[3]; // Array of texture indices.
short data[1201][1201];
short data2[200][200];

// Movement/Viewpoint Globals
static float my_x=0;
static float my_y=500;
static float my_z=600;
static float my_angle=0;
static float myY_angle=0;

static int window_width;
static int window_height;
static int last_mouse_x;
static int last_mouse_y;

void calculateAltitude()
{
    FILE* in = fopen("N35W086-2.hgt", "r");
    
    if (in == NULL)
    {
        perror("cannot open file\n");
        exit(-1);
    }
    
    
    for (int i=0; i<1201; i++)
    {
        for (int j=0; j<1201; j++)
        {
//            fread(&data[i][j], sizeof(short), 1, in);
            
            unsigned char unpack[2];
            
            
            fread(&unpack[1], 1, 1, in);
            fread(&unpack[0], 1, 1, in);
            
            data[i][j]=*(short*)unpack;
        }
    }
    int a=1001;
    int b=800;
    
    for (int i=a; i<1201; i++)
    {
        for (int j=b; j<1000; j++)
        {
            data2[i-a][j-b] = data[i][j];
        }
    }
//    printf("min: %d (%d,%d) -- max: %d (%d,%d)\n", min, mini, minj, max, maxi, maxj);
}

short getAltitude(int x, int z)
{
    return data2[x][z];
}

// Load external textures.
void loadExternalTextures()			
{
    // Local storage for bmp image data.
    BitMapFile *image[2];

    // Load the images.
    image[0] = getbmp("grass.bmp");
    image[1] = getbmp("skyline.bmp");
    image[2] = getbmp("water.bmp");
    
    // Bind grass image to texture object texture[0].
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
  	            GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    // Bind sky image to texture object texture[1]
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Bind water image to texture object texture[2].
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->sizeX, image[2]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

}

// CreateSurface Globals
static int xres=200;
static int zres=200;

static float* surfaceVerts;
static int* surfaceTris;
static float* surfaceTexs;

// nx, nz >= 2
void createSurface(float minX, float maxX, float minZ, float maxZ, int nx, int nz)
{
    float dx = (maxX - minX) / (nx-1); // delta coord x
    float dz = (maxZ - minZ) / (nz-1); // delta coord z
    
    float dtx = 1.0 / (nx-1);     // delta texture x
    float dtz = 1.0 / (nz-1);     // delta texture z

    surfaceVerts = new float[nx*nz*3];
    surfaceTexs = new float[nx*nz*2];
    surfaceTris = new int[(nx-1)*(nz-1)*6];

    int vp=0;
    int tp=0;
 
    // vertex data
    for (int row=0; row<nz; row++)
    {
        for (int col=0; col<nx; col++)
        {
            surfaceVerts[vp++]=col*dx+minX;
            surfaceVerts[vp++]=0;
            surfaceVerts[vp++]=row*dz+minZ;
            
            surfaceTexs[tp++]=col*dtx;
            surfaceTexs[tp++]=row*dtz;
        }
    }

    // triangle data
    tp=0;
    for (int row=0; row<nz-1; row++)
    {
        for (int col=0; col<nx-1; col++)
        {
            surfaceTris[tp++]=row*nx+col;
            surfaceTris[tp++]=row*nx+col+1;
            surfaceTris[tp++]=row*nx+col+1+nx;

            surfaceTris[tp++]=row*nx+col;
            surfaceTris[tp++]=row*nx+col+1+nx;
            surfaceTris[tp++]=row*nx+col+nx;
        }
    }
}
// variables for simplicity when changing values
// - used in setup() for createSurface() and drawScene()
float x0 = -1500; // minX
float x1 = 1500;  // maxX
float z0 = 1800;  // minZ
float z1 = -1800; // maxZ

// Initialization routine.
void setup(void)
{    
    glClearColor(1,1,1,1);

    // Create texture ids.
    glGenTextures(3, texture);
    // load textures
    loadExternalTextures();
    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
//    createSurface(-1500, 1500, 1800, -1800, xres, zres);
    createSurface(x0, x1, z0, z1, xres, zres);
    calculateAltitude();
    short alt;
    for (int row=0; row<zres; row++)
    {
        for (int col=0; col<xres; col++)
        {
//            surfaceVerts[(row*xres+col)*3+1] = sin(row*col);
            alt = getAltitude(row, col); // or just: alt = data[row][col];
            surfaceVerts[(row*xres+col)*3+1] = alt;
            printf("alt: %d, (%d,%d)\n", alt, row, col);
        }
    }

   // enable stuff
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);

   // Cull back faces.
   // glEnable(GL_CULL_FACE);
   // glCullFace(GL_BACK);
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

//    gluLookAt(0.0, 10.0, 20.0, 0.0, 0, -15, 0.0, 1.0, 0.0);
    gluLookAt(my_x, my_y, my_z,
              my_x - 5*sin(M_PI/180 * my_angle),
              my_y + 5*sin(M_PI/180 * myY_angle),
              my_z - 5*cos(M_PI/180 * my_angle),
              0, 1, 0);

//---- Map the grass texture[0] -------------------------/
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // draw the surface
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, surfaceVerts);
    glTexCoordPointer(2, GL_FLOAT, 0, surfaceTexs);

    glDrawElements(GL_TRIANGLES, (zres-1)*(xres-1)*6, GL_UNSIGNED_INT, surfaceTris);
//-------------------------//----------------------------/

//---- Map the skyline texture[1] -----------------------/
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 0.0, -70.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(100.0, 0.0, -70.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(100.0, 120.0, -70.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-100.0, 120.0, -70.0);
    glEnd();
//-------------------------//----------------------------/
    
//---- Map the water texture[1] -------------------------/
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(x0, 200, z0);
    glTexCoord2f(1.0, 0.0); glVertex3f(x1, 200, z0);
    glTexCoord2f(1.0, 1.0); glVertex3f(x1, 200, z1);
    glTexCoord2f(0.0, 1.0); glVertex3f(x0, 200, z1);
    glEnd();
//-------------------------//----------------------------/

    
    glutSwapBuffers();	
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    window_width=w;
    window_height=h;
    
    last_mouse_x = w/2;
    last_mouse_y = h/2;
    
    float aspect = w*1.0/h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
    gluPerspective(90, aspect, 0.1, 5000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch(key) 
    {
        case 27:
            exit(0);
            break;
            
        case 'd':    // go straight down
            my_z -= 5*sin((M_PI/180.0)*my_angle);
            my_y -= 5*cos((M_PI/180.0)*my_angle);
            break;
            
        case 'u':    // go straigt up
            my_z += 5*sin((M_PI/180.0)*my_angle);
            my_y += 5*cos((M_PI/180.0)*my_angle);
            break;
            
        case '/':    // move backward in the direction, regardless of gravity
            my_x += 1*sin((M_PI/180.0)*my_angle);
            my_z += 1*cos((M_PI/180.0)*my_angle);
            my_y -= 1*sin((M_PI/180.0)*myY_angle);
            break;
            
        case '?':    // move forward in the direction, regardless of gravity
            my_x -= 1*sin((M_PI/180.0)*my_angle);
            my_z -= 1*cos((M_PI/180.0)*my_angle);
            my_y += 1*sin((M_PI/180.0)*myY_angle);
            break;
            
        case '.':   // look left without mouse
            my_angle+=5;
            break;
            
        case '>':   // look right without mouse
            my_angle-=5;
            break;
        default:
            break;
    }
    if (my_angle > 360.0) my_angle -= 360;
    if (my_angle < 0) my_angle += 360;
    if (myY_angle > 90) my_angle -= 90;
    if (myY_angle < 0) my_angle += 90;
    glutPostRedisplay();
}

void specialKeyInput(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:    // my_angle+=5;
            my_z += 10*sin((M_PI/180.0)*my_angle);
            my_x -= 10*cos((M_PI/180.0)*my_angle);
            break;
            
        case GLUT_KEY_RIGHT:   // my_angle-=5;
            my_z -= 10*sin((M_PI/180.0)*my_angle);
            my_x += 10*cos((M_PI/180.0)*my_angle);
            break;
            
        case GLUT_KEY_UP:
            my_x -= 10*sin((M_PI/180.0)*my_angle);
            my_z -= 10*cos((M_PI/180.0)*my_angle);
            break;
            
        case GLUT_KEY_DOWN:
            my_x += 10*sin((M_PI/180.0)*my_angle);
            my_z += 10*cos((M_PI/180.0)*my_angle);
            break;
    }
    if (my_angle > 360.0) my_angle -= 360;
    if (my_angle < 0) my_angle += 360;
    //    printf("my angle: %f\n", my_angle);
    glutPostRedisplay();
}
void trackMouse(int x, int y)
{
    //    printf("mouse:  %d %d\n", x,y);
    last_mouse_x = x;
    last_mouse_y = y;
}
void mousePositionIdleUpdate()
{
    // if the last recorded mouse position is near the horizontal window
    // boundaries, update the angle and redraw the scene
    if (last_mouse_x <= .15*window_width)
    {
        my_angle += 0.3;
        if (last_mouse_x <= .075*window_width) my_angle+=0.3;
        glutPostRedisplay();
    }
    else if (last_mouse_x >= .85*window_width)
    {
        my_angle -= 0.3;
        if (last_mouse_x >= .925*window_width) my_angle-=0.3;
        glutPostRedisplay();
    }
    else if (last_mouse_y <= .1*window_height)
    {
        myY_angle += 0.5;
        if (last_mouse_y <= .03*window_height) myY_angle+=0.5;
        if (myY_angle > 90) myY_angle = 90;
        glutPostRedisplay();
    }
    else if (last_mouse_y >= .9*window_height)
    {
        myY_angle -= 0.5;
        if (last_mouse_y >= .97*window_height) myY_angle-=0.5;
        if (myY_angle <= -90) myY_angle = -90;
        glutPostRedisplay();
    }
}


// Main routine.
int main(int argc, char **argv) 
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Chattanooga Valley 5.cpp");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);
    glutPassiveMotionFunc(trackMouse);
    glutIdleFunc(mousePositionIdleUpdate);
    

   setup(); 

   glutMainLoop(); 
}
