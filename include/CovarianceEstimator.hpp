#ifndef COVARIANCEESTIMATOR_H
#define COVARIANCEESTIMATOR_H

#include "ConvergenceTester.hpp"

template<class T>
class CovarianceEstimator{ 
  
  unsigned n;//number of iterations for the process (when with a sample, n =1)
  T variable1, variable2;//variables whose Covariance matrix need be estimated
  Eigen::VectorXd mean1; //sample mean of variable1 after having had a look at n samples
  Eigen::VectorXd mean2; //sample mean of variable1 after having had a look at n samples
  Eigen::MatrixXd product; //after nth iterations, the mean value of the product x*y.transpose()
  Eigen::MatrixXd var; //covariance matrix
  void addSamples();//pick new realisations of variable1 and variable2 and update the covariance matrix with it
  
public:
  CovarianceEstimator(const T& variable1, const T& variable2);
  void estimate(double epsilon, unsigned cauchyNumber);//relative accuracy needed between the close matrices, number of close consecutive matrices needed

};

#endif