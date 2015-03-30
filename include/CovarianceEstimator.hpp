#ifndef COVARIANCEESTIMATOR_H
#define COVARIANCEESTIMATOR_H

#include "ConvergenceTester.hpp"

template<class T>
class CovarianceEstimator{ 
  
  std::mt19937 randomGenerator1, randomGenerator2;//random generators used to get random values for variable1 and variable2, both are initialised with the same random seed
  unsigned n;//number of iterations for the process (when with a sample, n =1)
  T variable1, variable2;//variables whose Covariance matrix need be estimated
  Eigen::VectorXd mean1; //sample mean of variable1 after having had a look at n samples
  Eigen::VectorXd mean2; //sample mean of variable1 after having had a look at n samples
  Eigen::MatrixXd product; //after nth iterations, the mean value of the product x*y.transpose()
  Eigen::MatrixXd var; //covariance matrix
  void addSample();//pick new realisations of variable1 and variable2 with randomGenerator and update the covariance matrix with it
  
public:
  CovarianceEstimator(const T& variable1, const T& variable2);//the variables need to implement a getRealisation() method
  unsigned getNumberOfIterations() const;
  const T& getVariable1() const;
  const T& getVariable2() const;
  const Eigen::VectorXd& getMean1() const;
  const Eigen::VectorXd& getMean2() const;
  const Eigen::MatrixXd& getCovarianceMatrix() const;
  void estimate(double epsilon, unsigned cauchyNumber);//Mersenne-Twister generator to get the realisations, relative accuracy needed between the close matrices, number of close consecutive matrices needed

};

template <class T>
std::ostream& operator<<(std::ostream& output, const CovarianceEstimator<T>& covarianceEstimator){
  
  output<<"Number of iterations = "<<covarianceEstimator.getNumberOfIterations()<<"\n"
    <<"Covariance Matrix = "<<"\n"<<covarianceEstimator.getCovarianceMatrix()<<"\n"
    <<"Mean1 = "<<"\n"<<covarianceEstimator.getMean1()<<"\n"
    <<"Mean2 = "<<"\n"<<covarianceEstimator.getMean2()<<"\n";
  return output;
  
}

template <class T>
CovarianceEstimator<T>::CovarianceEstimator(const T& variable1, const T& variable2):randomGenerator1(std::random_device()()),randomGenerator2(randomGenerator1),n(0),variable1(variable1),variable2(variable2),mean1(variable1.getDimensionOfRealisations()),mean2(variable2.getDimensionOfRealisations()),product(mean1.size(),mean2.size()),var(product.rows(),product.cols()){

  mean1.setZero();
  mean2.setZero();
  product.setZero();
  var.setZero();
  
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
const Eigen::VectorXd& CovarianceEstimator<T>::getMean1() const{
  
  return mean1;

}

template <class T>
const Eigen::VectorXd& CovarianceEstimator<T>::getMean2() const{
  
  return mean2;

}

template <class T>
const Eigen::MatrixXd& CovarianceEstimator<T>::getCovarianceMatrix() const{
  
  return var;

}

template <class T>
void CovarianceEstimator<T>::addSample(){
  
  const double nd = n;
  
  auto realisation1 = variable1.getRealisation(randomGenerator1);
  auto realisation2 = variable2.getRealisation(randomGenerator2);
  auto x1 = Eigen::Map<const Eigen::VectorXd>(realisation1.data(), realisation1.getDimension());
  auto x2 = Eigen::Map<const Eigen::VectorXd>(realisation2.data(), realisation2.getDimension());
  
  mean1 = x1/(nd+1)+mean1*nd/(nd+1);
  mean2 = x2/(nd+1)+mean2*nd/(nd+1);
  
  product +=x1*x2.transpose();
  
  if(n > 0) var = (1/nd)*product - ((nd+1)/nd)*mean1*mean2.transpose();
  
  n += 1;

}

template <class T>
void CovarianceEstimator<T>::estimate(double epsilon, unsigned int cauchyNumber){

  ConvergenceTester<CovarianceEstimator<T>> convergenceTester(epsilon, cauchyNumber);
  
  while(!convergenceTester.converges()){
    
    addSample();
    convergenceTester.feed(*this);

  }
  
}

#endif