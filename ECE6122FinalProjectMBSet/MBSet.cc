// Calculate and display the Mandelbrot set
// ECE8893 final project, Spring 2017

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "complex.h"
#include <vector>
using namespace std;

class Color{
private:
	float r;
	float g;
	float b;
public:
	Color(){
        		
	}
	Color(float _r, float _g, float _b){
		r = _r;
		g = _g;
		b = _b;
	}
	float getR(){
		return r;
	}
	float getG(){
		return g;
	}
	float getB(){
		return b;
	}
    bool operator == (Color& c) const{
        return r == c.getR() && g == c.getG() && b == c.getB();
    }
};

class Memory{
private:
	Complex min;
	Complex max;
public:
	Memory(){
		min = Complex(0,0);
		max = Complex(0,0);
	}
	Memory(Complex _min, Complex _max){
		min = _min;
		max = _max;
	}
	Complex getMin(){
		return min;
	}
	Complex getMax(){
		return max;
	}
};
vector<Memory> MemVec;

class Position{
public:
	float x;
	float y;
public:
	Position(){

	}

	Position(float _x, float _y){
		x = _x;
		y = _y;
	}
};
// Min and max complex plane values
Complex minC(-2.0, -1.2);
Complex maxC( 1.0, 1.8);
int nThreads = 16;
int maxIt = 2000;     // Max iterations for the set computations
int numPixels = 512;

bool* barrierCnt = new bool[nThreads];
bool drawBox = false;
Position start;
Position end;
Complex* positions = new Complex[numPixels * numPixels];
int *iterationCnts = new int[numPixels * numPixels];
Color* colors = new Color[maxIt+1];


void ColorMapping(){
	for(int i = 0; i<maxIt; i++){
        if(i<5){
            colors[i] = Color(1,1,1);
        }else{
	    	colors[i] = Color(drand48()+0.5,drand48(),drand48());
        }
    }
}

void MyBarrier_Init(){
	for(int i = 0; i<nThreads; i++){
		barrierCnt[i] = false;
	}
}
void MyBarrier_Set(int i){
	barrierCnt[i] = true;
}

bool MyBarrier_Check(){
	for(int i = 0; i<nThreads; i++){
		if(barrierCnt[i] == false) return false;
	}
	return true;
}
// Each thread call MyBarrier after computations
void MyBarrier(){
	while(MyBarrier_Check() == false){

	}
}
//Each line each column
void CalIter(int offset){
	for(int i = 0; i<numPixels; i++){
		int pos = offset+i;
		int cnt = 0;
		Complex c = positions[pos];
		while(cnt < maxIt && c.Mag2() < 4.0){
			cnt++;
			c = (c * c) + positions[pos];
		}
      //  if(cnt == 2000) 
           // cout<<"find black here!!!!!!!!!!"<<endl
		iterationCnts[pos] = cnt;
	}
}

void* MBSetThread(void* v){
	unsigned long ID = (unsigned long)v;
	int rowPerThread = numPixels/nThreads;
	int startRow = ID * rowPerThread;
	int offset = 0;
	for(int i = 0; i<rowPerThread; i++){
		offset = (startRow + i) * numPixels;
		CalIter(offset);
	}
	MyBarrier_Set((int)ID);
    cout<<"Wave from thread sweety No "<<(int)ID<<endl;
	return 0;
}

