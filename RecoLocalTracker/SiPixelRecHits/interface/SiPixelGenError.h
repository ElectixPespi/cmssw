//
//  SiPixelGenError.h (v1.00)
//
//  Object to contain Lorentz drift and error information for the Generic Algorithm
//
// Created by Morris Swartz on 1/10/2014.
//
//
 
// Build the template storage structure from several pieces 

#ifndef SiPixelGenError_h
#define SiPixelGenError_h 1


#include<vector>
#include<cassert>

#ifndef SI_PIXEL_TEMPLATE_STANDALONE
#include "CondFormats/SiPixelObjects/interface/SiPixelGenErrorDBObject.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "RecoLocalTracker/SiPixelRecHits/interface/PixelClusterType.h"
#else
#include "PixelClusterType.h"

#endif

struct SiPixelGenErrorEntry { //!< Basic template entry corresponding to a single set of track angles
  int runnum;              //!< number of pixelav run used to generate this entry 
  float cotalpha;          //!< cot(alpha) is proportional to cluster length in x and is basis of interpolation
  float cotbeta;           //!< cot(beta) is proportional to cluster length in y and is basis of interpolation
  float qavg;              //!< average cluster charge for this set of track angles (now includes threshold effects)
  float pixmax;            //!< maximum charge for individual pixels in cluster
  float dyone;             //!< mean offset/correction for one pixel y-clusters
  float syone;             //!< rms for one pixel y-clusters 
  float dxone;             //!< mean offset/correction for one pixel x-clusters
  float sxone;             //!< rms for one pixel x-clusters 
  float dytwo;             //!< mean offset/correction for one double-pixel y-clusters 
  float sytwo;             //!< rms for one double-pixel y-clusters 
  float dxtwo;             //!< mean offset/correction for one double-pixel x-clusters 
  float sxtwo;             //!< rms for one double-pixel x-clusters 
  float qmin;              //!< minimum cluster charge for valid hit (keeps 99.9% of simulated hits) 
  float qmin2;
  float yavggen[4];        //!< generic algorithm: average y-bias of reconstruction binned in 4 charge bins
  float yrmsgen[4];        //!< generic algorithm: average y-rms of reconstruction binned in 4 charge bins
  float xavggen[4];        //!< generic algorithm: average x-bias of reconstruction binned in 4 charge bins
  float xrmsgen[4];        //!< generic algorithm: average x-rms of reconstruction binned in 4 charge bins
} ;





struct SiPixelGenErrorHeader {           //!< template header structure
   int ID;                 //!< template ID number
   int NTy;                //!< number of Template y entries
   int NTyx;               //!< number of Template y-slices of x entries
   int NTxx;               //!< number of Template x-entries in each slice
   int Dtype;              //!< detector type (0=BPix, 1=FPix)
   float qscale;           //!< Charge scaling to match cmssw and pixelav
   float lorywidth;        //!< estimate of y-lorentz width for optimal resolution
   float lorxwidth;        //!< estimate of x-lorentz width for optimal resolution
   float lorybias;         //!< estimate of y-lorentz bias
   float lorxbias;         //!< estimate of x-lorentz bias
   float Vbias;            //!< detector bias potential in Volts
   float temperature;      //!< detector temperature in deg K
   float fluence;          //!< radiation fluence in n_eq/cm^2
   float s50;              //!< 1/2 of the multihit dcol threshold in electrons
   float ss50;             //!< 1/2 of the single hit dcol threshold in electrons
   char title[80];         //!< template title
   int templ_version;      //!< Version number of the template to ensure code compatibility
   float Bfield;           //!< Bfield in Tesla
   float fbin[3];          //!< The QBin definitions in Q_clus/Q_avg
   float xsize;            //!< pixel size (for future use in upgraded geometry)
   float ysize;            //!< pixel size (for future use in upgraded geometry)
   float zsize;            //!< pixel size (for future use in upgraded geometry)
} ;



