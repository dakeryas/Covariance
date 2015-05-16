#ifndef VARIANCEESTIMAR_H
#define VARIANCEESTIMAR_H

#include "ConvergenceTester.hpp"

template<class T>
class VarianceEstimator{ 
  
  std::mt19937 randomGenerator;
  unsigned n;//number of iterations for the process (when with a sample, n =1)
  T variable;//variables whose Covariance matrix need be estimated
  Eigen::VectorXd mean; //sample mean of variable after having had a look at n samples
  Eigen::MatrixXd product; //after nth iterations, the mean value of the product x*y.transpose()
  Eigen::MatrixXd var; //covariance matrix
  void addSample();//pick new realisations of variable and update the covariance matrix with it
  
public:
  VarianceEstimator(const T& variable);
  unsigned getNumberOfIterations() const;
  const T& getVariable() const;
  const Eigen::VectorXd& getMean() const;
  Eigen::MatrixXd getCorrelationMatrix() const;
  const Eigen::MatrixXd& getCovarianceMatrix() const;
  void estimate(double epsilon, unsigned cauchyNumber);//relative accuracy needed between the close matrices, number of close consecutive matrices needed
  void addSlopeMatrix(double slope);//add to 'var' a 'sigma * sigma.transpose()' matrix where 'sigma(i) = slope * mean(i) * (i+.5)'
  
};

template <class T>
std::ostream& operator<<(std::ostream& output, const VarianceEstimator<T>& covarianceEstimator){
  
  output<<"Number of iterations = "<<covarianceEstimator.getNumberOfIterations()<<"\n"
    <<"Covariance Matrix = "<<"\n"<<covarianceEstimator.getCovarianceMatrix()<<"\n"
    <<"Mean = "<<"\n"<<covarianceEstimator.getMean()<<"\n";
  return output;
  
}

template <class T>
VarianceEstimator<T>::VarianceEstimator(const T& variable):randomGenerator(std::random_device()()),n(0),variable(variable),mean(variable.getDimensionOfRealisations()),product(mean.size(),mean.size()),var(product.rows(),product.cols()){

  mean.setZero();
  product.setZero();
  var.setZero();
  
}

template <class T>
unsigned VarianceEstimator<T>::getNumberOfIterations() const{
  
  return n;

}

template <class T>
const T& VarianceEstimator<T>::getVariable() const{
  
  return variable;

}

template <class T>
const Eigen::VectorXd& VarianceEstimator<T>::getMean() const{
  
  return mean;

}

template<class T>
Eigen::MatrixXd VarianceEstimator<T>::getCorrelationMatrix() const{
  
  unsigned numberPositive = (var.diagonal().array() > 0).count();
  Eigen::MatrixXd inverseErrors = var.diagonal().head(numberPositive).array().sqrt().inverse().matrix().asDiagonal();
  return inverseErrors * var.topLeftCorner(numberPositive, numberPositive) * inverseErrors;
  
}

template <class T>
const Eigen::MatrixXd& VarianceEstimator<T>::getCovarianceMatrix() const{
  
  return var;

}

template <class T>
void VarianceEstimator<T>::addSample(){
  
  const double nd = n;
  
  auto realisation = variable.getRealisation(randomGenerator);
  auto x = Eigen::Map<const Eigen::VectorXd>(realisation.data(), realisation.getDimension());
  
  mean = x/(nd+1)+mean*nd/(nd+1);
  
  product +=x*x.transpose();
  
  if(n > 0) var = (1/nd)*product - ((nd+1)/nd)*mean*mean.transpose();
  
  n += 1;

}

template <class T>
void VarianceEstimator<T>::estimate(double epsilon, unsigned cauchyNumber){

  ConvergenceTester<VarianceEstimator<T>> convergenceTester(epsilon, cauchyNumber);
  
  while(!convergenceTester.converges()){
    
    addSample();
    convergenceTester.feed(*this);

  }
  
}

template <class T>
void VarianceEstimator<T>::addSlopeMatrix(double slope){
  
  Eigen::VectorXd sigma(mean.size());
  for(unsigned k = 0; k < sigma.size(); ++k) sigma(k) = slope * mean(k) * (k + .5);
  var += sigma * sigma.transpose();

}

#endif