void CalPositions(){
	float width = maxC.real - minC.real;
	float height = maxC.imag - minC.imag;
	for(int i = 0; i<numPixels; i++){
		for(int j = 0; j<numPixels; j++){
			float real = minC.real +(float)i * width/numPixels;
			float imag = minC.imag + (float)j * height/numPixels;
			positions[i*numPixels+j] = Complex(real, imag);
		}
	}
}
void MBSet(){
	CalPositions();
    MyBarrier_Init();
    for(int i = 0; i<nThreads; i++){
        pthread_t pthread;
        pthread_create(&pthread, 0, MBSetThread, (void*)i);
    }
    MyBarrier();
    cout<<"Umm seems like all jobs done here"<<endl;
}
void display(void)
{ // Your OpenGL display code here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	glClearColor(1.0, 1.0, 1.0, 1.0);
    glLoadIdentity();
  	
  	glBegin(GL_POINTS);
  	for(int i = 0; i<numPixels; i++){
  		for(int j = 0; j<numPixels; j++){
  			glColor3f(colors[iterationCnts[i*numPixels+j]].getR(), colors[iterationCnts[i*numPixels+j]].getG(), colors[iterationCnts[i*numPixels+j]].getB());
  			glVertex2d(i, j);
  		}
  	}
  	glEnd();

  	int l = abs(start.x - end.x);
  	int min_x = start.x < end.x ? start.x : end.x;
  	int min_y = start.y < end.y ? start.y : end.y;
  	if(drawBox == true){
        glBegin(GL_LINES);
	    glColor3f(1,0,0);
	  	glVertex2d(min_x, min_y);
	  	glVertex2d(min_x, min_y + l);
	  	glEnd();

	  	glBegin(GL_LINES);
	    glColor3f(1,0,0);
	  	glVertex2d(min_x, min_y);
	  	glVertex2d(min_x + l, min_y);
	  	glEnd();
	  	
	  	glBegin(GL_LINES);
	    glColor3f(1,0,0);
	  	glVertex2d(min_x + l, min_y);
	  	glVertex2d(min_x + l, min_y + l);
	  	glEnd();

	  	glBegin(GL_LINES);
	    glColor3f(1,0,0);
	  	glVertex2d(min_x, min_y + l);
	  	glVertex2d(min_x + l, min_y + l);
	  	glEnd();
  	}
  	glutSwapBuffers();
}

void init()
{ // Your OpenGL initiali ation code here
	glViewport(0,0,numPixels, numPixels);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,numPixels,numPixels,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void reshape(int w, int h){ 
// Your OpenGL window reshape code here
}

void mouse(int button, int state, int x, int y){ 
// Your mouse click processing here
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.
	if(button == GLUT_LEFT_BUTTON && state == 0){
		start.x = x;
		start.y = y;
		end.x = x;
		end.y = y;
		drawBox = true;
        cout<<"I am drawing a cool square now!"<<endl;
	}
	if(button == GLUT_LEFT_BUTTON && state != 0){
		MemVec.push_back(Memory(minC, maxC));
		int dx = abs(x - start.x);
		int dy = abs(y - start.y);
		int l = dx > dy ? dy : dx;
		end.x = x > start.x ? start.x + l : start.x - l;
		end.y = y > start.y ? start.y + l : start.y - l;
		
		int start_row = start.x < end.x ? start.x : end.x;
		int start_col = start.y < end.y ? start.y : end.y;
		minC.real = positions[start_row * numPixels + start_col].real;
		minC.imag = positions[start_row * numPixels + start_col].imag;

		int end_row = start.x < end.x ? end.x : start.x;
		int end_col = start.y < end.y ? end.y : start.y;
		maxC.real = positions[end_row * numPixels + end_col].real;
		maxC.imag = positions[end_row * numPixels + end_col].imag;

		MBSet();
		glutPostRedisplay();
		drawBox = false;
        cout<<"I have finished the cool square now... Awesome job done!"<<endl;
	}
}

void motion(int x, int y){ 
// Your mouse motion here, x and y coordinates are as above
	int dx = abs(x - start.x);
	int dy = abs(y - start.y);
	int l = dx < dy ? dx : dy;
	end.x = x > start.x ? start.x + l : start.x - l;
	end.y = y > start.y ? start.y + l : start.y - l;
	glutPostRedisplay();
}

void keyboard(unsigned char c, int x, int y){ 
// Your keyboard processing here
	if(c == 'b'){
		if(MemVec.size() > 0){
			Memory mem = MemVec.back();
			MemVec.pop_back();
			maxC.real = mem.getMax().real;
			maxC.imag = mem.getMax().imag;
			minC.real = mem.getMin().real;
			minC.imag = mem.getMin().imag;
			MBSet();
			glutPostRedisplay();
		}else{
			cout<<"You idiot!"<<endl;
		}
	}else{
        cout<<"usage: press 'b' to retrieve MBSet pictures"<<endl;
    }

}

int main(int argc, char** argv){
  // Initialize OpenGL, but only on the "master" thread or process.
  // See the assignment writeup to determine which is "master" 
  // and which is slave.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitDisplayMode(GLUT_DEPTH);
    glutInitDisplayMode(GL_DOUBLE);
    glClearColor(1.0,1.0,1.0,0);
	glutInitWindowSize(numPixels, numPixels);
	glutCreateWindow("MBSet");
	ColorMapping();
	MBSet();
	init();
	
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();
  return 0;
}

