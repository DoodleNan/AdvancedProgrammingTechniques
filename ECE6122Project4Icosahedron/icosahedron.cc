// Draw an Icosahedron
// ECE4893/8893 Project 4
// YOUR NAME HERE

//
//  ECE6122Icosahedron
//
//  Created by Ashlie on 4/9/17.
//  Copyright © 2017 Ashlie. All rights reserved.
//
#include <iostream>
#include <math.h>

#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>



using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

static int updateRate = 10;
// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

void normalize(GLfloat v[3]){
  GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  if(d == 0) return;
  v[0] /= d;
  v[1] /= d;
  v[2] /= d;
}

void drwaTriangle(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]){
  glBegin(GL_TRIANGLES);
  GLfloat colorx = (v1[0] + v2[0] + v3[0])/3.0 + 0.5;
  GLfloat colory = (v1[1] + v2[1] + v3[1])/3.0 + 0.5;
  GLfloat colorz = (v1[2] + v2[2] + v3[2])/3.0 + 0.5;
  glColor3f(colorx, colory, colorz);
  glVertex3fv(v1);
  glVertex3fv(v2);
  glVertex3fv(v3);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(1,1,1);
  glVertex3fv(v2);
  glVertex3fv(v3);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(1,1,1);
  glVertex3fv(v3);
  glVertex3fv(v1);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(1,1,1);
  glVertex3fv(v1);
  glVertex3fv(v2);
  glEnd();
}

void subDivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth){
   if(depth == 0){
    drwaTriangle(v1, v2, v3);
    return;
   }

   GLfloat v12[3];
   GLfloat v23[3];
   GLfloat v31[3];

   for(int i = 0; i <3; i++){
    v12[i] = v1[i] + v2[i];
    v23[i] = v2[i] + v3[i];
    v31[i] = v3[i] + v1[i];
   }
   normalize(v12);
   normalize(v23);
   normalize(v31);

   // now draw the smaller triangles
   subDivide(v1, v12, v31, depth-1);
   subDivide(v2, v23, v12, depth-1);
   subDivide(v3, v31, v23, depth-1);
   subDivide(v12, v23, v31, depth-1); 
}


int testNumber; // Global variable indicating which test number is desired
int depth;
// Test cases.  Fill in your code for each test case
void Test1() 
{
  for(int i = 0; i < 20; i++){
    subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], 0);
  }
}

void Test2()
{
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY += 1.0;

  for(int i = 0; i < 20; i++){
    subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], 0);
  }
}

void Test3()
{
  for(int i = 0; i < 20; i++){
    subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], 1);
  }
}

void Test4()
{
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY += 1.0;

  for(int i = 0; i < 20; i++){
    subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], 1);
  }
}

void Test5(int depth)
{
  for(int i = 0; i < 20; i++){
    subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], depth);
  }
}

void Test6(int depth)
{
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY += 1.0;

  for(int i = 0; i < 20; i++){
    subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], depth);
  }
}

void display(){
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  // glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0,1.0, 0.0);
  
  switch(testNumber){
    case 1: Test1(); break;
    case 2: Test2(); break;
    case 3: Test3(); break;
    case 4: Test4(); break;
    case 5: Test5(depth); break;
    case 6: Test6(depth); glFlush(); break;
  }
}

void reshape(int width, int height){
  GLfloat aspect = (GLfloat) width / (GLfloat) height;
  glViewport(0,0,width,height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(width <= height){
    glOrtho(-1, 1, -1 * aspect, 1 * aspect, -2.0, 2.0);
  }else{
    glOrtho(-1 * aspect, 1 * aspect, -1, 1, -2.0, 2.0);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
  }
}

void timer(int){
  glutPostRedisplay();
  glutTimerFunc(1000.0 / updateRate, timer, 0);
}

void init(){
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glLineWidth(2);
  glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
  // Set the global test number
  testNumber = atol(argv[1]);
  if (argc > 2){
    depth = atoi(argv[2]);
  }
  // Initialize glut  and create your window here
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutCreateWindow("Icosahedron");  
  // Set your glut callbacks here
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  // Enter the glut main loop here
  init();
  glutMainLoop();
  return 0;
}

