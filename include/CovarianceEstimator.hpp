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
  void addSample();//pick new realisations of variable1 and variable2 and update the covariance matrix with it
  
public:
  CovarianceEstimator(const T& variable1, const T& variable2);
  unsigned getNumberOfIterations() const;
  const T& getVariable1() const;
  const T& getVariable2() const;
  Eigen::VectorXd& getMean1() const;
  Eigen::VectorXd& getMean2() const;
  Eigen::MatrixXd getCovarianceMatrix() const;
  void estimate(double epsilon = 1e-3, unsigned cauchyNumber = 3);//relative accuracy needed between the close matrices, number of close consecutive matrices needed

};

template <class T>
std::ostream& operator<<(std::ostream& output, const CovarianceEstimator<T>& covarianceEstimator){
  
  output<<"Number of iterations = "<<covarianceEstimator.getNumberOfIterations()<<"\n"
    <<"Covariance Matrix = "<<"\n"<<covarianceEstimator.getCovarianceMatrix()<<"\n";
  return output;
  
}

template <class T>
CovarianceEstimator<T>::CovarianceEstimator(const T& variable1, const T& variable2):n(0),variable1(variable1),variable2(variable2),mean1(variable1.getDimension()),mean2(mean2.getDimension()),product(variable1.getDimension(),variable2.getDimension()),var(product.rows(),product.cols()){
  
  mean1.setZero();
  mean2.setZero();
  product.setZero();
  var.setZero();
  
}

template <class T>
unsigned CovarianceEstimator::getNumberOfIterations() const{
  
  return n;

}

template <class T>
const T& CovarianceEstimator::getVariable1() const{
  
  return variable1;

}

template <class T>
const T& CovarianceEstimator::getVariable2() const{
  
  return variable2;

}

template <class T>
Eigen::VectorXd& CovarianceEstimator::getMean1() const{
  
  return mean1;

}

template <class T>
Eigen::VectorXd& CovarianceEstimator::getMean2() const{
  
  return mean2;

}

template <class T>
Eigen::MatrixXd CovarianceEstimator::getCovarianceMatrix() const{
  
  return var;

}

template <class T>
void CovarianceEstimator::addSample(){
  
  const double nd = n;
  
  auto x1 = variable1.getRealisation();
  auto x2 = variable2.getRealisation();
  
  mean1 = x1/(nd+1)+mean1*nd/(nd+1);
  mean2 = x2/(nd+1)+mean2*nd/(nd+1);
  
  product +=x1*x2.transpose();
  
  if(n > 0) var = (1/nd)*product - ((nd+1)/nd)*mean1*mean2.transpose();
  
  n += 1;

}

template <class T>
void CovarianceEstimator::estimate(double epsilon, unsigned cauchyNumber){

  ConvergenceTester<T> convergenceTester(epsilon, cauchyNumber);
  
  while(!convergenceTester.converges()){
    
    addSample();
    convergenceTester.feed(*this);
    
  }
  
}

#endif