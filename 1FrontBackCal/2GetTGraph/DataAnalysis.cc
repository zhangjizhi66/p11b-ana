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
#include "TROOT.h"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DataAnalysis::Init()
{
  if(ipt == NULL) return;
  ipt->SetBranchAddress("event", &br_event);

  benchmark = new TBenchmark;
  
  // init TH1/TH2

  for (int dssd=1; dssd<=nDSSD; dssd++){
    gROOT->Macro(Form("../1GetTCutG/d%dcut.C",dssd));
    cutg[dssd] = (TCutG*)gROOT->FindObject(Form("d%dcut",dssd));
    hcount[dssd] = new TH2I(Form("hcount_dssd%d",dssd),Form("correlated statistics between xstrip and ystrip for dssd%d",dssd),nxstrips,0,nxstrips,nystrips,0,nystrips);
  }
    
    for (int dssd=1; dssd<=nDSSD; dssd++)
        for (int xstrip=0; xstrip<nxstrips; xstrip++)
            for (int ystrip=0; ystrip<nystrips; ystrip++){
                grDSSD[dssd][xstrip][ystrip] = new TGraph;
                grDSSD[dssd][xstrip][ystrip]->SetName(Form("gr_dssd%d_x%d_y%d",dssd,xstrip,ystrip));
                grDSSD[dssd][xstrip][ystrip]->SetTitle(Form("graph dssd%d xstrip%d ystrip%d",dssd,xstrip,ystrip));
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
      
      if ( vsifh.size() == 1 && vsifv.size() == 1 ){
          /*
          if ( run == 5399 ){
              if ( vsifh[0].id ==  0 && vsifh[0].ts < 2.65e13 ) continue;
              if ( vsifv[0].id == 11 && vsifv[0].ts < 2.65e13 ) continue;
          }
          if ( run == 5400 ){
              if ( vsifh[0].id ==  0 && vsifh[0].ts < 2.80e13 ) continue;
          }
          */
          
          int dssd = 1;
          int xstrip = vsifv[0].id;
          int ystrip = vsifh[0].id;
          int xraw   = vsifv[0].raw;
          int yraw   = vsifh[0].raw;
          Long64_t xts = vsifv[0].ts;
          Long64_t yts = vsifh[0].ts;
          if ( run > 2100 && (xstrip==4 || xstrip==8 || xstrip==11) ){
              if ( abs(xts-yts-offset[dssd]) < window[dssd] )
                  grDSSD[dssd][xstrip][ystrip]->SetPoint(grDSSD[dssd][xstrip][ystrip]->GetN(), xraw, yraw);
          }
          else if ( cutg[dssd]->IsInside(xraw, yraw) && abs(xts-yts-offset[dssd]) < window[dssd] )
              grDSSD[dssd][xstrip][ystrip]->SetPoint(grDSSD[dssd][xstrip][ystrip]->GetN(), xraw, yraw);
      }
 
      if ( vsibh.size() == 1 && vsibv.size() == 1 ){
          /*
          if ( run == 5400 ){
              if ( vsibh[0].id ==  1 && vsibh[0].ts < 6.00e11 ) continue;
              if ( vsibv[0].id == 14 && vsibv[0].ts < 2.80e12 ) continue;
          }
          */
          
          int dssd = 2;
          int xstrip = vsibv[0].id;
          int ystrip = vsibh[0].id;
          int xraw   = vsibv[0].raw;
          int yraw   = vsibh[0].raw;
          Long64_t xts = vsibv[0].ts;
          Long64_t yts = vsibh[0].ts;
          //if ( xstrip == 14 && xts < 1.185e12 ) xraw -= 400;
          if ( cutg[dssd]->IsInside(xraw, yraw) && abs(xts-yts-offset[dssd]) < window[dssd] )
              grDSSD[dssd][xstrip][ystrip]->SetPoint(grDSSD[dssd][xstrip][ystrip]->GetN(), xraw, yraw);
      }


      // if(vref.size()>0 || vge.size()>0 || vlabr.size()>0 || vsi.size()>0) 
      // opt->Fill();

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

    for (int dssd=1; dssd<=nDSSD; dssd++){
        cutg[dssd]->Write();
        for (int xstrip=0; xstrip<nxstrips; xstrip++)
            for (int ystrip=0; ystrip<nystrips; ystrip++){
                hcount[dssd]->SetBinContent(xstrip+1, ystrip+1, grDSSD[dssd][xstrip][ystrip]->GetN());
                grDSSD[dssd][xstrip][ystrip]->Write();
            }
        hcount[dssd]->Write();
    }

  benchmark->Show("dataana");//计时结束并输出时间
}

void DataAnalysis::BranchOpt()
{
  br_event = 0;

  // opt->Branch("ref", &vref);
  // opt->Branch("ge", &ge);
  // opt->Branch("labr", &labr);
  // opt->Branch("si", &vsi);
}

// 
// event.cc ends here
