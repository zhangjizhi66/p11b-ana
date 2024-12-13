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
#include "TDirectoryFile.h"
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
    
  Long64_t maxts = -1, mints = -1;
    
  for (Long64_t jentry = 0; jentry < ipt->GetEntries(); jentry++){
      ipt->GetEntry(jentry);
      if ( int(br_event->size()) > 0 ){
          if ((*br_event)[0].sr == 250)
              mints = (*br_event)[0].ts*8;
          else if ((*br_event)[0].sr == 100 || (*br_event)[0].sr == 500)
              mints = (*br_event)[0].ts*10;
          break;
      }
  }
  for (Long64_t jentry = ipt->GetEntries()-1; jentry >= 0; jentry--){
      ipt->GetEntry(jentry);
      if ( int(br_event->size()) > 0 ){
          if ((*br_event)[0].sr == 250)
              maxts = (*br_event)[0].ts*8;
          else if ((*br_event)[0].sr == 100 || (*br_event)[0].sr == 500)
              maxts = (*br_event)[0].ts*10;
          break;
      }
  }

  TDirectoryFile *dir_dssd[nDSSD+1];
  TDirectoryFile *dir_eid[nDSSD+1];
  TDirectoryFile *dir_ets[nDSSD+1];
  TDirectoryFile *dir_exy[nDSSD+1];
  TDirectoryFile *dir_ets_xid[nDSSD+1][nxstrips];
  TDirectoryFile *dir_ets_yid[nDSSD+1][nystrips];

  for (int dssd = 1; dssd <= nDSSD; dssd++){
      dir_dssd[dssd] = new TDirectoryFile(Form("dssd%d",dssd),"");
      dir_eid[dssd] = new TDirectoryFile(Form("heid_d%d",dssd),"","",dir_dssd[dssd]);
      dir_ets[dssd] = new TDirectoryFile(Form("hets_d%d",dssd),"","",dir_dssd[dssd]);
      dir_exy[dssd] = new TDirectoryFile(Form("hexy_d%d",dssd),"","",dir_dssd[dssd]);
  }
    
  for (int dssd = 1; dssd <= nDSSD; dssd++){
      dir_dssd[dssd]->cd();
      hts[dssd] = new TH1I(Form("hts_dssd%d",dssd),"",1000,-EVENTTIMEWINDOWSWIDTH,EVENTTIMEWINDOWSWIDTH);
      
      dir_eid[dssd]->cd();
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          heyid[dssd][xstrip] = new TH2I(Form("heyid_d%d_x%d",dssd,xstrip),"",nystrips,0,nystrips,ybins,ymin,ymax);
      for (int ystrip = 0; ystrip < nystrips; ystrip++)
          hexid[dssd][ystrip] = new TH2I(Form("hexid_d%d_y%d",dssd,ystrip),"",nxstrips,0,nxstrips,xbins,xmin,xmax);
      
      //dir_ets[dssd]->cd();
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          dir_ets_xid[dssd][xstrip] = new TDirectoryFile(Form("hets_d%d_x%d",dssd,xstrip),"","",dir_ets[dssd]);
      for (int ystrip = 0; ystrip < nystrips; ystrip++)
          dir_ets_yid[dssd][ystrip] = new TDirectoryFile(Form("hets_d%d_y%d",dssd,ystrip),"","",dir_ets[dssd]);
      
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          for (int ystrip = 0; ystrip < nystrips; ystrip++){
              
              dir_ets_yid[dssd][ystrip]->cd();
              hetsx[dssd][xstrip][ystrip] = new TH2I(Form("hetsx_d%d_x%d_y%d",dssd,xstrip,ystrip),"", tsbins, mints, 1.1*maxts, xbins, xmin, xmax);
              dir_ets_xid[dssd][xstrip]->cd();
              hetsy[dssd][xstrip][ystrip] = new TH2I(Form("hetsy_d%d_x%d_y%d",dssd,xstrip,ystrip),"", tsbins, mints, 1.1*maxts, ybins, ymin, ymax);
              
              dir_exy[dssd]->cd();
              hexy[dssd][xstrip][ystrip] = new TH2I(Form("hexy_d%d_x%d_y%d",dssd,xstrip,ystrip), "",xbins,xmin,xmax,ybins,ymin,ymax);
          }
  }
}

void DataAnalysis::Loop(TTree *opt_, TFile *opf)
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

	  
	  if(flag == 0) //ref for eam bursts
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
    

    if ( vsifh.size()==1 && vsifv.size()==1 ){
        int dssd = 1;
        hts[dssd]->Fill(vsifv[0].ts - vsifh[0].ts);
        hexid[dssd][vsifh[0].id]->Fill(vsifv[0].id, vsifv[0].raw);
        heyid[dssd][vsifv[0].id]->Fill(vsifh[0].id, vsifh[0].raw);
        hetsx[dssd][vsifv[0].id][vsifh[0].id]->Fill(vsifv[0].ts, vsifv[0].raw);
        hetsy[dssd][vsifv[0].id][vsifh[0].id]->Fill(vsifh[0].ts, vsifh[0].raw);
        hexy[dssd][vsifv[0].id][vsifh[0].id]->Fill(vsifv[0].raw, vsifh[0].raw);
    }
    
    if ( vsibh.size()==1 && vsibv.size()==1 ){
        int dssd = 2;
        hts[dssd]->Fill(vsibv[0].ts - vsibh[0].ts);
        hexid[dssd][vsibh[0].id]->Fill(vsibv[0].id, vsibv[0].raw);
        heyid[dssd][vsibv[0].id]->Fill(vsibh[0].id, vsibh[0].raw);
        hetsx[dssd][vsibv[0].id][vsibh[0].id]->Fill(vsibv[0].ts, vsibv[0].raw);
        hetsy[dssd][vsibv[0].id][vsibh[0].id]->Fill(vsibh[0].ts, vsibh[0].raw);
        hexy[dssd][vsibv[0].id][vsibh[0].id]->Fill(vsibv[0].raw, vsibh[0].raw);
    }
      

    //if ( vsifh.size()>0 || vsifv.size()>0 || vsibh.size()>0 || vsibv.size()>0 ) 
       //opt->Fill();

	   // display progress and time needed
        if (jentry%1000000 == 999999){
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
  br_event = 0;

  //opt->Branch("vsifh", &vsifh);
  //opt->Branch("vsifv", &vsifv);
  //opt->Branch("vsibh", &vsibh);
  //opt->Branch("vsibv", &vsibv);
}


// 
// event.cc ends here
