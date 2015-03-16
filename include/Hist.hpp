#ifndef HIST_H
#define HIST_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <Eigen/Core>
#include <TH1.h>

class Hist: public TH1D{ 

  std::vector<double> edge;
  void fillEdge();

public:
  Hist();
  Hist(const TH1D& h);
  Hist(const Hist& other);
  Hist& operator+=(const Hist& other);
  Hist& operator*=(double a);
  unsigned getDimension() const;//returns the number of bins
  const std::vector<double>& getEdge() const;
  const double* data() const;//return the start of the actual bin contents (skip the underflow)
  void setZero();//set all bin contents to zero
  void setErrors(const Eigen::VectorXd& errors);
  void setErrorsFrom(const Eigen::MatrixXd& binsCovarianceMatrix);//sets the errors to the diagonal of the square root 
  bool isCompatibleWith(const Hist& other) const; //checks if the binnings match 
  
};

Hist operator+(Hist h1, const Hist& h2);
std::ostream& operator<<(std::ostream& output, const Hist& hist);

#endif