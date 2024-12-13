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
  void Loop(TTree *opt_);
  void BranchOpt();

private:
  std::vector<dets> *br_vsifh, *br_vsifv, *br_vsibh, *br_vsibv;
  TRandom3 *gr;
  TBenchmark *benchmark;
  TTree *ipt;
  TTree *opt;

  // --------------------------------------------
  
  // 0 ref, 

  TCutG *psd[4];

  dets det;
  std::vector<dets> vsifh, vsifv, vsibh, vsibv;
    
  TH1D *he[nDSSD+1][nxstrips][nystrips];
    
  TH2D *hstat[nDSSD+1];
};


#endif /* _DATAANALYSIS_H_ */
// 
// DataAnalysis.hh ends here

