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

#define RAWFILEPATH "../1FrontBackCal/"   //Path to the original ROOT file
#define RAWFILENAME "ana"                   //The file name of the original file

#define ROOTFILEPATH "./"  //The path to generate the ROOT file
#define ROOTFILENAME "ana"

//#define ALLSINGLE

const int nDSSD = 2;
const int nxstrips = 16;
const int nystrips = 16;

const double minfitrange = 0;
const double maxfitrange = 15000;
const int xbins = 1500;
const int ybins = 1500;

const double ereso[nDSSD+1] = {-10000, 60, 30};

// #define WAVEFORM
// #define ENERGYSUM
// #define QDCSUM
// #define EXTERNALTS


#define EVENTTIMEWINDOWSWIDTH  100   //ns



#endif /* _USERDEFINE_H_ */

// 
// UserDefine.h ends here
