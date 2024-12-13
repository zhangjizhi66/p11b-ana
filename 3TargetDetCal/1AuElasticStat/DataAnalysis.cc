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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
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
   
  for (int dssd = 1; dssd <= nDSSD; dssd++)
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          for (int ystrip = 0; ystrip < nystrips; ystrip++)
              he[dssd][xstrip][ystrip] = new TH1D(Form("he_dssd%d_%d_%d",dssd,xstrip,ystrip),Form("energy spectra (dssd%d, x%d,y%d)",dssd,xstrip,ystrip), nbins, emin, emax);
    
  for (int dssd = 1; dssd <= nDSSD; dssd++)
      hstat[dssd] = new TH2D(Form("hstat_dssd%d",dssd),Form("statistic distribution (dssd%d)",dssd), nxstrips, 0, nxstrips, nystrips, 0, nystrips);
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
      vsifh = *br_vsifh;
      vsifv = *br_vsifv;
      vsibh = *br_vsibh;
      vsibv = *br_vsibv;

      //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    int dssd;
      
    dssd = 1;
    for (int idet = 0; idet < int(std::min(vsifv.size(), vsifh.size())); idet++)
        he[dssd][vsifv[idet].id][vsifh[idet].id]->Fill(vsifh[idet].e);

    dssd = 2;
    for (int idet = 0; idet < int(std::min(vsibv.size(), vsibh.size())); idet++)
        he[dssd][vsibv[idet].id][vsibh[idet].id]->Fill(vsibh[idet].e);


    //if ( vsifh.size()>0 || vsifv.size()>0 || vsibh.size()>0 || vsibv.size()>0 ) 
       //opt->Fill();

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

  for (int dssd = 1; dssd <= nDSSD; dssd++){
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          for (int ystrip = 0; ystrip < nystrips; ystrip++)
              hstat[dssd]->SetBinContent(xstrip+1, ystrip+1, he[dssd][xstrip][ystrip]->Integral(he[dssd][xstrip][ystrip]->FindBin(epeak-ereso), he[dssd][xstrip][ystrip]->FindBin(epeak+ereso)));
      hstat[dssd]->Write();
  }
    
  
  for (int dssd = 1; dssd <= nDSSD; dssd++)
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          for (int ystrip = 0; ystrip < nystrips; ystrip++)
              he[dssd][xstrip][ystrip]->Write();

  // TH1/TH2 write

  benchmark->Show("dataana");//计时结束并输出时间
}

void DataAnalysis::BranchOpt()
{
  br_vsifh = 0;
  br_vsifv = 0;
  br_vsibh = 0;
  br_vsibv = 0;
    
  //opt->Branch("vsifh", &vsifh);
  //opt->Branch("vsifv", &vsifv);
  //opt->Branch("vsibh", &vsibh);
  //opt->Branch("vsibv", &vsibv);
}


// 
// event.cc ends here
