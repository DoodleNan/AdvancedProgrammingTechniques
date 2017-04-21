// Threaded two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 2


#include <iostream>
#include <string>
#include <math.h>

#include "Complex.h"
#include "InputImage.h"

// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.

using namespace std;
Complex* ImageData;
int ImageWidth;
int ImageHeight;
int N;
int nThread = 16;
int count;
bool* barrierCnt = new bool[nThread];
pthread_mutex_t cntMutex;
pthread_mutex_t exitMutex;
pthread_cond_t exitCondi;
// Function to reverse bits in an unsigned integer
// This assumes there is a global variable N that is the
// number of points in the 1D transform.
unsigned ReverseBits(unsigned v)
{ //  Provided to students
  unsigned n = N; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.

// Call MyBarrier_Init once in main
void MyBarrier_Init(){// you will likely need some parameters)
    for (int i = 0; i<nThread; i++) {
        barrierCnt[i] = false;
    }
}

void setBarri(int i){
    barrierCnt[i] = true;
}
bool checkBarri(){
    for (int i = 0; i<nThread; i++) {
        if (barrierCnt[i] == false) {
            return false;
        }
    }
    return true;
}
// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier(){ // Again likely need parameters
    while (checkBarri() == false) {
    }
}


void Transpose(Complex* h, int width, int height){
    for (int i = 0; i<height; i++) {
        for (int j = 0; j<width; j++) {
            if (j > i) {
                Complex temp = h[i*width+j];
                h[i*width+j] = h[j*width+i];
                h[j*width+i] = temp;
            }
        }
    }
}
void Transform1D(Complex* h, int width)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
    Complex* temp = new Complex[width]();
    for (int i = 0; i<width; i++) {
        temp[ReverseBits(i)] = h[i];
    }
    for (int i = 0; i<width; i++) {
        h[i] = temp[i];
    }
    delete []temp;
    //Compute Wn values
    
    Complex* W = new Complex[width]();
    for (int i = 0; i<width/2; i++) {
        W[i] = Complex(cos(2*M_PI*i/N), -sin(2*M_PI*i/width));
        W[i+N/2] = Complex(-1) * W[i];
    }
    
    int len = 2;
    int groupNum = width/len;
    while(groupNum>=1){
        for (int i = 0; i<groupNum; i++) {
            int iter = 0;
            Complex* temp = new Complex[len];
            for (int j = i*len; j<(i+1)*len; j++) {
                if (iter < len/2) {
                    temp[iter] = h[j] + W[iter*N/len]*h[j+len/2];
                }else{
                    temp[iter] = h[j-len/2] + W[iter*N/len]*h[j];
                }
                iter++;
            }
            for (int j = 0; j<len; j++) {
                h[i*len+j] = temp[j];
            }
            delete []temp;
        }
        len = len * 2;
        groupNum = groupNum/2;
    }
    delete []W;
}

void Transform1DInverse(Complex* h, int width)
{
    // Implement the efficient Danielson-Lanczos IDFT here.
    // "h" is an input/output parameter
    // "N" is the size of the array (assume even power of 2)
    Complex* temp = new Complex[width]();
    for (int i = 0; i<width; i++) {
        temp[ReverseBits(i)] = h[i];
    }
    for (int i = 0; i<width; i++) {
        h[i] = temp[i];
    }
    delete []temp;
    //Compute Wn values
    
    Complex* W = new Complex[width]();
    for (int i = 0; i<N/2; i++) {
        W[i] = Complex(cos(2*M_PI*i/width), sin(2*M_PI*i/width));
        W[i+width/2] = Complex(-1) * W[i];
    }
    
    int len = 2;
    int groupNum = width/len;
    while(groupNum>=1){
        for (int i = 0; i<groupNum; i++) {
            int iter = 0;
            Complex* temp = new Complex[len];
            for (int j = i*len; j<(i+1)*len; j++) {
                if (iter < len/2) {
                    temp[iter] = h[j] + W[iter*N/len]*h[j+len/2];
                }else{
                    temp[iter] = h[j-len/2] + W[iter*N/len]*h[j];
                }
                iter++;
            }
            for (int j = 0; j<len; j++) {
                h[i*len+j] = temp[j];
            }
            delete[] temp;
        }
        len = len * 2;
        groupNum = groupNum/2;
    }
    for (int i = 0; i<width; i++) {
        h[i] = h[i] * Complex(1.0/width);
        if (h[i].Mag().real < 1e-10) {
            h[i] = Complex(0);
        }
    }
    delete []W;
}
void* Transform2DTHread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
    unsigned long ID = (unsigned long)v;
    int rowPerThread = ImageHeight/nThread;
    int startRow = (int)ID*rowPerThread;
    int offset = 0;
    for (int i = 0; i<rowPerThread; i++) {
        offset = (startRow + i)*ImageWidth;
        Transform1D(ImageData+offset, ImageWidth);
    }
    setBarri((int)ID);
    return 0;
}

