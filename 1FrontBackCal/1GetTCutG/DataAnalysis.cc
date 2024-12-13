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
  ipt->SetBranchAddress("event", &br_event);

  benchmark = new TBenchmark;
  gr = new TRandom3(0);
  
  // init TH1/TH2
    
  for (int dssd = 1; dssd <= nDSSD; dssd++){
      hall[dssd] = new TH2I(Form("hall_dssd%d",dssd),"",xbins,xmin,xmax,ybins,ymin,ymax);
  }
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
      // vref.clear();
      vsifh.clear();
      vsifv.clear();
      vsibh.clear();
      vsibv.clear();


      for(int i = 0; i < int(br_event->size()); i++) 
	{
	  int flag = (*br_event)[i].det;
	  det.det = (*br_event)[i].det;
	  det.id = (*br_event)[i].id;
	  det.raw = (*br_event)[i].energy;
	  //det.e = (*br_event)[i].e;
	  det.ts = (*br_event)[i].ts;
	  //det.pileup = (*br_event)[i].pileup;
	  //det.outofr = (*br_event)[i].outofr;

          /*
	  det.subts = 0.0;

	  if((*br_event)[i].sr == 250)
	    det.ts = 8*det.ts;
	  else if((*br_event)[i].sr == 100 || (*br_event)[i].sr == 500)
	    det.ts = 10*det.ts;

	  
	  if((*br_event)[i].sr == 500)
	    det.subts = ((*br_event)[i].cfds-1+(*br_event)[i].cfd/8192.0)*2.0;
	  else if((*br_event)[i].sr == 250)
	    det.subts = ((*br_event)[i].cfd/16384.0-(*br_event)[i].cfds)*4.0;
	  else if((*br_event)[i].sr == 100)
	    det.subts = ((*br_event)[i].cfd/32768.0)*10.0;
	    */
	    
#ifdef WAVEFORM
	det.wave.clear();
	if((*br_event)[i].ltra > 0) //!!!!!!
	  {
	    det.wave.assign((*br_event)[i].data.begin(),(*br_event)[i].data.end());
	    
	    // det.wave.size()    det.wave[i]
	    // 
	    
	  }
#endif	  
	  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

	  
	  if(flag == 0)
	    {
	      vsifh.push_back(det);
	    }
	  
	  if(flag == 1) 
	    {
	      vsifv.push_back(det);
	    }
	  
	  if(flag == 2)
	    {
	      vsibh.push_back(det);
	    }

	  if(flag == 3)
	    {
	      vsibv.push_back(det);
	    }


	}// br_event->size()

      //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    int dssd;
      
    if ( vsifh.size()==1 && vsifv.size()==1 ){
        dssd = 1;
        if ( abs(vsifv[0].ts - vsifh[0].ts - offset[dssd]) < window[dssd] ){
            hall[dssd]->Fill(vsifv[0].raw, vsifh[0].raw);
        }
    }
    
    if ( vsibh.size()==1 && vsibv.size()==1 ){
        dssd = 2;
        if ( abs(vsibv[0].ts - vsibh[0].ts - offset[dssd]) < window[dssd] ){
            hall[dssd]->Fill(vsibv[0].raw, vsibh[0].raw);
        }
    }



    //if ( vsifh.size()>0 || vsifv.size()>0 || vsibh.size()>0 || vsibv.size()>0 ) 
    //   opt->Fill();

        // display progress and time needed
        if (jentry%500000 == 499999){
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
    
  for (int dssd = 1; dssd <= nDSSD; dssd++){
      hall[dssd]->Write();
  }

  benchmark->Show("dataana");//计时结束并输出时间
}

void DataAnalysis::BranchOpt()
{
  br_event = 0;

  //opt->Branch("vsifh", &vsifh);
  //opt->Branch("vsifv", &vsifv);
  //opt->Branch("vsibh", &vsibh);
  //opt->Branch("vsibv", &vsibv);
}

// 
// event.cc ends here
