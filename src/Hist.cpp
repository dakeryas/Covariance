# include "Hist.hpp"

std::ostream& operator<<(std::ostream& output, const Hist& h){
  
  output<<h.GetName()<<"\n";
  for(unsigned k = 0; k<h.getDimension(); ++k) output<<"["<<std::setw(6)<<std::internal<<h.GetXaxis()->GetBinLowEdge(k+1)<<", "<<std::setw(6)<<std::internal<<h.GetXaxis()->GetBinUpEdge(k+1)<<"]"<<std::setw(8)<<std::left<<" "
    <<"-->"<<std::setw(8)<<std::left<<" "<<std::setw(12)<<std::left<<h.GetBinContent(k+1)
    <<"+/-"<<std::setw(2)<<std::left<<" "<<std::setw(12)<<std::left<<h.GetBinError(k+1)<<"\n";
  return output;
  
}

Hist operator+(Hist h1, const Hist& h2){
  
  h1 += h2;
  return h1;

}

void normalise(Hist& h, double newArea){
  
  double integral = h.Integral();
  if(integral != 0) h.Scale(newArea/integral);

}

Hist::Hist(const std::string& name, const std::string& title, unsigned dimension, double firstEdge, double lastEdge):TH1D(name.c_str(), title.c_str(), dimension, firstEdge, lastEdge){  

  fillEdge();
  
}

Hist::Hist(const TH1D& h):TH1D(h){
  
  fillEdge();
  
}

Hist& Hist::operator+=(const Hist& other){
  
  if(isCompatibleWith(other)){
    
    Add(&other);
    SetName((GetName() + std::string("_+_") + other.GetName()).c_str());
    
  }
  else if(isEmpty()) *this = other;//if the histogram was empty use the assignment operator
    
  return *this;

}

Hist& Hist::operator*=(double a){
  
  this->Scale(a);
  SetName((GetName() + std::string("_x_")+ std::to_string(a)).c_str());
  return *this;

}

void Hist::fillEdge(){//turn an array into a vector
   
  int n = GetNbinsX();
  double edgeArray[n];
  GetLowEdge(edgeArray);
  edge = std::vector<double>(edgeArray, edgeArray + n);
  edge.push_back(GetBinLowEdge(n+1));//the low edge of the overflow bin should be the upper edge of the previous bin
  
}

unsigned Hist::getDimension() const{
  
  return edge.size()-1;//we have one bin less than edges
  
}

const std::vector<double>& Hist::getEdge() const{

  return edge;
  
}

double Hist::getFirstEdge() const{
  
  return edge.front();

}

double Hist::getLastEdge() const{
  
  return edge.back();

}

const double* Hist::data() const{
  
  return GetArray() + 1;

}

void Hist::setBinContents(const Eigen::VectorXd& binContents){
  
  if(binContents.size() == getDimension()){
    
    for(unsigned k = 0; k < getDimension(); ++k) SetBinContent(k, binContents(k));
    
  }

}

void Hist::setZero(){
  
  setBinContents(Eigen::VectorXd::Zero(getDimension()));

}

void Hist::setErrors(const Eigen::VectorXd& errors){
  
  if(errors.size() == getDimension()){
    
    for(unsigned k = 0; k < getDimension(); ++k) SetBinError(k, errors(k));
    
  }

}

void Hist::setErrorsFrom(const Eigen::MatrixXd& binsCovarianceMatrix){
  
  setErrors(binsCovarianceMatrix.diagonal().array().sqrt().matrix());

}

bool Hist::isCompatibleWith(const Hist& other) const{
  
  return edge == other.edge;

}

bool Hist::isEmpty() const{
  
  return getDimension() > 0;

}
