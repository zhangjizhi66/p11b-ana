
#ifndef _USERDEFINE_H_
#define _USERDEFINE_H_

#define RAWFILEPATH "../1AuElasticStat/"   //Path to the original ROOT file
#define RAWFILENAME "ana"                   //The file name of the original file

#define SCATTERFILEPATH "../../../database_Rutherford/pAu_2MeV.txt"

#define ROOTFILEPATH "./"  //The path to generate the ROOT file
#define ROOTFILENAME "ana"

const int nDSSD = 2;
const int nxstrips = 16;
const int nystrips = 16;

const int xminfitrange = 0;
const int xmaxfitrange = 16;
const int yminfitrange = 0;
const int ymaxfitrange = 16;

const double pixel_lx = 3;  // mm
const double pixel_ly = 3;  // mm

// Fit
const double posparlimits = 0.001;
const double angparlimits = 0.001;

// output precision
const int precision = 5;

// In the following, dssd=0 is not used

// center position of DSSD
const double xx0[nDSSD+1] = {-10000, -54.59, 54.27};  // mm
const double yy0[nDSSD+1] = {-10000, 0.52, 3.65};  // mm   {-10000, -10, -8}
const double zz0[nDSSD+1] = {-10000, 24.25, -18.75};  // mm, along the beam

// normal vector of DSSD
const double dnx[nDSSD+1] = {-10000, 70.70, 70.97};
const double dny[nDSSD+1] = {-10000, -6.50, 2.00};
const double dnz[nDSSD+1] = {-10000, 0, 0};

// vector on the plane of DSSD, start from id=(0,0)
//along the x axis of DSSD
const double dux[nDSSD+1] = {-10000, 0, 0};
const double duy[nDSSD+1] = {-10000, 0, 0};
const double duz[nDSSD+1] = {-10000, -1, -1};
//along the y axis of DSSD
const double dvx[nDSSD+1] = {-10000, -6.50, 2.00};
const double dvy[nDSSD+1] = {-10000, -70.70, -70.97};
const double dvz[nDSSD+1] = {-10000, 0, 0};

#define EVENTTIMEWINDOWSWIDTH  100   //ns

#endif /* _USERDEFINE_H_ */
