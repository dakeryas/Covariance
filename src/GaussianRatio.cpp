#include "include/GaussianRatio.hpp"

GaussianRatio::GaussianRatio(double mean, double standardDeviation):distribution(mean,standardDeviation){

}

std::unique_ptr<Ratio> GaussianRatio::clone() const{
  
  return std::unique_ptr<Ratio>(new GaussianRatio(*this));

}

double GaussianRatio::getMean() const{
  
  return distribution.mean();

}

double GaussianRatio::getStandardDeviation() const{
  
  return distribution.stddev();

}

double GaussianRatio::getRealisation(){
  
  double realisation = distribution(randomGenerator);
  while(realisation < 0) realisation = distribution(randomGenerator);//we don't want the gaussian's tails that produce negative ratios
  return realisation;
  
}


