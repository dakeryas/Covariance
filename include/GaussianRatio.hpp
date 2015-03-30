#ifndef GAUSSIAN_RATIO_H
#define GAUSSIAN_RATIO_H

#include "Ratio.hpp"

class GaussianRatio: public Ratio{ //ratio generated following a gaussian distribution
  
  std::normal_distribution<double> distribution;
  
public:
  GaussianRatio() = default;//(the mean is set to 0 and the deviation to 1)
  GaussianRatio(double mean, double standardDeviation);
  std::unique_ptr<Ratio> clone() const;//for polymorphism
  double getMean() const;
  double getStandardDeviation() const;
  double getRealisation(std::mt19937& randomGenerator);
  
};

#endif