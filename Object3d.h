#ifndef OBJECT_H_
#define OBJECT_H_
#include<vector>
#include<math.h>
#include<fstream>
#include <sstream>
#include<string>
#include<algorithm>
#include<iostream>
#include "transformation.h"


void replaceAll(std::string& source, const std::string& from, const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += source.substr(lastPos);

    source.swap(newString);
}





struct Vertex
{
    Vec3 v;
    Vec3 norm;
    int cnt=0;
    Vertex(){
        v = Vec3(0,0,0);
        norm = Vec3(0,0,0);

    }
    Vertex(const Vec3& input){
        v = input;
        norm = Vec3(0,0,0);

    }
};

struct Edge
{
    Vec2 *v1,*v2;
    Edge(){
        v1 = NULL;
        v2 = NULL;
    }
    Edge(Vec2* a, Vec2* b):v1(a),v2(b){
        //make sure v1 has small y and v2 has larger y value
        //if not so swap them
        if (v1->y > v2->y){
            Vec2 *temp = v1;
            v1 = v2;
            v2 = temp;
        }
    }
    void operator = (Edge Ed){
        v1 = Ed.v1; v2 = Ed.v2;
    }
};


class Object3d
{
private:
    vector <Vertex> vertBuffer;     //List of Vertices
    vector <Vec2> textureBuffer;    //List of textures
    vector <Vec3> normBuffer;       //List of normals
    vector <Vec3> surfaceBuffer;    //list of Surfaces(vert,texture,norm)
    vector <Vec3> vertexNorm;         //List of vertex normal
    bool texture;
    int xoffset,yoffset;            //to move the object by some distance in x
public:

    Object3d(int xoff=0,int yoff=0)
    {
        xoffset=xoff;
        yoffset=yoff;
        texture = false;
    }
    void addVertex(Vec3& v){vertBuffer.push_back(Vertex(v));}
    void addSurface(Vec3& v){surfaceBuffer.push_back(v);}
    void addNormal(Vec3& v){normBuffer.push_back(v);}
    void addTexture(Vec2& v){textureBuffer.push_back(v);}


