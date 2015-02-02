#include "include/spectrum.hpp"

void FillPathsIf(const directory_iterator it, const string& name, vector<path>& found_paths){//compare the name of the content pointed by a directory iterator to a char, returns true if the char is containted in the path name
  
  if(is_directory(it->status()) == 0 && it->path().filename().string().find(name) != string::npos) found_paths.push_back(it->path()); ; //find returns the position of 'name' in the path, so if it returned a position indeed(namely not npos), we found something
  
}

void PathGrabber(const path& search_path, const string& fileparttern, vector<path>& found_paths){ //retrieves the path of all files in a directory
      
  directory_iterator end; //the default constructor creates an end iterator which cannot be reached unless nothing was found
  for(directory_iterator it(search_path); it!= end; ++it) FillPathsIf(it, fileparttern, found_paths);
  
}

spectrum::spectrum():err(0){// default class constructor
  
}

spectrum::spectrum(const vector<TH1D>& indiv_spectra, const vector<double> rc, const bool err, const vector<double> re):indiv_spectra(indiv_spectra), err(err){ //class constructor
  
  NormaliseSpectra();//after they have been filled, normalise the indiv_spectra
  FillRatios(rc);//Fill updates the size of indiv_spectra, so the size of r must change accordingly; adapts rc into r(normalize the sum of the ratios to 1)
  rmean = r;//once r has been filled, save it into rmean
  FillErrors(rc, re);//if there is an error vector that matches, copy it
  rmin = vector<double>(r.size(), 0);//if r = rc is known, you shouldn't need to use any bounds for r since you don't pick it randomly
  rmax = vector<double>(r.size(), 1);//if r = rc is known, you shouldn't need to use any bounds for r since you don't pick it randomly
  
  NewEvent();
  
}

spectrum::spectrum(const vector<spectrum*>& indiv_spectra, const vector<double> rc, const bool err, const vector<double> re):err(err){ //class constructor
  
  FillSpectra(indiv_spectra);
  
  FillRatios(rc);//Fill updates the size of indiv_spectra, so the size of r must change accordingly; adapts rc into r(normalize the sum of the ratios to 1)
  rmean = r;//once r has been filled, save it into rmean
  FillErrors(rc, re);//if there is an error vector that matches, copy it
  rmin = vector<double>(r.size(), 0);//if r = rc is known, you shouldn't need to use any bounds for r since you don't pick it randomly
  rmax = vector<double>(r.size(), 1);//if r = rc is known, you shouldn't need to use any bounds for r since you don't pick it randomly
  
  NewEvent();
  
}


spectrum::spectrum(const path& database_path, const string& sorter, const vector<double>& rminc, const vector<double>& rmaxc):err(0){
  
  FillSpectra(database_path, sorter);
  NormaliseSpectra();
  
  r.resize(indiv_spectra.size());//indiv_spectra should have been updated by FillSpectra
  if(rmin.size() == r.size()) rmin = rminc;
  else rmin = vector<double>(r.size(), 0);
  if(rmax.size() == r.size()) rmax = rmaxc;
  else rmax = vector<double>(r.size(), 1);
  
  NewEvent();
  
}

spectrum::spectrum(const path& database_path, const string& sorter, const vector<double> rc, const bool err, const vector<double> re):err(err){
  
  FillSpectra(database_path, sorter);
  NormaliseSpectra();
  
  FillRatios(rc);//Fill updates the size of indiv_spectra, so the size of r must change accordingly; adapts rc into r(normalize the sum of the ratios to 1)
  rmean = r;//once r has been filled, save it into rmean
  FillErrors(rc, re);//if there is an error vector that matches, copy it
  rmin = vector<double>(r.size(), 0);//if r = rc is known, you shouldn't need to use any bounds for r since you don't pick it randomly
  rmax = vector<double>(r.size(), 1);//if r = rc is known, you shouldn't need to use any bounds for r since you don't pick it randomly
  
  NewEvent();

}

void spectrum::StoreFromTH1(TKey* key){//reads a key to retrieve a TH1 and store it in h
  
  if(string(key->GetClassName()).find(string("TH1")) != string::npos) indiv_spectra.push_back(*dynamic_cast<TH1D*>(key->ReadObj())); //if "GetClassName" contains TH1, the function finds shouldn't return "string::npos"
  
}

void spectrum::StoreFromCan(TKey* key){//reads a key to retrieve a TH1 from a canvas and store it in h
  
  TCanvas* can = dynamic_cast<TCanvas*>(key->ReadObj());//if one of the 'items' is a canvas, we store the pointer to it in can
  TObject *can_obj; //then we can loop over the objects of this canvas
  
  TIter objit(can->GetListOfPrimitives()); //iterator over the contents of the canvas
  while((can_obj = objit())){
    
    if(string(can_obj->ClassName()).find(string("TH1"))!= string::npos) indiv_spectra.push_back(*dynamic_cast<TH1D*>(can_obj));
      
  }
  
}

void spectrum::StoreFromPaths(const vector<path>& database_path){ //store all TH1D histograms of all files into the vector h
  
  TKey *current_key; //to point to the 'items' in a root file
  TDirectory* CurrentDir = gDirectory->GetDirectory("");//shameful trick 
  
  for(unsigned k = 0; k<database_path.size(); ++k){
  
    TFile file(database_path[k].string().c_str());
    CurrentDir->cd(); //shameful trick, second part
    
    TIter keyit(file.GetListOfKeys()); //iterator over the contents of database_path[k]
    while((current_key =(TKey*)keyit())){ //returns the pointer and increment it, when the pointer is not allocated it returns zero so the loop ends
            
      StoreFromTH1(current_key);//reads a key to retrieve a TH1 and store it in h
      if(string(current_key->GetClassName()) == string("TCanvas")) StoreFromCan(current_key);//reads a key to retrieve a TH1 from a canvas and store it in h      

    }
    
  }
  
}

