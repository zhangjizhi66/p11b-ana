// main.cc --- 
// 
// Description: 
// Author: Hongyi Wu(吴鸿毅)
// Email: wuhongyi@qq.com 
// Created: 一 9月 21 20:06:21 2020 (+0800)
// Last-Updated: 一 1月 16 11:43:06 2023 (+0800)
//           By: Hongyi Wu(吴鸿毅)
//     Update #: 6
// URL: http://wuhongyi.cn 

#include "UserDefine.hh"

#include "TFile.h"
#include "TMath.h"
#include "TString.h"
#include "TTree.h"
#include "TGraph.h"
#include "TF1.h"
#include "TH1.h"
#include "TCut.h"
#include "TSpectrum.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <map>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const char gVERSION[] = "Version: Fit - 2023.01.16";

double par[nDSSD+1][2];   // xpar[dssd][strip][fitparameter], dssd=0 is not used

std::ofstream ofs_par;                 // fit parameter output

int main(int argc, char* argv[])
{
  std::cout<<gVERSION<<std::endl;
  std::cout<<"Copyright @ Hongyi Wu (wuhongyi@qq.com/wuhongyi@pku.edu.cn)"<<std::endl;

  if(argc != 2)
    {
      std::cout << "eg. " << argv[0] << "  [run number]" <<std::endl;
      return 1;
    }

  TString RunNumber(argv[1]);
  int run = RunNumber.Atoi();


  TFile *ipf = new TFile(TString::Format("%s%s_%04d_W%d.root", RAWFILEPATH, RAWFILENAME, run, EVENTTIMEWINDOWSWIDTH).Data());
  TTree *tree = (TTree*)ipf->Get("tree");
    
  TFile *opf = new TFile(TString::Format("%s%s_%04d_W%d.root", ROOTFILEPATH, ROOTFILENAME, run, EVENTTIMEWINDOWSWIDTH).Data(), "RECREATE");

  int nbin_e0[nDSSD+1] =  {0, 50, 50};
  int rl_e0[nDSSD+1] = {0, 1350, 1000};
  int rr_e0[nDSSD+1] = {0, 1550, 1100};
    
  int nbin_e1[nDSSD+1] =  {0, 50, 100};
  int rl_e1[nDSSD+1] = {0, 4000, 3500};
  int rr_e1[nDSSD+1] = {0, 5000, 4500};
    
  TH1D *h[nDSSD+1][npeaks];
  for (int dssd = 1; dssd <= nDSSD; dssd++){
      tree->Draw(Form("vsi.e>>h_dssd%d_e0(%d,%d,%d)",dssd,nbin_e0[dssd],rl_e0[dssd],rr_e0[dssd]), Form("vsi.dssd==%d && vsi.xid==15 && vsi.yid==15",dssd));
      h[dssd][0] = (TH1D*)gROOT->FindObject(Form("h_dssd%d_e0",dssd));
      tree->Draw(Form("vsi.e>>h_dssd%d_e1(%d,%d,%d)",dssd,nbin_e1[dssd],rl_e1[dssd],rr_e1[dssd]), Form("vsi.dssd==%d && vsi.xid==15 && vsi.yid==15",dssd));
      h[dssd][1] = (TH1D*)gROOT->FindObject(Form("h_dssd%d_e1",dssd));
      
      TGraph *gr = new TGraph;
      
      for (int ipeak = 0; ipeak < npeaks; ipeak++){
          TSpectrum s(500);
          int nfound = s.Search(h[dssd][ipeak], 2, "", 0.5);
          double *xpeaks = s.GetPositionX();
          h[dssd][ipeak]->Write();
          
          double maxpeak = -1;
          for (int j = 0; j < nfound; j++)
              if ( xpeaks[j] > maxpeak || maxpeak == -1 )
                  maxpeak = xpeaks[j];
          
          gr->SetPoint(ipeak, maxpeak, e0[dssd][ipeak]);
      }
      
      gr->Fit("pol1", "Q");
      gr->Write();
      
      TF1 *f1 = gr->GetFunction("pol1");
      par[dssd][0] = f1->GetParameter(0);
      par[dssd][1] = f1->GetParameter(1);
  }
    
  opf->Close();
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  ofs_par.open("fitparameter.txt", std::ios::out);                 // fit parameter output
    
  // fit parameter output
  ofs_par<<std::setprecision(parameterprecision);
  for (int dssd=1; dssd<=nDSSD; dssd++)
      for (int ipar=0; ipar<2; ipar++){
          ofs_par<<par[dssd][ipar]<<'\n';
      }
  
  return 0;
}

// 
// main.cc ends here