    void loadObject(string filename)
    {
        vertBuffer.clear();
        surfaceBuffer.clear();
        normBuffer.clear();
        textureBuffer.clear();
        bool vertonly = true, vertnorm = false, vertnormtext = false,typefixed = false;
        //These three bools say that the face contains either vertex only
        //or vertex and normal only or
        //vertex normal and texture

        ifstream obj;
        obj.open(filename.c_str(),ios::in);
        if (!obj.is_open()) std::cout<<"Can not open";
        else std::cout<<"opened"<<endl;
        vector <Vec3> temp;
        string line,keyword;

        while (getline(obj,line))
        {

//            while(line.compare(0,1," "))
//            {
//                line.erase(line.begin()); // remove leading whitespaces
//            }
//            while(line.size()>0 && line.compare(line.size()-1,1," ")==0)
//            {
//                line.erase(line.end()-1); // remove trailing whitespaces
//            }
            if (line == "" || line == "\n") continue;

            istringstream linestream(line);
            linestream >> keyword;
            //cout<<line<<endl;
            if(keyword=="v")
            {
                Vec3 temp;
                unsigned int t;
                //cout<<"hello vertices"<<endl;
                linestream>>temp.x;
                linestream>>temp.y;
                linestream>>temp.z;
                if (!(linestream>>t))
                t = 1.0f;

                //normalize wrt t
                if (t>0 && t<1)
                {
                    temp.x = temp.x / t;
                    temp.y = temp.y / t;
                    temp.z = temp.z / t;
                }

                addVertex(temp);
                //cout<<vertBuffer.size()<<endl;
            }
            else if (keyword == "vn")
            {
                Vec3 v;
                linestream >> v.x;
                linestream >> v.y;
                linestream >> v.z;
                addNormal(v);
            }
            else if(keyword == "vt")
            {
                texture = true;
                Vec2 v;
                linestream >> v.x;
                linestream >> v.y;
                addTexture(v);
            }

            else if(keyword=="f")
            {
                Vec3 v[3];
                line = line.substr(1,line.length()-1); //remove the preceding f
                while(line.compare(0,1," ")==0)
                line.erase(line.begin()); // remove leading whitespaces
                while(line.size()>0 && line.compare(line.size()-1,1," ")==0)
                line.erase(line.end()-1); // remove trailing whitespaces

                ///These sets of command run only once to determine the type of object definition
                std::size_t found = line.find("//");
                if (found!=std::string::npos && typefixed == false)
                {
                    vertonly = false; vertnorm = true; typefixed = true;
                }
                // a // means v and n

                found = line.find('/');
                if (found!=std::string::npos && typefixed == false)
                {
                    vertonly = false; vertnormtext = true; typefixed = true;
                }
                //for a single / it is v and t and n
                if (typefixed == false)
                {
                    typefixed = true;
                }
                //else it means vertonly
                ///Now we know if our face contains vertex only, vertex and normal only , or v&n&texture



                if (vertnormtext)
                {
                    replaceAll(line,"/"," "); //remove the / for easy calculatoin
                    //replace( line.begin(), line.end(), "/", " ");
                    istringstream lstream(line);
                    //v contains .x = vertex index, .y = texture .z= normal index

                    lstream >> v[0].x;lstream >> v[0].y;lstream >> v[0].z;
                    lstream >> v[1].x;lstream >> v[1].y;lstream >> v[1].z;
                    lstream >> v[2].x;lstream >> v[2].y;lstream >> v[2].z;

                    addSurface(v[0]);addSurface(v[1]);addSurface(v[2]);
                }

                else if (vertnorm)
                {
                    replaceAll(line,"//"," "); //remove the / for easy calculatoin
                    //replace( line.begin(), line.end(), "/", " ");
                    istringstream lstream(line);
                    //v contains .x = vertex index, .y = texture .z= normal index

                    lstream >> v[0].x;lstream >> v[0].z;
                    lstream >> v[1].x;lstream >> v[1].z;
                    lstream >> v[2].x;lstream >> v[2].z;

                    addSurface(v[0]);addSurface(v[1]);addSurface(v[2]);
                }

                else{
                    istringstream lstream(line);
                    //v contains .x = vertex index, .y = texture .z= normal index

                    lstream >> v[0].x;
                    lstream >> v[1].x;
                    lstream >> v[2].x;

                    addSurface(v[0]);addSurface(v[1]);addSurface(v[2]);
                }

            }

        }
        obj.close();
        for(int i=0;i<vertBuffer.size();i++)
        {
            Translate(vertBuffer[i].v,Vec3(xoffset,yoffset,0));
        }
        calculateNorm();

    }

    void drawline(Vec2 P1, Vec2 P2, Vec3 c){
        if (P1.x > P2.x){
            Vec2 temp = P1;
            P1 = P2;
            P2 = temp;
        }

        int x1 = P1.x; int y1 = P1.y;
        int x2 = P2.x; int y2 = P2.y;

        float dVal = P1.z, delta_d = P2.z - P1.z;       // The depth value of that point, and the difference delta_d
        float iVal = P1.i, delta_i = P2.i - P1.i;       //The intensity values

        float dx = x2-x1;
        for (int i=x1; i<= x2; ++i) {
            glColor3f(iVal*c.x,iVal*c.y,iVal*c.z);
            glBegin(GL_POINTS);
            glVertex2i(i,y1);
            glEnd();
            //setpixel(i, y1, dVal, c, iVal);
            dVal += delta_d/dx;
            iVal += delta_i/dx;
        }

    }

