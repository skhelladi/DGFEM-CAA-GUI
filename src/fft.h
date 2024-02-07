#ifndef FFT_H_INCLUDED
#define FFT_H_INCLUDED


#include <complex>
#include <iostream>
#include <valarray>
#include <vector>
#include <fstream>

#include <eigen3/Eigen/Eigen>


using namespace std;
using namespace Eigen;


const double PI = 3.141592653589793238460;

typedef complex<double> Complex;
typedef valarray<Complex> CArray;

// Cooley–Tukey FFT (in-place, divide-and-conquer)
// Higher memory requirements and redundancy although more intuitive
void _fft(CArray& x);

// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
// !!! Warning : in some cases this code make result different from not optimased version above (need to fix bug)
// The bug is now fixed @2017/05/30
void fft(CArray& x);

// inverse fft (in-place)
void ifft(CArray& x);

int getNearestPowerOf2(int number);

//vector<vector<double> > FFT_spec(vector<vector<double> > usrData);
MatrixXd FFT_spec(MatrixXd usrData);


//int TestFFT(vector<vector<double> > usrData);
int WriteFFT(MatrixXd usrData, string filename);


#endif // FFT_H_INCLUDED
