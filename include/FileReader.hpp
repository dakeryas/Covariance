#ifndef FILEREADER_H
#define FILEREADER_H

#include <boost/filesystem.hpp>
#include "TCanvas.h"
#include "TFile.h"
#include "TKey.h"
#include "TIterator.h"
#include "Hist.hpp"

namespace FileReader{

  Hist read(const boost::filesystem::path& filePath);//store the first hist that can be found in filePath

};

#endif