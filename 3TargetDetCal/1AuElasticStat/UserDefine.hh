// UserDefine.h --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 日 1月 10 21:11:47 2021 (+0800)
// Last-Updated: 五 9月 22 12:35:35 2023 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 12
// URL: http://wuhongyi.cn

#ifndef _USERDEFINE_H_
#define _USERDEFINE_H_

#define RAWFILEPATH "../../2AnaMultiHit/"   //Path to the original ROOT file
#define RAWFILENAME "ana"                   //The file name of the original file


#define ROOTFILEPATH "./"  //The path to generate the ROOT file
#define ROOTFILENAME "ana"

const int nDSSD = 2;
const int nxstrips = 16;
const int nystrips = 16;

// energy spectrum
const double emin = 0;
const double emax = 10000;
const int nbins = 5000;

// elastic scattering peak
const double epeak = 1000;   
const double ereso = 100;   // integral (epeak-ereso, epeak+ereso)

// #define WAVEFORM
// #define ENERGYSUM
// #define QDCSUM
// #define EXTERNALTS


#define EVENTTIMEWINDOWSWIDTH  100   //ns



#endif /* _USERDEFINE_H_ */

// 
// UserDefine.h ends here
