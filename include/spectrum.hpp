#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <vector>
#include <string>
#include <iostream>
#include <TFile.h>
#include <TKey.h>
#include <TObject.h>
#include <TDirectory.h>
#include <TIterator.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TRandom3.h>

#include <boost/filesystem.hpp>
using namespace boost::filesystem;
using namespace std;

class spectrum{ //class to spectrum paths(or more precisely the filename in the path of a directory entry)

  vector<TH1D> indiv_spectra; //individual spectra that serve to build up the res_spectrum weighed by the branching ratios r
  vector<double> r;//branching ratios of the indiv_spectra
  vector<double> rmean;//usually equal to r when built but while r may change for every new event with guassian ratios, rmean has to stay the same - may only be used with err set to 1
  bool err;//to know if errors on r are available
  vector<double> re;//if the error on r is known, use it to pick ratios following a guassian distribution of width re; this is the vector of relative errors
  vector<double> rmin;//if r is unknow, can be used to constrain the lower values of the components of r when picking them randomly
  vector<double> rmax;//if r is unknow, can be used to constrain the upper values of the components of r when picking them randomly
  TH1D res_spectrum;//resulting spectrum from adding the indiv_spectra weighed by the branching ratios r
  
  void StoreFromCan(TKey* key);//stores all histograms in a canvas into indiv_spectra
  void StoreFromTH1(TKey* key);//copies a histogram from a key into indiv_spectra
  void StoreFromPaths(const vector<path>& database_path);//store from files using the right methods if they contains cans or histograms
  double GetRatioSum();//sums the ratios and returns its value
  void NormaliseSpectra();//normalise all the indiv_spectra to one
  void FillRatios(const vector<double>& rc);// r = rc if rc is not empty
  void FillErrors(const vector<double>& rc, const vector<double>& rec);//re = rec if rec is not empty and it the size doesn't fit, complete it with zeros, we need rc to fill relative errors re[k]/rc[k]
  void DivideRatios(double s);//divides every component of r by s
  void PickRandomRatios(unsigned n = 0);
  void PickRatiosFromError();//uses re to pick guassian ratios for r
  void UpdateResSpectrum();//uses indiv_spectra to compute res_spectrum with r
  
public:
  spectrum(); //constructor, creates empty vectors
  spectrum(const vector<TH1D>& indiv_spectra, const vector<double> rc = vector<double> (0), const bool err = 0, const vector<double> re = vector<double> (0));//you don't need rmin and rmax in this case
  spectrum(const vector<spectrum*>& indiv_spectra, const vector<double> rc = vector<double> (0), const bool err = 0, const vector<double> re = vector<double> (0));
  spectrum(const path& database_path, const string& sorter = "", const vector<double> rc = vector<double> (0), const bool err = 0, const vector<double> re = vector<double> (0));//calls FillSpectra and if rc is not precised, initialises rmin and rmax to zeroes and ones
  spectrum(const path& database_path, const string& sorter, const vector<double>& rmin, const vector<double>& rmax);//calls FillSpectra 
  void FillSpectra(const vector<spectrum*>& new_indiv_spectra);//fills indiv_spectra with copies of the elements pointed at in new_indiv_spectra
  void FillSpectra(const path& database_path = path("."), const string& sorter = "");//filenames in a path and fills any TH1 in any canvas in them in indiv_spectra
  void UpdateFromHist(const vector<TH1D>& new_indiv_hist);
  void UpdateFromSpectra(const vector<spectrum*>& new_indiv_spectra);
  void NewEvent();//updates the ratios according to the value of err (either random within rmin and rmax or gaussian with re) and updates the res_spectrum
  void ConstrainRatios(const vector< double >& rminc, const vector< double >& rmaxc);
  const TH1D& GetResultingSpectrum();
  
};

#endif