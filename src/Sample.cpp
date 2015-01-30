#include "Sample.hpp"

void setCosmetics(TH1D& h){
  
  h.SetLineWidth(2);
  h.SetXTitle("Visible Energy [MeV]");
  string binwidth = to_string(1e3*h.GetBinWidth(1));
  size_t decimalPosition = binwidth.find(".");
  if(decimalPosition != string::npos) binwidth.erase(decimalPosition);
  string ytitle = "Events [per " + binwidth + " keV]";
  h.SetYTitle(ytitle.c_str());
  h.GetYaxis()->SetTitleOffset(1.3);
  
}

ostream& operator<<(ostream& output, const Sample& corr){//for input masses in MeV sets the file into GeV

  output<<"Number of iterations = "<<corr.GetIteration()<<"\n"
  <<"COV = "<<"\n"<<corr.GetVar()<<"\n"<<"\n"
  <<"CORR = "<<"\n"<<corr.GetCorrMatrix()<<"\n"<<"\n"
  <<"Relative errors = "<<"\n"<<corr.GetRelErrors()<<"\n"<<"\n";
  return output;
  
}

Sample operator+(Sample s1, const Sample& s2){
  
  s1+= s2;
  return s1;
  
}

vector<double> Bin(const vector<double>& edges, const vector<double>& width){//returns a variable size bining following the edges and the width between edge[k] and edge[k+1]
   
  vector<double> rebin;
  double content = edges.front();
  rebin.push_back(content);
  unsigned nbin;
  
  for(unsigned k = 0; k<width.size(); ++k){//loop over all ranges
    
    nbin = static_cast<unsigned>(floor((edges[k+1]-edges[k])/width[k]));//get the number of bins in the range
    for(unsigned p = 0; p<nbin; ++p){//loop over the bins of a range
      
      content += width[k];
      rebin.push_back(content);
      
    }
    
  }
  
  return rebin;
  
}

Sample::Sample():n(0){
    
}

Sample::Sample(const VectorXd& x):n(1),x(x),mean(x), product(x.size(), x.size()), var(x.size(), x.size()){
 
  product = x*x.transpose();
  var.setZero();
  
}

Sample::Sample(vector<double>& xstd):n(1){
 
  ReadFromStd(xstd);
  mean = x;
  product.resize(x.size(), x.size());
  product = x*x.transpose();
  var.resize(x.size(), x.size());
  var.setZero();
  
}

Sample::Sample(const TH1D& h):n(1){
  
  ReadFromHist(h);
  mean = x;
  product.resize(x.size(), x.size());
  product = x*x.transpose();
  var.resize(x.size(), x.size());
  var.setZero();
  
}

Sample& Sample::operator+=(const Sample& add){
  
  if (add.n > n) n = add.n;//pick the max between (add.n and n)
  x = (x + add.x)*0.5;
  mean = (mean + add.mean)*0.5;
  product = (product + add.product)*0.5;
  var = var + add.var;
  
  return *this;

}

void Sample::ReadFromStd(const vector<double>& xstd){
  
  x = Map<const VectorXd>(xstd.data(), xstd.size());
  
} 

void Sample::ReadFromHist(const TH1D& h){
  
  x = Map<const VectorXd>(h.GetArray()+1, h.GetNbinsX());//the first bin of a TH1 (number 0) contains the underflow, so we have to shift the array by one
  
}

void Sample::UpdateMean(){
  
  const double nd = n;
  mean = x/(nd+1)+mean*nd/(nd+1);
  
}

void Sample::UpdateProd(){

  product +=x*x.transpose();

}

void Sample::UpdateVar(){
  
  const double nd = n;
  var = (1/nd)*product - ((nd+1)/nd)*mean*mean.transpose();

}

void Sample::UpdateAll(){
  
  UpdateMean();
  UpdateProd();
  UpdateVar();
  n+=1;
    
}

void Sample::Update(const vector<double>& xstdnth){//add another sample, increase n, and update the mean, mean_product, and var
   
  if (static_cast<int>(xstdnth.size()) == x.size()){
    
    ReadFromStd(xstdnth);
    UpdateAll();
    
  }
  
}

void Sample::Update(const VectorXd& xnth){//add another sample, increase n, and update the mean, mean_product, and var
   
  if (xnth.size() == x.size()){
    
    x = xnth;
    UpdateAll();
    
  }
  
}

