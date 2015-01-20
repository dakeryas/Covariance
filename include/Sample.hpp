#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include <iostream>
#include <cmath>
#include <Eigen/Core>
#include <TH1.h>
#include <TFile.h>
#include <TMatrixD.h>

using namespace::std;
using namespace::Eigen;

class Sample { 

protected:
  unsigned n;//number of iterations for the process (when with a sample, n =1)
  VectorXd x; //to store the current event n, namely the values at the nth iteration of all its m variables
  VectorXd mean; //the mean vector value of all the variables after having had a look at n samples
  MatrixXd product; //after nth iterations, the mean value of the product x(i) x(j) where x(i) is the ith component of the random vector variable 
  MatrixXd var; //the variance matrix
  
  void ReadFromStd(const vector< double >& xstd);
  void ReadFromHist(const TH1D &h);
  void UpdateMean();//update the mean from the current x values
  void UpdateProd();//add x times the transpose of x to the current value of prod
  void UpdateVar();//updates the whole var matrix using prod and mean
  void UpdateAll();//updates all the values in the former order and increases n afterward
  unsigned NbGreaterThan(const double epsilon = 1e-6) const;//counts and returns the number of coefficients greater than epsilon in 'mean'
  
public:
  Sample();
  Sample(const VectorXd& x);
  Sample(vector<double>& xstd);
  Sample(const TH1D& h);
  Sample& operator +=(const Sample& add);
  void Update(const vector< double >& xstdnth);//add another sample, increase n, and update the mean, mean_product, and var
  void Update(const VectorXd& xnth);//add another sample, increase n, and update the mean, mean_product, and var
  void Update(const TH1D& h);//add another sample from the values of the vector of bin contents, increase n, and update the mean, mean_product, and var
  unsigned GetIteration() const;//returns n
  VectorXd GetMean() const;//returns mean
  MatrixXd GetVar() const;//returns var
  MatrixXd GetCorrMatrix() const;//returns the Sample matrix computed from var
  VectorXd GetErrors() const;//return the square root of the diagonal of var
  VectorXd GetRelErrors() const;//returns the square root of the diagonal of var divided by the mean values
  TH1D GetMeanSpectrum(double min, double max) const;//we need a range for mean, so give min and max and by default you will have a constant binning with mean.size() bins between min and max, 
  TH1D GetMeanSpectrum(const vector<double> rebin) const;//you can also rebin it
  void SaveToRoot(const char* outname, const TH1D& h);
  void SaveToRoot(const char* outname, const vector<double> edges, const vector<double> width);//rebin with the edges and the binwidth[k] between edge[k] and edge[k+1]

};

ostream& operator<<(ostream& output, const Sample& corr);
Sample operator+(Sample s1, const Sample& s2);

#endif