    void calculateNorm()
    {
        unsigned int len = surfaceBuffer.size();
        unsigned int t1, t2, t3;
        Vec3 V1, V2, V3;
        Vec3 norm(0,0,0);

        for(unsigned int i=0;i<len;i+=3){
            //indexes for the vertices
            t1 =surfaceBuffer[i].x-1;
            t2 =surfaceBuffer[i+1].x-1;
            t3 =surfaceBuffer[i+2].x-1;
    //

            //Actual Vertices
            V1 = vertBuffer[t1].v;
            V2 = vertBuffer[t2].v;
            V3 = vertBuffer[t3].v;

            //determine the normal of the traingle and assign
            Vec3 A = (V3-V1)/(V3-V1).magnitude();//(glm::length(V3-V1));
            Vec3 B = (V2-V1)/(V2-V1).magnitude();//(glm::length(V2-V1));
            norm = B.crossProduct(A);
            norm = norm / norm.magnitude();

            vertBuffer[t1].norm = norm + vertBuffer[t1].norm ;
            vertBuffer[t2].norm = norm + vertBuffer[t2].norm ;
            vertBuffer[t3].norm = norm + vertBuffer[t3].norm ;


            vertBuffer[t1].norm = vertBuffer[t1].norm / vertBuffer[t1].norm.magnitude();
            vertBuffer[t2].norm = vertBuffer[t2].norm / vertBuffer[t2].norm.magnitude();
            vertBuffer[t3].norm = vertBuffer[t3].norm / vertBuffer[t3].norm.magnitude();


            vertBuffer[t1].cnt++; vertBuffer[t2].cnt++; vertBuffer[t3].cnt++;
        }

        len = vertBuffer.size();
        for(unsigned int i=0;i<len;i++){
            vertBuffer[i].norm.x = vertBuffer[i].norm.x / vertBuffer[i].cnt;
            vertBuffer[i].norm.y = vertBuffer[i].norm.y / vertBuffer[i].cnt;
            vertBuffer[i].norm.z = vertBuffer[i].norm.z / vertBuffer[i].cnt;
            vertBuffer[i].norm.x = vertBuffer[i].norm.x / vertBuffer[i].norm.magnitude();
            vertBuffer[i].norm.y = vertBuffer[i].norm.y / vertBuffer[i].norm.magnitude();
            vertBuffer[i].norm.z = vertBuffer[i].norm.z / vertBuffer[i].norm.magnitude();

        }
    }



    void drawSpan(Edge& E1, Edge& E2, Vec3 color){

        //calculate the y difference for interpolation
        float e1ydiff = (float)(E1.v2->y - E1.v1->y);
        if (e1ydiff == 0)
            return;

        float e2ydiff = (float)(E2.v2->y - E2.v1->y);
        if (e2ydiff == 0)
            return;


        float x1 = E1.v1->x;
        float x2 = E2.v1->x;

        float x1i = (float)(E1.v2->x - E1.v1->x)/ e1ydiff;
        float x2i = (float)(E2.v2->x - E2.v1->x)/ e2ydiff;
        // float factor,stepfactor;
        float z1= E1.v1->z;
        float z2= E2.v1->z;
        float z1i = (float)(E1.v2->z - E1.v1->z)/e1ydiff;
        float z2i = (float)(E2.v2->z - E2.v1->z)/e2ydiff;
        float i1 = E1.v1->i;
        float i2 = E2.v1->i;
        float i1i = (float)(E1.v2->i - E1.v1->i)/e1ydiff;
        float i2i = (float)(E2.v2->i - E2.v1->i)/e2ydiff;

        // float x1,x2,z1,z2,i1,i2;
        unsigned int y;
        // loop through the lines between the edges and draw spans
        for(y = E2.v1->y; y < E2.v2->y; y++) {
            // create and draw span
            x1 += x1i;
            x2 += x2i;
            z1 += z1i;
            z2 += z2i;
            i1 += i1i;
            i2 += i2i;


                //glBegin(GL_LINE);
                //drawline(x1,x2,y,y,color,i1);
//                glColor3f(color.x*i1,color.y*i1,color.z*i1);
//                glVertex2f(x1,y);
//                glColor3f(color.x*i2,color.y*i2,color.z*i2);
//                glVertex2f(x2,y);
                //glEnd();

//            if(color == 1)
            drawline(Vec2(x1,y,z1,i1),Vec2(x2,y,z2,i2),color);
//            if(color == 2)
//            st_line(Vec2(x1+xoffset,y+yoffset,z1,i1),Vec2(x2+xoffset,y+yoffset,z2,i2),Vec3(255,0,0));
//            if(color == 3)
//            st_line(Vec2(x1+xoffset,y+yoffset,z1,i1),Vec2(x2+xoffset,y+yoffset,z2,i2),Vec3(0,100,0));
//            if(color == 4)
//            st_line(Vec2(x1+xoffset,y+yoffset,z1,i1),Vec2(x2+xoffset,y+yoffset,z2,i2),Vec3(50,50,50));

            //S->refresh();
        }
        *(E1.v1) = Vec2(x1,y,z1,i1);
    }

