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
  ipt->SetBranchAddress("event", &br_event);

  benchmark = new TBenchmark;
  gr = new TRandom3(0);
  
  // init TH1/TH2

    std::ifstream ifs(Form("../3Fit/fitparameter_%04d.txt",run));  // fit parameter input
    // fit parameter input
    for (int dssd=1; dssd<=nDSSD; dssd++)
        for (int par=0; par<3; par++){
            for (int strip=0; strip<nxstrips; strip++)
                ifs>>xpar[dssd][strip][par];
            for (int strip=0; strip<nystrips; strip++)
                ifs>>ypar[dssd][strip][par];
        }
    
    for (int dssd=1; dssd<=nDSSD; dssd++){
        heidx[dssd] = new TH2D(Form("heidx_dssd%d",dssd),Form("x energy spectra vs. id (dssd%d, nhit=1)",dssd), xbins, minfitrange, maxfitrange, nxstrips, 0, nxstrips);
        heidy[dssd] = new TH2D(Form("heidy_dssd%d",dssd),Form("y energy spectra vs. id (dssd%d, nhit=1)",dssd), ybins, minfitrange, maxfitrange, nystrips, 0, nystrips);
    }
}

void DataAnalysis::Loop(TTree *opt_, int run)
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
 
    int dssd;
      
    dssd = 1;
      
    for (int idet = 0; idet < int(vsifv.size()); idet++){
        vsifv[idet].e = xCalibrate(1, vsifv[idet].id, vsifv[idet].raw);
        //vsifv[idet].raw += gr->Uniform(0,1);
        //vsifv[idet].e = xpar[1][vsifv[idet].id][0] + xpar[1][vsifv[idet].id][1] * vsifv[idet].raw + xpar[1][vsifv[idet].id][2] * vsifv[idet].raw * vsifv[idet].raw;
    }
    
    for (int idet = 0; idet < int(vsifh.size()); idet++){
        vsifh[idet].e = yCalibrate(1, vsifh[idet].id, vsifh[idet].raw);
        //vsifh[idet].raw += gr->Uniform(0,1);
        //vsifh[idet].e = ypar[1][vsifh[idet].id][0] + ypar[1][vsifh[idet].id][1] * vsifh[idet].raw + ypar[1][vsifh[idet].id][2] * vsifh[idet].raw * vsifh[idet].raw; 
        vsifh[idet].ts += offset[dssd];
    }
    
    if ( vsifh.size() == 1 && vsifv.size() == 1 && vsifh.size() > 0 && vsifv.size() > 0 ){
        double xe = vsifv[0].e;
        double ye = vsifh[0].e;
        int xid   = vsifv[0].id;
        int yid   = vsifh[0].id;
          
        heidx[dssd]->Fill(xe, xid);
        heidy[dssd]->Fill(ye, yid);
    }
      
    dssd = 2;

    for (int idet = 0; idet < int(vsibv.size()); idet++){
        vsibv[idet].e = xCalibrate(2, vsibv[idet].id, vsibv[idet].raw);
        //vsibv[idet].raw += gr->Uniform(0,1);
        //vsibv[idet].e = xpar[2][vsibv[idet].id][0] + xpar[2][vsibv[idet].id][1] * vsibv[idet].raw + xpar[2][vsibv[idet].id][2] * vsibv[idet].raw * vsibv[idet].raw;
    }
    
    for (int idet = 0; idet < int(vsibh.size()); idet++){
        vsibh[idet].e = yCalibrate(2, vsibh[idet].id, vsibh[idet].raw);
        //vsibh[idet].raw += gr->Uniform(0,1);
        //vsibh[idet].e = ypar[2][vsibh[idet].id][0] + ypar[2][vsibh[idet].id][1] * vsibh[idet].raw + ypar[2][vsibh[idet].id][2] * vsibh[idet].raw * vsibh[idet].raw;
        vsibh[idet].ts += offset[dssd];
    }
    
    if ( vsibh.size() == 1 && vsibv.size() == 1 && vsibh.size() > 0 && vsibv.size() > 0 ){
        double xe = vsibv[0].e;
        double ye = vsibh[0].e;
        int xid   = vsibv[0].id;
        int yid   = vsibh[0].id;
          
        heidx[dssd]->Fill(xe, xid);
        heidy[dssd]->Fill(ye, yid);
    }

    if ( vsifh.size()>0 || vsifv.size()>0 || vsibh.size()>0 || vsibv.size()>0 ) 
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

  for (int dssd=1; dssd<=nDSSD; dssd++){
      heidx[dssd]->Write();
      heidy[dssd]->Write();
  }

  benchmark->Show("dataana");//计时结束并输出时间
}

void DataAnalysis::BranchOpt()
{
  br_event = 0;

  opt->Branch("vsifh", &vsifh);
  opt->Branch("vsifv", &vsifv);
  opt->Branch("vsibh", &vsibh);
  opt->Branch("vsibv", &vsibv);
}

double DataAnalysis::xCalibrate(int dssd, Short_t id, double raw)
{
    raw += gr->Uniform(0,1);
    return xpar[dssd][id][0] + xpar[dssd][id][1] * raw + xpar[dssd][id][2] * raw * raw;
}

double DataAnalysis::yCalibrate(int dssd, Short_t id, double raw)
{
    raw += gr->Uniform(0,1);
    return ypar[dssd][id][0] + ypar[dssd][id][1] * raw + ypar[dssd][id][2] * raw * raw; 
}

// 
// event.cc ends here
