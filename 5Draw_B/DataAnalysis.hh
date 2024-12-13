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
#include <TVector3.h>
#include "TLorentzVector.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef struct events
{
    int dssd;
    int xid;
    int yid;
    
    Long64_t ts;
    
    Double_t e;
    Double_t theta;
    Double_t phi;
} events;

class DataAnalysis
{
 public:
 
  DataAnalysis()
  {
    ipt = NULL;
    opt = NULL;
  }
  DataAnalysis(TTree *ipt_, int run_)
  {
    ipt = ipt_;
    run = run_;
    Init();
  }

  void Init();
  void Loop(TTree *opt_);
  void BranchOpt();
    
  int run;

private:
  std::vector<events> *br_vsi;
  TRandom3 *gr;
  TBenchmark *benchmark;
  TTree *ipt;
  TTree *opt;

  // --------------------------------------------
  
  // 0 ref, 

  TCutG *psd[4];

  events event;
  std::vector<events> vsi;
    
  double pixelpos[nDSSD+1][nxstrips][nystrips][posfileparnum];
    
  Double_t GetInvMass(Int_t npar, Double_t *m0, Double_t *Ek, TVector3 *direction,Double_t Eth);

  // Draw
    
  TH2I *hxe,*hye,*hxraw,*hyraw,*hmraw,*hvraw,*hbraw,*hgraw;
  TH2I *hdtx,*hdty;

  double e[3];
  double esum;
  TVector3 d[3];
  double m[3];
  
  TH2I *h1, *h1_16;
  TH1I *hcos;
  TH1I *hs, *hs16, *hs1, *hs2, *hs3;
  TH1I *hsbg;
  TH1I *ha1, *ha2, *ha3;
  TH2I *hsp;

  TH2I *hdeq;
  TH2I *ha1ha2, *ha1ha3, *ha2ha3;
    
  TH2I *hthetaphi;
  //TH1I *hstheta[nDSSD+1][ntheta];
  TH1I *hstheta[ntheta];
  TH1I *hspixel[nDSSD+1][nxstrips][nystrips];
};


#endif /* _DATAANALYSIS_H_ */
// 
// DataAnalysis.hh ends here

