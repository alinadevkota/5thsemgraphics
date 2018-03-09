#include <GL/glut.h>
#include "MatVec.h"
#include <vector>
#include<algorithm>
#include<math.h>
#include<iostream>
using namespace std;




class Cube
{
private:
    vector <Vec2> vertBuffer;
    vector <Vec3> colorBuffer;
public:
    Cube()
    {

    }
    void loadObject()
    {
        vertBuffer.push_back(Vec2(0, 0));
        vertBuffer.push_back(Vec2(200, 200));
        vertBuffer.push_back(Vec2(20, 200));
        colorBuffer.push_back(Vec3(0.0, 0.0, 1.0));
        colorBuffer.push_back(Vec3(0.0, 1.0, 0.0));
        colorBuffer.push_back(Vec3(1.0, 0.0, 0.0));
    }

    void drawBox(void)
    {

      glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_TRIANGLES);

    for (int i=0;i<3;i++)
    {
        glColor3f(colorBuffer[i].x,colorBuffer[i].y,colorBuffer[i].z);  /* blue */
        glVertex2i(vertBuffer[i].x,vertBuffer[i].y);
    }

  glEnd();
  glFlush();  /* Single buffered, so needs a flush. */
    }

};

