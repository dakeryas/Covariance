#ifndef CORRELATIONESTIMATOR_H
#define CORRELATIONESTIMATOR_H

#include <thread>
#include "VarianceEstimator.hpp"
#include "CovarianceEstimator.hpp"

template<class T>
class CorrelationEstimator: public CovarianceEstimator<T>{//Covariance needs Var(X) and Var(Y) to compute Corr(X,Y) from its Cov(X,Y)
  
  VarianceEstimator<T> varianceEstimator1;
  VarianceEstimator<T> varianceEstimator2;
  
public:
  CorrelationEstimator(const T& variable1, const T& variable2);//the variables need to implement a getRealisation() method
  Eigen::MatrixXd getCorrelationMatrix() const;//Corr(X,Y)
  const VarianceEstimator<T>& getVarianceEstimator1() const;//for Var(X)
  const VarianceEstimator<T>& getVarianceEstimator2() const;//for Var(Y)
  void estimate(double epsilon, unsigned cauchyNumber);//Mersenne-Twister generator to get the realisations, relative accuracy needed between the close matrices, number of close consecutive matrices needed

};

template <class T>
CorrelationEstimator<T>::CorrelationEstimator(const T& variable1, const T& variable2):CovarianceEstimator<T>(variable1, variable2),varianceEstimator1(variable1),varianceEstimator2(variable2){
  
}

template <class T>
Eigen::MatrixXd CorrelationEstimator<T>::getCorrelationMatrix() const{
  
  return CovarianceEstimator<T>::getCorrelationMatrix(varianceEstimator1.getCovarianceMatrix(), varianceEstimator2.getCovarianceMatrix());
  
}

template <class T>
const VarianceEstimator<T>& CorrelationEstimator<T>::getVarianceEstimator1() const{
  
  return varianceEstimator1;

}

template <class T>
const VarianceEstimator<T>& CorrelationEstimator<T>::getVarianceEstimator2() const{
  
  return varianceEstimator2;

}

template <class T>
void CorrelationEstimator<T>::estimate(double epsilon, unsigned cauchyNumber){

  std::thread thread1(&VarianceEstimator<T>::estimate, &varianceEstimator1, epsilon, cauchyNumber);
  std::thread thread2(&VarianceEstimator<T>::estimate, &varianceEstimator2, epsilon, cauchyNumber);
  CovarianceEstimator<T>::estimate(epsilon, cauchyNumber);
  thread1.join();
  thread2.join();
  
}

#endif