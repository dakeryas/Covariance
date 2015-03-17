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
  CovarianceEstimator(const T& variable1, const T& variable2);//the variables need to implement a getRealisation() method
  CovarianceEstimator(const T& variable);//sets variable1 and variable2 to variable
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
CovarianceEstimator<T>::CovarianceEstimator(const T& variable1, const T& variable2):n(0),variable1(variable1),variable2(variable2),mean1(variable1.getDimensionOfRealisations()),mean2(variable2.getDimensionOfRealisations()),product(mean1.size(),mean2.size()),var(product.rows(),product.cols()){
  
  mean1.setZero();
  mean2.setZero();
  product.setZero();
  var.setZero();
  
}

template <class T>
CovarianceEstimator<T>::CovarianceEstimator(const T& variable):CovarianceEstimator(variable, variable){
  
}

template <class T>
unsigned CovarianceEstimator<T>::getNumberOfIterations() const{
  
  return n;

}

template <class T>
const T& CovarianceEstimator<T>::getVariable1() const{
  
  return variable1;

}

template <class T>
const T& CovarianceEstimator<T>::getVariable2() const{
  
  return variable2;

}

template <class T>
Eigen::VectorXd& CovarianceEstimator<T>::getMean1() const{
  
  return mean1;

}

template <class T>
Eigen::VectorXd& CovarianceEstimator<T>::getMean2() const{
  
  return mean2;

}

template <class T>
Eigen::MatrixXd CovarianceEstimator<T>::getCovarianceMatrix() const{
  
  return var;

}

template <class T>
void CovarianceEstimator<T>::addSample(){
  
  const double nd = n;
  
  auto realisation1 = variable1.getRealisation();
  auto realisation2 = variable2.getRealisation();
  auto x1 = Eigen::Map<const Eigen::VectorXd>(realisation1.data(), realisation1.getDimension());
  auto x2 = Eigen::Map<const Eigen::VectorXd>(realisation2.data(), realisation2.getDimension());
  
  mean1 = x1/(nd+1)+mean1*nd/(nd+1);
  mean2 = x2/(nd+1)+mean2*nd/(nd+1);
  
  product +=x1*x2.transpose();
  
  if(n > 0) var = (1/nd)*product - ((nd+1)/nd)*mean1*mean2.transpose();
  
  n += 1;

}

template <class T>
void CovarianceEstimator<T>::estimate(double epsilon, unsigned cauchyNumber){

  ConvergenceTester<CovarianceEstimator<T>> convergenceTester(epsilon, cauchyNumber);
  
  while(!convergenceTester.converges()){
    
    addSample();
    convergenceTester.feed(*this);
    
  }
  
}

#endif