void* Transform2DThreadInverse(void* v){
    // This is the thread startign point.  "v" is the thread number
    // Calculate 1d Inverse DFT for assigned rows
    // wait for all to complete
    // Calculate 1d Inverse DFT for assigned columns
    // Decrement active count and signal main if all complete
    unsigned long ID = (unsigned long)v;
    int rowPerThread = ImageHeight/nThread;
    int startRow = (int)ID*rowPerThread;
    int offset = 0;
    for (int i = 0; i<rowPerThread; i++) {
        offset = (startRow + i)*ImageWidth;
        Transform1DInverse(ImageData+offset, ImageWidth);
    }
    setBarri((int)ID);
    return 0;
}
void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  InputImage image(inputFN);  // Create the helper object for reading the image
  // Create the global pointer to the image array data
  // Create 16 threads
  // Wait for all threads complete
  // Write the transformed data
    ImageData = image.GetImageData();
    ImageWidth = image.GetWidth();
    ImageHeight = image.GetHeight();
    N = ImageWidth;
    
    pthread_mutex_init(&cntMutex, 0);
//    pthread_cond_init(&exitCondi, 0);
//    pthread_mutex_lock(&exitMutex);
    //create 16 threads
    for (int i = 0; i<nThread; i++) {
        pthread_t pthread;
        pthread_create(&pthread, 0, Transform2DTHread, (void*)(i));
    }
    //pthread_cond_wait(&exitCondi, &exitMutex);
    MyBarrier();
    cout<<"All threads have completed their task: FFT 1D"<<endl;
    string file("/Users/ashlie/Documents/GraduateStudy/2017Spring/ECE-6122/ECE6122_Project_2/ECE6122_Project_2/MyAfter1D.txt");
    image.SaveImageData(file.c_str(), ImageData, ImageWidth, ImageHeight);
    
    Transpose(ImageData, ImageWidth, ImageHeight);
   
    MyBarrier_Init();
    for (int i = 0; i<nThread; i++) {
        pthread_t pthread;
        pthread_create(&pthread, 0, Transform2DTHread, (void*)i);
    }
    MyBarrier();
    cout<<"All threads have completed their task: FFT 2D"<<endl;
    Transpose(ImageData, ImageHeight, ImageWidth);
    string fn1("/Users/ashlie/Documents/GraduateStudy/2017Spring/ECE-6122/ECE6122_Project_2/ECE6122_Project_2/MyAfter2D.txt");
    image.SaveImageData(fn1.c_str(), ImageData, ImageWidth, ImageHeight);
    cout<<"FFT 2D done!"<<endl;
    MyBarrier_Init();
    
    //2D Inverse FFT
    for (int i = 0; i<nThread; i++) {
        pthread_t pthread;
        pthread_create(&pthread, 0, Transform2DThreadInverse, (void*)i);
    }
    MyBarrier();
    cout<<"All threads have completed their task: Inverse FFT 1D"<<endl;
    Transpose(ImageData, ImageWidth, ImageHeight);
    MyBarrier_Init();
    for (int i = 0; i<nThread; i++) {
        pthread_t pthread;
        pthread_create(&pthread, 0, Transform2DThreadInverse, (void*)i);
    }
    MyBarrier();
    cout<<"All threads have completed their task: Inverse FFT 2D"<<endl;
    Transpose(ImageData, ImageWidth, ImageHeight);
    string fn2("/Users/ashlie/Documents/GraduateStudy/2017Spring/ECE-6122/ECE6122_Project_2/ECE6122_Project_2/MyAfterInverse.txt");
    image.SaveImageDataReal(fn2.c_str(), ImageData, ImageWidth, ImageHeight);
    cout<<"Inverse FFT done!"<<endl;
}

int main(int argc, char** argv)
{
  string fn("/Users/ashlie/Documents/GraduateStudy/2017Spring/ECE-6122/ECE6122_Project_2/ECE6122_Project_2/Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  Transform2D(fn.c_str()); // Perform the transform.
}  
  

  
