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

#define RAWFILEPATH "../4EnergyCal/"   //Path to the original ROOT file
#define RAWFILENAME "ana"                   //The file name of the original file


#define ROOTFILEPATH "./"  //The path to generate the ROOT file
#define ROOTFILENAME "ana"

const int nDSSD = 2;
const int nxstrips = 16;
const int nystrips = 16;
const int posfileparnum = 3;

const double m_alpha = 3727379.161754000000655651092529296875;

const double window = 100;  // ns

// for single spectra, deg
const double mintheta = 50;     // minimum central theta value
const double dtheta = 5;        // step of central theta value
const int ntheta = 17;          // total number of theta value
const double rangetheta = dtheta/2;  // theta range of each theta
const int enbins = 5000;

// for conversion to Center of Mass system
const double mp = 1.00782503190;
const double mt = 7.01600343568;
const double ma = 4.00260325413;
const double Ep = 1;     // MeV

// #define WAVEFORM
// #define ENERGYSUM
// #define QDCSUM
// #define EXTERNALTS


#define EVENTTIMEWINDOWSWIDTH  100   //ns



#endif /* _USERDEFINE_H_ */

// 
// UserDefine.h ends here