void spectrum::FillSpectra(const vector<spectrum*>& new_indiv_spectra){
   
  for(spectrum* s : new_indiv_spectra) indiv_spectra.push_back(s->GetResultingSpectrum());
    
}

void spectrum::FillSpectra(const path& database_path, const string& sorter){
  
  vector<path> found_paths;
  PathGrabber(database_path, sorter, found_paths);
  if(found_paths.empty()) cout<<"No files matching '"<<sorter<<"' were found in "<<database_path<<"."<<endl;
  sort(found_paths.begin(), found_paths.end());//this especially important to have a common ordering for the branching ratios
  cout<<"Using files:\n";
  for(const path& p : found_paths) cout<<p<<"\n";
  cout<<"for "<<sorter<<"."<<endl;
  StoreFromPaths(found_paths);
  
}

void spectrum::NormaliseSpectra(){
  
  for(TH1D& h : indiv_spectra) h.Scale(1/h.Integral());
  
}

void spectrum::UpdateFromHist(const vector<TH1D>& new_indiv_hist){
  
  indiv_spectra = new_indiv_hist;
  NewEvent();
  
}

void spectrum::UpdateFromSpectra(const vector<spectrum*>& new_indiv_spectra){
  
  if(new_indiv_spectra.size()>=indiv_spectra.size()){
    
    for(unsigned k = 0; k<indiv_spectra.size(); ++k) indiv_spectra[k] = new_indiv_spectra[k]->GetResultingSpectrum();
    NewEvent();
    
  }
  
}

double spectrum::GetRatioSum(){//sums the ratios and returns its value
  
  double s = 0;
  for(unsigned k = 0; k<r.size(); ++k) s+= r[k];
  return s;
  
}

void spectrum::FillRatios(const vector<double>& rc){
  
  if(rc.size() != 0){
    
    r = rc;
    if(rc.size() != indiv_spectra.size()) r.resize(indiv_spectra.size(), 0);//add zeroes if 'indiv_spectra' is larger or cut 'r' if 'r' is too large
  
  }
  else r.resize(indiv_spectra.size(), 1);//if rc.size == 0 add, fill r with ones
  DivideRatios(GetRatioSum());
  
}
 
void spectrum::FillErrors(const vector<double>& rc, const vector<double>& rec){
  
  re.resize(rc.size());
  
  if(rec.size()>=rc.size()) for(unsigned k = 0; k<re.size(); ++k) re[k] = rec[k]/rc[k];
  else{
    
    for(unsigned k = 0; k<rec.size(); ++k) re[k] = rec[k]/rc[k];
    for(unsigned k = rec.size(); k<rc.size(); ++k) re[k] = 0;//otherwise the errors are supposed to be zero percents
    
  }

}

void spectrum::DivideRatios(double s){
  
  for(unsigned k = 0; k<r.size(); ++k) r[k] = r[k]/s;
  
}

void spectrum::PickRandomRatios(unsigned n){
  
  TRandom3 ran(0);
  
  if(n != 0) r.resize(n);//r.size() should equal indiv_spectra.size() otherwise
  else if(n == 0 && r.size() == 0) r.resize(indiv_spectra.size());
  
  double s = 0;
  for(unsigned k = 0; k<r.size(); ++k){
    
    r[k] = ran.Uniform(rmin[k],rmax[k]);
    s+= r[k];
    
  }
  DivideRatios(s);

}

void spectrum::PickRatiosFromError(){
  
  TRandom3 ran(0);
  
  double s = 0;
  for(unsigned k = 0; k<r.size(); ++k){
    
    r[k] = ran.Gaus(rmean[k],rmean[k]*re[k]);
    while(r[k]<0 || r[k]>1) r[k] = ran.Gaus(rmean[k],rmean[k]*re[k]);
    s+= r[k];
    
  }
  DivideRatios(s);
  
}

void spectrum::UpdateResSpectrum(){

  if(!indiv_spectra.empty()){
    
    res_spectrum = indiv_spectra.front();
    
    if(indiv_spectra.size()>1 && r.empty()){
      
      for(unsigned k=1; k<indiv_spectra.size(); ++k) res_spectrum.Add(&indiv_spectra[k]);
      
    }
    else if(indiv_spectra.size()>1 && r.size()>1 && indiv_spectra.size()>= r.size()){
      
      res_spectrum.Scale(r.front());
      for(unsigned k=1; k<r.size(); ++k) res_spectrum.Add(&indiv_spectra[k], r[k]);
      
    }
      
  }

}

void spectrum::NewEvent(){

  if(err) PickRatiosFromError();//if rc is not even given we can't but pick random ratios
  else PickRandomRatios();//if rc is given and an error as well, pick random ratios following gaussian laws of widths = rec
  UpdateResSpectrum();
  
}

void spectrum::ConstrainRatios(const vector<double>& rminc, const vector<double>& rmaxc){
  
  rmin = rminc;
  rmax = rmaxc;
  
}

const TH1D& spectrum::GetResultingSpectrum(){

  return res_spectrum;
  
}