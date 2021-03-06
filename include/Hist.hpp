#ifndef HIST_H
#define HIST_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <Eigen/Core>
#include <TH1.h>

class Hist: public TH1D{ 

  std::vector<double> edge;
  void fillEdge();

public:
  Hist() = default;
  Hist(const std::string& name, const std::string& title, unsigned dimension, double firstEdge, double lastEdge);
  Hist(const TH1D& h);
  Hist(const Hist& other) = default;
  ~ Hist() = default;
  Hist& operator=(const Hist& other) = default;
  Hist& operator+=(const Hist& other);
  Hist& operator*=(double a);
  unsigned getDimension() const;//returns the number of bins
  const std::vector<double>& getEdge() const;
  double getFirstEdge() const;//returns the low edge of the first bin
  double getLastEdge() const;//returns the upper edge of the last bin
  const double* data() const;//return the start of the actual bin contents (skip the underflow)
  void setZero();//set all bin contents to zero
  void setBinContents(const Eigen::VectorXd& binContents);
  void setErrors(const Eigen::VectorXd& errors);
  void setErrorsFrom(const Eigen::MatrixXd& binsCovarianceMatrix);//sets the errors to the diagonal of the square root 
  bool isCompatibleWith(const Hist& other) const; //checks if the binnings match
  bool isEmpty() const;//detects whether the edge has at least two elements
  
};

Hist operator+(Hist h1, const Hist& h2);
std::ostream& operator<<(std::ostream& output, const Hist& hist);
void normalise(Hist& h, double newArea = 1);//normalise 'h' to 'newArea'

#endif