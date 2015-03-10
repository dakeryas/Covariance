#ifndef UNIFORM_RATIO_H
#define UNIFORM_RATIO_H

#include "Ratio.hpp"

class UniformRatio: public Ratio{ //ratio generated following a uniform distribution
  
  std::uniform_real_distribution<double> distribution;
  
public:
  UniformRatio() = default;//(the bounds are set to zero and one)
  UniformRatio(double lowerBound, double upperBound);
  std::unique_ptr<Ratio> clone() const;//for polymorphism
  double getLowerBound() const;
  double getUpperBound() const;
  double getRealisation();
  
};

#endif