    void render(Vec3& camera,Vec3& LookTo,vector<Vec3> LightPos,float pWidth,float pHeight, Vec3 color, float intensityValue,int flag){
    unsigned int len = vertBuffer.size();
    Vec2 vert2d[len];
    float intensity = 0;
    Vec3 temp(0,0,0);

    for (unsigned int i=0;i<len;i++)
    {
        temp = Vec3(0,0,0);
        intensity = 0;
        if (flag==0)
        {
            //Translate(vertBuffer[i].v,Vec3(-xoffset,0,0));
            RotateX(vertBuffer[i].v,15);
            //Translate(vertBuffer[i].v,Vec3(xoffset,0,0));
        }
        else if (flag==1)
        {
            //Translate(vertBuffer[i].v,Vec3(-xoffset,0,0));
            RotateX(vertBuffer[i].v,-15);
            //Translate(vertBuffer[i].v,Vec3(xoffset,0,0));
        }
        else if (flag==2)
        {
            //Translate(vertBuffer[i].v,Vec3(0,-yoffset,0));
            RotateY(vertBuffer[i].v,15);
            //Translate(vertBuffer[i].v,Vec3(0,yoffset,0));
        }
        else if (flag==3)
        {
            //Translate(vertBuffer[i].v,Vec3(0,-yoffset,0));
            RotateY(vertBuffer[i].v,-15);
            //Translate(vertBuffer[i].v,Vec3(0,yoffset,0));
        }
        else if (flag==4)
        {
            //Translate(vertBuffer[i].v,Vec3(0,0,0));
            RotateZ(vertBuffer[i].v,15);
            //Translate(vertBuffer[i].v,Vec3(0,0,0));
        }
        else if (flag==5)
        {
            //Translate(vertBuffer[i].v,Vec3(-xoffset,-yoffset,0));
            RotateZ(vertBuffer[i].v,-15);
            //Translate(vertBuffer[i].v,Vec3(xoffset,yoffset,0));
        }
        vert2d[i] = World_To_Pixel(vertBuffer[i].v,camera,LookTo,pWidth,pHeight,640,480,flag);
//        //assign intensity here for shading
        for(unsigned int j=0; j<LightPos.size();j++){
            Vec3 A = LightPos[j] - camera;
            A = A / A.magnitude();
            intensity = intensity + vertBuffer[i].norm.dotProduct(A);
        }
        //if it is > 1 we truncate it to be 1
        //change the intensity component
          intensity *= intensityValue;
        if (intensity > 1)
            intensity = 1;
//        cout << v[i].z << endl;

        // if the intensity is -ve we simply avoid the intensity as it is the back face
        if (intensity < 0)
            intensity = 0.05;
//        else
//            Lpos.push_back(vertBuffer[i].v);
         vert2d[i].i = intensity;
    }

    unsigned int t1,t2,t3;
    len = surfaceBuffer.size();

    //temp store of vertices and edges
    Vec2 v[3];
    Edge E[3];

    //for all surfaces
    for(unsigned int i=0;i<len;i+=3){
        //indexes for the vertices
        t1 =surfaceBuffer[i].x-1;
        t2 =surfaceBuffer[i+1].x-1;
        t3 =surfaceBuffer[i+2].x-1;
       // if (t1 <0 || t2 <0||t3 <0) continue;
      //vertexes
      v[0] = vert2d[t1];
      v[1] = vert2d[t2];
      v[2] = vert2d[t3];
        //Edges
        E[0] = Edge(v,v+1);
        E[1] = Edge(v+1,v+2);
        E[2] = Edge(v+2,v);

        //we need to find the longest edge in y-axis. to do so check the y-lenght of all edges
        float maxLen =0;
        int longIndex =0;
        for (int i=0;i<3;i++){
            float Len = E[i].v2->y - E[i].v1->y;
            if (Len>maxLen){
                maxLen = Len;
                longIndex = i;
            }
        }

        //Index for other two edges
        int shortIndex1 = (longIndex + 1) % 3;
        int shortIndex2 = (longIndex + 2) % 3;

        if (E[shortIndex1].v1->y > E[shortIndex2].v1->y)
            swap (shortIndex1,shortIndex2);

        drawSpan(E[longIndex],E[shortIndex1],color);
        drawSpan(E[longIndex],E[shortIndex2],color);

    }




}





};




#endif // OBJECT_H_
