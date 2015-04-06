#include "FileReader.hpp"

namespace FileReader{
  
  namespace{
    
    bool itemMatches(TObject* obj, const std::string& className){

      return std::string(obj->ClassName()).find(className) != std::string::npos;
      
    }
    
    void readAsHist(Hist& hist, TObject* obj){

      if(itemMatches(obj, "TH1D")){
	
	hist = *dynamic_cast<TH1D*>(obj); //if "GetClassName" contains TH1, the function finds shouldn't return "std::string::npos"
	delete obj;
	
      }
      else if(itemMatches(obj, "TH1F")){
	
	TH1D htemp;
	dynamic_cast<TH1F*>(obj)->Copy(htemp);//copy the TH1F into a TH1D
	hist = htemp;
	delete obj;
	
      }

    }
    
    void readFromCan(Hist& hist, TObject* readObject){//reads a key to retrieve a TH1 from a canvas and store it in h

      TCanvas* can = dynamic_cast<TCanvas*>(readObject);//if one of the 'items' is a canvas, we store the pointer to it in can
      TObject* obj; //then we can loop over the objects of this canvas
      
      TIter objit(can->GetListOfPrimitives()); //iterator over the contents of the canvas
      while((obj = objit())) readAsHist(hist, obj);
      
    }
    
  }
  
  Hist read(const boost::filesystem::path& filePath){
  
    TH1::AddDirectory(false);//otherwise ROOT leads to segmentation violations
    Hist hist;

    if(boost::filesystem::is_regular_file(filePath)){
      
      TFile file(filePath.string().c_str());
      
      TKey* currentKey; //to point to the 'items' in a root file
      TObject* readObject;//to flush the key into a TObject
      
      TIter keyit(file.GetListOfKeys()); //iterator over the contents of filePath
      while((currentKey = dynamic_cast<TKey*>(keyit()))){ //returns the pointer and increment it, when the pointer is not allocated it returns zero so the loop ends
	
	readObject = currentKey->ReadObj();//this is silly to cast a TObject to a TKey to eventually return a TObject with the ReadObj method, but heh, this is ROOT !
	if(itemMatches(readObject, "TH1")) readAsHist(hist, readObject);//reads a key to retrieve a TH1 and store it in h
	else if(itemMatches(readObject, "TCanvas")) readFromCan(hist, readObject);//reads a key to retrieve a TH1 from a canvas and store it in h
	
      };
      
    }
    else std::cout<<filePath.string()<<" is not a regular file\n";
    
    return hist;

  }
  
}
