// Distributed two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;
void Transform2D(const char* inputFN);
void Transform1D(Complex* h, int w, Complex* H);
void InverseTransform1D(Complex* input, int w, Complex* output);
void Transpose(Complex* input, Complex* output, int width, int height);

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
  // 2) Use MPI to find how many CPUs in total, and which one
  //    this process is
  // 3) Allocate an array of Complex object of sufficient size to
  //    hold the 2d DFT results (size is width * height)
  // 4) Obtain a pointer to the Complex 1d array of input data
  // 5) Do the individual 1D transforms on the rows assigned to your CPU
  // 6) Send the resultant transformed values to the appropriate
  //    other processors for the next phase.
  // 6a) To send and receive columns, you might need a separate
  //     Complex array of the correct size.
  // 7) Receive messages from other processes to collect your columns
  // 8) When all columns received, do the 1D transforms on the columns
  // 9) Send final answers to CPU 0 (unless you are CPU 0)
  //   9a) If you are CPU 0, collect all values from other processors
  //       and print out with SaveImageData().
  InputImage image(inputFN);  // Create the helper object for reading the image
  // Step (1) in the comments is the line above.
  // Your code here, steps 2-9
  int width = image.GetWidth();
  int height = image.GetHeight();
  int size = width*height;

  int nCPU;
  int rank;
  int offset;

  MPI_Comm_size(MPI_COMM_WORLD, &nCPU);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status* status = new MPI_Status[nCPU];
  MPI_Status* request = new MPI_Status[nCPU];
  cout<<"total number CPU: "<<nCPU<<" Current CPU: "<< rank<<endl;
  int rowsPerCPU;
  int startingRow;

  //fft
  Complex* data = image.GetImageData();
  Complex* after1D = new Complex[size];
  Complex* transposeAfter1D = new Complex[size];
  Complex* after2D = new Complex[size];
  Complex* output = new Complex[size];
 
  //ifft
  Complex* InverseAfter1D = new Complex[size];
  Complex* InverseTransposeAfter1D = new Complex[size];
  Complex* InverseAfter2D = new Complex[size];
  rowsPerCPU = height/nCPU;
  startingRow = rank * rowsPerCPU;
  
  for(int i = 0; i<rowsPerCPU; i++){
      offset = (startingRow+i)*width;
      Transform1D(data+offset, width, after1D+i*width);
  }

  cout<<"CPU "<<rank<<" complete fft 1D"<<endl;

  if(rank == 0){
    for(int i = 1; i<nCPU; i++){
        offset = rowsPerCPU*i*width;
        MPI_Recv(after1D+offset,rowsPerCPU*width*sizeof(Complex),MPI_CHAR, i, 0, MPI_COMM_WORLD, &status[rank]);
        cout<<"CPU0 recieve CPU "<<i<<" 1D FFT row chunk"<<endl;
    }
    cout<<"CPU 0 receive all row chunks after fft1D"<< endl;
  }

  if(rank != 0){
    MPI_Send(after1D, rowsPerCPU*width*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    cout<<"CPU "<<rank<<" send fft1D row chunk to CPU0"<<endl;
  }

  if(rank == 0){
      cout<<"Creating Image File After1D.txt"<<endl;
      image.SaveImageData("MyAfter1D.txt", after1D,width, height);
  }
  
  rowsPerCPU = width/nCPU;
  startingRow = rank*rowsPerCPU;
  if(rank == 0){
      Transpose(after1D, transposeAfter1D, width, height);
      for(int i = 1; i<nCPU; i++){
          MPI_Send(transposeAfter1D, size*sizeof(Complex), MPI_CHAR, i, 0, MPI_COMM_WORLD);
      }
      cout<<"CPU 0 send transposeAfter1D to other CPUs"<< endl;
  }

  if(rank != 0){
      MPI_Recv(transposeAfter1D, size*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status[rank]);
  }

  for(int i = 0; i<rowsPerCPU; i++){
      offset = (startingRow+i)*height;
      Transform1D(transposeAfter1D+offset, height, after2D+i*height);
  }

  if(rank == 0){
      for(int i = 1; i<nCPU; i++){
          offset = rowsPerCPU*i*height;
          MPI_Recv(after2D+offset, rowsPerCPU*height*sizeof(Complex), MPI_CHAR, i,0, MPI_COMM_WORLD, &status[rank]);    
      }
      cout<<"CPU 0 receive all column chunk from other CPUs"<< endl;
  }
  if(rank != 0){
      MPI_Send(after2D, rowsPerCPU*height*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  }

  if(rank == 0){
      Transpose(after2D, output, height, width);
      cout<<"Generating and save Image File After2D.txt"<<endl;
      image.SaveImageData("MyAfter2D.txt", output, width, height);
  }

  /*******************************************************************
   * ****************************************************************/
  rowsPerCPU =  height/nCPU;
  startingRow = rank*rowsPerCPU;
  if(rank == 0){
      for(int i = 1; i<nCPU; i++){
        //offset = i*width*rowsPerCPU;
        MPI_Send(output, size*sizeof(Complex), MPI_CHAR, i, 0, MPI_COMM_WORLD);
      }
      cout<<"CPU 0 send output to all other CPUs"<<endl;
  }

  if(rank != 0){
      MPI_Recv(output, size*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status[rank]);
  }

  for(int i = 0; i<rowsPerCPU; i++){
      offset = (startingRow+i)*width;
      InverseTransform1D(output+offset, width, InverseAfter1D+i*width);
  }
  
  if(rank == 0){
      for(int i = 1; i<nCPU; i++){
          offset = rowsPerCPU*i*width;
          MPI_Recv(InverseAfter1D+offset, rowsPerCPU*width*sizeof(Complex), MPI_CHAR, i, 0, MPI_COMM_WORLD, &status[rank]);
      }
      cout<<"CPU 0 receive all row ifft 1D chunks from other CPUs"<<endl;
  }
  
  if(rank != 0){
      MPI_Send(InverseAfter1D, rowsPerCPU*width*sizeof(Complex), MPI_CHAR, 0,0,MPI_COMM_WORLD);
  }

  if(rank == 0){
      Transpose(InverseAfter1D, InverseTransposeAfter1D, width, height);
      cout<<"Transpose on after1DInverse"<<endl;
  }
  
  if(rank == 0){
      for(int i = 1; i<nCPU; i++){
          MPI_Send(InverseTransposeAfter1D, size*sizeof(Complex), MPI_CHAR, i, 0, MPI_COMM_WORLD);
      }
      cout<<"CPU 0 send transposed IFFT 1D to all other CPUs"<< endl;
  }

  if(rank != 0){
      MPI_Recv(InverseTransposeAfter1D, size*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status[rank]); }

  rowsPerCPU = width/nCPU;
  startingRow = rank*rowsPerCPU;
  for(int i = 0; i<rowsPerCPU; i++){
      offset = (startingRow+i)*height;
      InverseTransform1D(InverseTransposeAfter1D+offset, height, InverseAfter2D+i*height);
  }

  if(rank == 0){
      for(int i = 1; i<nCPU; i++){
          offset = rowsPerCPU*i*height;
          MPI_Recv(InverseAfter2D+offset, rowsPerCPU*height*sizeof(Complex), MPI_CHAR, i, 0, MPI_COMM_WORLD, &status[rank]);    
      }
      cout<<"CPU 0 receive all column chunks from other CPUs"<<endl;
  }
  if(rank != 0){
      MPI_Send(InverseAfter2D, rowsPerCPU*height*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  }

  if(rank == 0){
      Transpose(InverseAfter2D, output, height, width);
      cout<<"Creating Image File to MyAfterInverse.txt"<<endl;
      image.SaveImageDataReal("MyAfterInverse.txt", output, width, height);
  }
}

void Transform1D(Complex* h, int w, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  for(int n = 0; n<w; n++){
      Complex sum(0,0);
      for(int k = 0; k<w; k++){
          double theta = 2*M_PI*n*k/w;
          Complex wnk(cos(theta), -sin(theta));
          sum = sum + wnk*h[k];
      }
      H[n] = sum;
  }
}

void InverseTransform1D(Complex* input, int w, Complex* output){
    for(int n = 0; n<w; n++){
        Complex sum(0,0);
        for(int k = 0; k<w; k++){
            double theta = 2*M_PI*n*k/w;
            Complex wnk(cos(theta), sin(theta));
            sum = sum + wnk*input[k];
        }
        output[n].real = sum.real/w;
        output[n].imag = sum.imag/w;
    }
}

void Transpose(Complex* input, Complex* output, int width, int height){
    for(int i = 0;i<height;i++){
        for(int j = 0; j<width; j++){
            output[j*height+i] = input[i*width+j];
        }
    }
}
int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  int rc = MPI_Init(&argc, &argv);
  if(rc != MPI_SUCCESS){
      printf("Error");
      MPI_Abort(MPI_COMM_WORLD, rc);
  }
  Transform2D(fn.c_str()); // Perform the transform.
  // Finalize MPI here
  MPI_Finalize();
}  
  

  
