///////////////////////////////////
// Chattanooga Valley.cpp
//
// OpenGL program to render a 3d model of the Chattanooga Valley.
// 
// Michael Crow.
///////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
//#  include <GL/glew.h>
//#  include <GL/freeglut.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#else
//#  include <GL/glew.h>
//#  include <GL/freeglut.h>
#  include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 
#endif

#include "getbmp.h"

#define PI 3.14159265358979324

using namespace std;

static float my_x=0;
static float my_y=0;
static float my_z=0;
static float my_angle=0;
static float myY_angle=0;

static int window_width;
static int window_height;
static int last_mouse_x;
static int last_mouse_y;

int width = 20;
int depth = 20;

//static float* vertices;
//static int* tris;

float dx = 1;
float dz = 1;

static unsigned int texture[1]; // Array of texture indices.

void loadExternalTextures()
{
    // Local storage for bmp image data.
    BitMapFile *image[1];
    
    // Load the images.
    image[0] = getbmp("grass.bmp");
    
    // Bind grass image to texture object texture[0].
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
}

static int xres=100;
static int zres=50;


static float* surfaceVerts;
static int* surfaceTris;
static float* surfaceTexs;


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

// Initialization routine.
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
//    glEnableClientState(GL_VERTEX_ARRAY);
//    //glEnable(GL_DEPTH_TEST);
//    //glEnable(GL_POLYGON_OFFSET_FILL);
//    //glPolygonOffset( 1, 1);
//    
//    vertices = new float[width*depth*3];
//    tris = new int[(width-1)*(depth-1)*6];
//    
//    int v=0;
//    for (int row=0; row<depth; row++)
//    {
//        for (int col=0; col<width; col++)
//        {
//            vertices[v++] = col*dx;
//            vertices[v++] = 0;
//            vertices[v++] = row*dz;
//        }
//    }
//    printf("vertices: %d -- %d\n", v, width*depth*3);
//    
//    int t=0;
//    for (int row=0; row<depth-1; row++)
//    {
//        for (int col=0; col<width-1; col++)
//        {
//            tris[t++] = row*width+col;
//            tris[t++] = row*width+col+1;
//            tris[t++] = row*width+col+1+width;
//            tris[t++] = row*width+col;
//            tris[t++] = row*width+col+1+width;
//            tris[t++] = row*width+col+width;
//        }
//    }
//    
//    printf("tris: %d -- %d\n", t, (depth-1)*(width-1)*6);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    
    // Create texture ids.
    glGenTextures(2, texture);
    // load textures
    loadExternalTextures();
    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    createSurface(-20, 20, 20, -20, xres, zres);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

// Drawing routine.
void drawScene(void)
{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();
//    // calculate heading
//    gluLookAt(my_x, my_y, my_z,
//              my_x - 5*sin(M_PI/180 * my_angle),
//              my_y + 5*sin(M_PI/180 * myY_angle),
//              my_z - 5*cos(M_PI/180 * my_angle),
//              0, 1, 0);
//    
//    glColor3f(0.0, 0.0, 0.0);
//    
//    drawGround();
//    glFlush();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    
    gluLookAt(0.0, 10.0, 20.0, 0.0, 0, -15, 0.0, 1.0, 0.0);
    
    // Map the texture
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    // glColor3f(0,0,0);
    
    // draw the surface
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, surfaceVerts);
    glTexCoordPointer(2, GL_FLOAT, 0, surfaceTexs);
    
    
    glDrawElements(GL_TRIANGLES, (zres-1)*(xres-1)*6, GL_UNSIGNED_INT, surfaceTris);
    
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
    //gluPerspective(50, aspect, 0.1, 250);
       glFrustum(-5,5,-5,5,5,100);
    //   glOrtho(-5,5,-5,5,0,200);
//    glOrtho(-5, 25, -1, 10, -5, 25);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);   // escape
            break;
        case 'd':    // go straight down
            my_z -= 1*sin((M_PI/180.0)*my_angle);
            my_y -= 1*cos((M_PI/180.0)*my_angle);
            break;
            
        case 'u':    // go straigt up
            my_z += 1*sin((M_PI/180.0)*my_angle);
            my_y += 1*cos((M_PI/180.0)*my_angle);
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
    if (myY_angle > 360.0) my_angle -= 360;
    if (myY_angle < 0) my_angle += 360;
    glutPostRedisplay();
}

void specialKeyInput(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:    // my_angle+=5;
            my_z += 1*sin((M_PI/180.0)*my_angle);
            my_x -= 1*cos((M_PI/180.0)*my_angle);
            break;
            
        case GLUT_KEY_RIGHT:   // my_angle-=5;
            my_z -= 1*sin((M_PI/180.0)*my_angle);
            my_x += 1*cos((M_PI/180.0)*my_angle);
            break;
            
        case GLUT_KEY_UP:
            my_x -= 1*sin((M_PI/180.0)*my_angle);
            my_z -= 1*cos((M_PI/180.0)*my_angle);
            break;
            
        case GLUT_KEY_DOWN:
            my_x += 1*sin((M_PI/180.0)*my_angle);
            my_z += 1*cos((M_PI/180.0)*my_angle);
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
        glutPostRedisplay();
    }
    else if (last_mouse_y >= .9*window_height)
    {
        myY_angle -= 0.5;
        if (last_mouse_y >= .97*window_height) myY_angle-=0.5;
        glutPostRedisplay();
    }
//    idleRotate();
}


// Main routine.
int main(int argc, char **argv) 
{
   glutInit(&argc, argv);

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(100, 100); 
   glutCreateWindow("ChattanoogaValley.cpp");
   glutDisplayFunc(drawScene); 
   glutReshapeFunc(resize);  
   glutKeyboardFunc(keyInput);
   glutSpecialFunc(specialKeyInput);
   glutPassiveMotionFunc(trackMouse);
    
   glutIdleFunc(mousePositionIdleUpdate);

   setup(); 
   
   glutMainLoop(); 
}