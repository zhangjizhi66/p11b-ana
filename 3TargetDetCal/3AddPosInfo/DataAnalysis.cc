// event.cc --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 一 9月 21 16:28:37 2020 (+0800)
// Last-Updated: 五 9月 22 12:54:15 2023 (+0800)
//	     By: Hongyi Wu(吴鸿毅)
//     Update #: 48
// URL: http://wuhongyi.cn

#include "DataAnalysis.hh"

#include "UserDefine.hh"
#include "TMath.h"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DataAnalysis::Init()
{
  if(ipt == NULL) return;
  ipt->SetBranchAddress("vsifv", &br_vsifv);
  ipt->SetBranchAddress("vsifh", &br_vsifh);
  ipt->SetBranchAddress("vsibv", &br_vsibv);
  ipt->SetBranchAddress("vsibh", &br_vsibh);

  benchmark = new TBenchmark;
  gr = new TRandom3(0);
  
  // init TH1/TH2

    std::ifstream ifs("../2PixelPosCal/pixelpos.txt");  // pixel info
    for (int dssd=1; dssd<=nDSSD; dssd++)
        for (int xstrip = 0; xstrip < nxstrips; xstrip++)
            for (int ystrip = 0; ystrip < nystrips; ystrip++)
                for (int ipar = 0; ipar < posfileparnum; ipar++)
                    ifs>>pixelpos[dssd][xstrip][ystrip][ipar];
}

void DataAnalysis::Loop(TTree *opt_)
{
  if(opt_ == NULL) return;

  opt = opt_;    
  BranchOpt();
  Long64_t TotalEntry = ipt->GetEntries();

  benchmark->Start("dataana");//计时开始
    
  Long64_t startentry = 0, stopentry = TotalEntry;
    
  clock_t start = clock(), stop = clock();
  
  for(Long64_t jentry = startentry; jentry < stopentry; jentry++)
    {
      ipt->GetEntry(jentry);
      vsi.clear();

      //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
    for (int idet = 0; idet < int(std::min(br_vsifv->size(), br_vsifh->size())); idet++){
        event.dssd  = 1;
        event.xid   = (*br_vsifv)[idet].id;
        event.yid   = (*br_vsifh)[idet].id;
        event.ts    = (*br_vsifh)[idet].ts;
        
        event.e     = (*br_vsifh)[idet].e;
        
        event.theta = pixelpos[event.dssd][event.xid][event.yid][0];
        event.phi   = pixelpos[event.dssd][event.xid][event.yid][1];
        event.solid = pixelpos[event.dssd][event.xid][event.yid][2];
        
        vsi.push_back(event);
    }

    for (int idet = 0; idet < int(std::min(br_vsibv->size(), br_vsibh->size())); idet++){
        event.dssd  = 2;
        event.xid   = (*br_vsibv)[idet].id;
        event.yid   = (*br_vsibh)[idet].id;
        event.ts    = (*br_vsibh)[idet].ts;
        
        event.e     = (*br_vsibh)[idet].e;
        
        event.theta = pixelpos[event.dssd][event.xid][event.yid][0];
        event.phi   = pixelpos[event.dssd][event.xid][event.yid][1];
        event.solid = pixelpos[event.dssd][event.xid][event.yid][2];
        
        vsi.push_back(event);
    }

    opt->Fill();

	    // display progress and time needed
        if (jentry%100000 == 99999){
            stop = clock();
            printf("Process %.2f %%  Time remaining %02d min %02d s                                     \r",double(jentry-startentry)/double(stopentry-startentry)*100.,
                int((stop-start)*(stopentry-jentry)/(jentry-startentry)/1e6/60),
                int((stop-start)*(stopentry-jentry)/(jentry-startentry)/1e6)%60);
            fflush(stdout);
        }
    }// loop for entry
    
    stop = clock();
    printf("Process %.2f %%  Total Time %02d min %02d s        \n",100.,int((stop-start)/1e6/60),int((stop-start)/1e6)%60);


  // TH1/TH2 write

  benchmark->Show("dataana");//计时结束并输出时间
}

void DataAnalysis::BranchOpt()
{
  br_vsifh = 0;
  br_vsifv = 0;
  br_vsibh = 0;
  br_vsibv = 0;
    
  opt->Branch("vsi", &vsi);
}

// 
// event.cc ends here
