#ifndef CTESTER_H
#define CTESTER_H

#include <deque>
#include "Sample.hpp"

class ConvergenceTester { 
  
  deque<MatrixXd> oldVar; //deque to store several covariance matrices to test the Cauchy convergence
  double epsilon;//required relative precision
  unsigned dequeMaxSize;//to have a fixed-size deque
 
  
public:
  ConvergenceTester(double espilon = 1.e-3, unsigned dequeMaxSize = 2);//by default only use 2 levels 
  void feed(const MatrixXd& Var);//add another var from a sample to the deque and pop the oldest element
  void feed(const Sample& sample);//get the variance matrix from the sample
  double getPrecision() const;//return epsilon
  unsigned getMaxRange() const;//returns max size
  bool converges() const;//returns convergence
  void setMaxRange(unsigned newDequeMaxSize); //change the deque's max size
  void setPrecision(const double epsilon);//change epsilon's value

};

#endif