void Sample::Update(const TH1D& h){//add another sample, increase n, and update the mean, mean_product, and var

  if (h.GetNbinsX() == x.size()){
    
    ReadFromHist(h);
    UpdateAll();
    
  }
  
}

unsigned Sample::GetIteration() const{
  
  return n;
  
}

VectorXd Sample::GetMean() const{
  
  return mean;
  
}
  
MatrixXd Sample::GetVar() const{
  
  return var;
  
}

unsigned Sample::NbGreaterThan(const double epsilon) const{//spectra should only store postive values (and we don't want the zeros in the following division !)
  
  return count_if(mean.data(), mean.data()+mean.size(), [&epsilon](double x){return x>epsilon;});
  
}

MatrixXd Sample::GetCorrMatrix() const{
  
  MatrixXd var_sqrtdia = var.diagonal().head(NbGreaterThan()).array().sqrt().inverse().matrix().asDiagonal();//purely diagonal matrix contaning the diagonal of var
  MatrixXd corr = var_sqrtdia*var.topLeftCorner(NbGreaterThan(), NbGreaterThan())*var_sqrtdia;
  return corr;
  
}

VectorXd Sample::GetErrors() const{
  
  VectorXd err = var.diagonal().array().sqrt();
  return err;
  
}

VectorXd Sample::GetRelErrors() const{
  
  VectorXd err = var.diagonal().head(NbGreaterThan()).array().sqrt()/mean.head(NbGreaterThan()).array();//select only the errors for which the value itself is positive
  return err;
  
}

TH1D Sample::GetMeanSpectrum(double min, double max) const{

  TH1D mean_spec("Mean_Spectrum", "Mean_Spectrum", mean.size(), min, max);
  mean_spec.SetContent(mean.data()-1);//we want the first component of mean.data() to be in the first bin, not in the underflow bin
  mean_spec.SetBinContent(0, 0);
  mean_spec.SetBinContent(mean.size()+1, 0);
  mean_spec.SetError(GetErrors().data()-1);
  
  setCosmetics(mean_spec);
  
  return mean_spec;
  
}

TH1D Sample::GetMeanSpectrum(const vector<double> rebin) const{
  
  TH1D mean_spec("Mean_Spectrum", "Mean_Spectrum", mean.size(), rebin.front(), rebin.back());
  mean_spec.SetContent(mean.data()-1);//we want the first component of mean.data() to be in the first bin, not in the underflow bin
  mean_spec.SetBinContent(0, 0);
  mean_spec.SetBinContent(mean.size()+1, 0);
  mean_spec.SetError(GetErrors().data()-1);
  
  double DCbins[rebin.size()];
  copy(rebin.begin(), rebin.end(), DCbins);
  mean_spec = *dynamic_cast<TH1D*>(mean_spec.Rebin(rebin.size()-1, "Mean_Spectrum", DCbins));//there is one interval less than edges
  setCosmetics(mean_spec);
  
  return mean_spec;
  
}

void Sample::SaveToRoot(const char* outname, const TH1D& h){

  TFile outfile(outname, "recreate");
  
  TMatrixD var_root(var.rows(), var.cols(), var.data());
  var_root.Write("Var");
  MatrixXd corr = GetCorrMatrix();
  TMatrixD corr_root(corr.rows(), corr.cols(), corr.data());//Eigen is Column Major and ROOT is Row Major but the matrices are symmetric...
  corr_root.Write("Corr");
 
  const double min = h.GetBinLowEdge(1);
  const double max = h.GetBinLowEdge(h.GetNbinsX()+1);
  GetMeanSpectrum(min, max).Write();
  
}

void Sample::SaveToRoot(const char* outname, const vector<double> edges, const vector<double> width){

  TFile outfile(outname, "recreate");
  
  TMatrixD var_root(var.rows(), var.cols(), var.data());
  var_root.Write("Var");
  MatrixXd corr = GetCorrMatrix();
  TMatrixD corr_root(corr.rows(), corr.cols(), corr.data());//Eigen is Column Major and ROOT is Row Major but the matrices are symmetric...
  corr_root.Write("Corr");
  GetMeanSpectrum(Bin(edges, width)).Write();
  
}
