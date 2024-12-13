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
  ipt->SetBranchAddress("vsi", &br_vsi);

  benchmark = new TBenchmark;
  gr = new TRandom3(0);
  
  // init TH1/TH2

    std::ifstream ifs("../1GetPar/fitparameter.txt");  // fit parameter input
    // fit parameter input
    for (int dssd=1; dssd<=nDSSD; dssd++)
        for (int ipar=0; ipar<2; ipar++)
            ifs>>par[dssd][ipar];
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
      vsi = *br_vsi;

      //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
    for (int idet = 0; idet < int(br_vsi->size()); idet++)
        vsi[idet].e = Calibrate((*br_vsi)[idet].dssd, (*br_vsi)[idet].e);

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
  br_vsi = 0;
    
  opt->Branch("vsi", &vsi);
}

double DataAnalysis::Calibrate(int dssd, double e)
{
    //e += gr->Uniform(0,1);
    return par[dssd][0] + par[dssd][1] * e;
}

// 
// event.cc ends here
