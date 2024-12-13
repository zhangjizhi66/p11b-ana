// DataAnalysis.hh --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 一 9月 21 16:28:28 2020 (+0800)
// Last-Updated: 五 9月 22 12:49:38 2023 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 25
// URL: http://wuhongyi.cn 

#ifndef _DATAANALYSIS_H_
#define _DATAANALYSIS_H_

#include "UserDefine.hh"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TCutG.h"
#include "TBenchmark.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef struct DATAEVENT
{
  Short_t      det;//探测器类型
  Short_t      id;//探测器编号

  UShort_t         fw;
  UShort_t         sr;
  Long64_t        ts;
  UShort_t        finets;
    
  UShort_t        energy;
  UShort_t        energyshort;
  
  double          e;
    
} DATAEVENT;

typedef struct dets
{
  Short_t det;
  Short_t id;
  Double_t e;
  Double_t raw;
  Long64_t ts;
  Double_t subts;
  Bool_t   pileup;
  Bool_t   outofr;

#ifdef WAVEFORM
  std::vector<float> wave;
#endif
  
  // add info
  
} dets;

class DataAnalysis
{
 public:
 
  DataAnalysis()
  {
    ipt = NULL;
    opt = NULL;
  }
  DataAnalysis(TTree *ipt_)
  {
    ipt = ipt_;
    Init();
  }

  void Init();
  void Loop(TTree *opt_, TFile *opf);
  void BranchOpt();

private:
  std::vector<DATAEVENT>* br_event;
  TRandom3 *gr;
  TBenchmark *benchmark;
  TTree *ipt;
  TTree *opt;

  // --------------------------------------------
  
  // 0 ref, 



  TCutG *psd[4];

  dets det;
  std::vector<dets> vsifh, vsifv, vsibh, vsibv;
    
  TH1I *hts[nDSSD+1];
    
  TH2I *hetsx[nDSSD+1][nxstrips][nystrips];
  TH2I *hetsy[nDSSD+1][nxstrips][nystrips];
    
  TH2I *hexid[nDSSD+1][nystrips];
  TH2I *heyid[nDSSD+1][nxstrips];
    
  TH2I *hexy[nDSSD+1][nxstrips][nystrips];
};


#endif /* _DATAANALYSIS_H_ */
// 
// DataAnalysis.hh ends here