struct SiPixelGenErrorStore { //!< template storage structure
  SiPixelGenErrorHeader head;
  float cotbetaY[60];
  float cotbetaX[5];
  float cotalphaX[29];
  SiPixelGenErrorEntry enty[60];     //!< 60 Barrel y templates spanning cluster lengths from 0px to +18px [28 entries for fpix]
  SiPixelGenErrorEntry entx[5][29];  //!< 29 Barrel x templates spanning cluster lengths from -6px (-1.125Rad) to +6px (+1.125Rad) in each of 5 slices [3x29 for fpix]
} ;


// ******************************************************************************************
//! \class SiPixelGenError 
//!
//!  A Generic Algorithm info management class.  SiPixelGenError contains thePixelTemp
//!  (a std::vector  of SiPixelGenErrorStore, each of which is a collection of many 
//!  SiPixelGenErrorEntries).  Each SiPixelGenErrorStore corresponds to a given detector 
//!  condition, and is valid for a range of runs.  We allow more than one Store since the 
//!  may change over time.
//!
//!  This class reads templates from files via pushfile() method.
//! 
//!  The main functionality of SiPixelGenError is qbin(), which produces algorithm info
//!  on the fly, given a specific track's alpha and beta.
//!
// ******************************************************************************************
class SiPixelGenError {
 public:
  SiPixelGenError(const std::vector< SiPixelGenErrorStore > & thePixelTemp) : thePixelTemp_(thePixelTemp) { id_current_ = -1; index_id_ = -1;} //!< Constructor for cases in which template store already exists
  
  static bool pushfile(int filenum, std::vector< SiPixelGenErrorStore > & thePixelTemp_);     // load the private store with info from the 
                                  // file with the index (int) filenum
								  
#ifndef SI_PIXEL_TEMPLATE_STANDALONE
  static bool pushfile(const SiPixelGenErrorDBObject& dbobject, std::vector< SiPixelGenErrorStore > & thePixelTemp_);     // load the private store with info from db
#endif

// initialize the binary search information;
  static void postInit(std::vector< SiPixelGenErrorStore > & thePixelTemp_);
  
	
// Interpolate input beta angle to estimate the average charge. return qbin flag for input cluster charge, and estimate y/x errors and biases for the Generic Algorithm.
  int qbin(int id, float cotalpha, float cotbeta, float locBz, float qclus,
           bool irradiationCorrection, PixelClusterType typey, PixelClusterType typex,  
           float& pixmx, float& sigmay, float& sigmax, float& deltay, float& deltax);
  // Overloaded method to provide only the LA parameters
  int qbin(int id);

  float lorywidth() {return lorywidth_;}                            //!< signed lorentz y-width (microns)
  float lorxwidth() {return lorxwidth_;}                            //!< signed lorentz x-width (microns)
  float lorybias() {return lorybias_;}                              //!< signed lorentz y-bias (microns)
  float lorxbias() {return lorxbias_;}                              //!< signed lorentz x-bias (microns)

  float fbin(int i) {
#ifndef SI_PIXEL_TEMPLATE_STANDALONE
      if(i < 0 || i > 2) {throw cms::Exception("DataCorrupt") << "SiPixelTemplate::chi2xminc2m called with illegal index = " << i << std::endl;}
#else
      assert(i>=0 && i<3);
#endif
  return fbin_[i];} //!< Return lower bound of Qbin definition
  float xsize() {return xsize_;}                                    //!< pixel x-size (microns)
  float ysize() {return ysize_;}                                    //!< pixel y-size (microns)
  float zsize() {return zsize_;}                                    //!< pixel z-size or thickness (microns)

 private:
  
  // Keep current template interpolaion parameters	
  
  int id_current_;           //!< current id
  int index_id_;             //!< current index
  
  
  // Keep results of last interpolation to return through member functions
  
  float lorxwidth_;         //!< Lorentz x-width
  float lorywidth_;         //!< Lorentz y-width (sign corrected for fpix frame)
  float lorxbias_;         //!< Lorentz x-width
  float lorybias_;         //!< Lorentz y-width (sign corrected for fpix frame)
  float fbin_[3];          //!< The QBin definitions in Q_clus/Q_avg
  float xsize_;             //!< Pixel x-size
  float ysize_;             //!< Pixel y-size
  float zsize_;             //!< Pixel z-size (thickness)

  
  // The actual template store is a std::vector container

  const std::vector< SiPixelGenErrorStore > & thePixelTemp_;
} ;


#endif
