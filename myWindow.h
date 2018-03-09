#ifndef WINDOW_H_
#define WINDOW_H_
#include<windows.h>
#include<GL/glew.h>
#include<glut.h>
#include<GL/glu.h>
#include<GL/gl.h>
#include <cstring>
#include <iostream>
#include "Cube.h"
#include "Object3d.h"
#include "MatVec.h"
#include "transformation.h"
using namespace std;
class myWindow
{
private:
    int height;
    int width;
    string name;
    int positionx;
    int positiony;
    int ac;
    char **av;
    static Vec3 camera;
    static int flag;

public:
    static Object3d myobj1,myobj2,road;
    myWindow(int argc, char ***argv,int h=500, int w=1000, int px=0, int py=0,string wname="my window")
    {
        height=h;
        width=w;
        name=wname;
        positionx=px;
        positiony=py;
        ac=argc;
        av=*argv;
        flag=1;
        camera=Vec3(0,0,40);
        myobj1.loadObject("E:/graphics_cloned_projects/myprojects/SUSHANTE/objects/plant.obj");
        myobj2.loadObject("E:/graphics_cloned_projects/myprojects/SUSHANTE/objects/plant.obj");
        road.loadObject("E:/graphics_cloned_projects/myprojects/SUSHANTE/objects/carexported.obj");




        show();

    }
    static void keyPressed(unsigned char key, int x, int y)
    {
        if (key == 'w')
        {
            flag=2;


        }
        if (key == 'a')
        {
            flag=0;

        }
        if (key == 's')
        {
            flag=3;

        }
        if (key == 'd')
        {

            flag=1;

        }
         if (key == 'z')
        {
            flag=5;

        }
         if (key == 'x')
        {
            flag=4;

        }
        glutPostRedisplay();
    }
    void show()
    {

        glutInit(&ac, av);
        glutInitWindowSize(width,height);
        glutCreateWindow("my window");

        glutDisplayFunc(display);
        glutKeyboardFunc(keyPressed);
        glutReshapeFunc(reshape);
        glutMainLoop();
    }
    static void display(void)
    {

        //Vec3 fcamera(-5,0,-80);

        Vec3 color1(0,0,1);
        Vec3 color2(0,1,0);
        Vec3 color3(0,0,0);

        int wireframe = 0;
        float intensity = 1;
        int lightsource = 5;

        //Vec3 camera(-25,-25,-25);
        Vec3 LookTo(0,0,0);
        vector<Vec3> Lightposition1;
        vector<Vec3> Lightposition2;
        vector<Vec3> Lightposition3;
        Lightposition1.push_back(Vec3(0,0,400));
        glClearColor(1,1,1,1);
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_LINES);
        myobj1.render(camera,LookTo,Lightposition1,0.5,0.5,color1,intensity,flag);
        myobj2.render(camera,LookTo,Lightposition1,0.5,0.5,color2,intensity,flag);
        road.render(camera,LookTo,Lightposition1,0.5,0.5,color3,intensity,flag);
        glEnd();
       glutSwapBuffers();
       // model.render(camera,LookTo,Lightposition1,0.5,0.5,color,intensity);
        glFlush();  /* Single buffered, so needs a flush. */
    }
    static void reshape(int w, int h)
    {
          /* Because Gil specified "screen coordinates" (presumably with an
             upper-left origin), this short bit of code sets up the coordinate
             system to correspond to actual window coodrinates.  This code
             wouldn't be required if you chose a (more typical in 3D) abstract
             coordinate system. */

          glViewport(0, 0, w, h);       /* Establish viewing area to cover entire window. */
          glMatrixMode(GL_PROJECTION);  /* Start modifying the projection matrix. */
          glLoadIdentity();             /* Reset project matrix. */
          glOrtho(0, w, 0, h, -1, 1);   /* Map abstract coords directly to window coords. */
          glScalef(1, -1, 1);           /* Invert Y axis so increasing Y goes down. */
          glTranslatef(0, -h, 0);       /* Shift origin up to upper-left corner. */
    }


};
Object3d myWindow::myobj2= Object3d(0,0);
Object3d myWindow::myobj1= Object3d(10,10);
Object3d myWindow::road= Object3d(0,0);
Vec3 myWindow::camera= Vec3(0,0,0);
int myWindow::flag=1;
#endif // WINDOW_H_

