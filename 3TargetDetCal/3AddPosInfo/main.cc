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

#include "DataAnalysis.hh"
#include "UserDefine.hh"

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TString.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const char gVERSION[] = "Version: DataAnalysis - 2023.01.16";

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


  TChain *ipt = new TChain("tree");
  ipt->Add(TString::Format("%s%s_%04d_W%d.root",RAWFILEPATH, RAWFILENAME, run, EVENTTIMEWINDOWSWIDTH).Data()); 

  auto opf = new TFile(TString::Format("%s%s_%04d_W%d.root", ROOTFILEPATH, ROOTFILENAME, run, EVENTTIMEWINDOWSWIDTH).Data(), "RECREATE");
  auto opt = new TTree("tree", "tree");

  
  auto it = new DataAnalysis(ipt);
  it->Loop(opt);

  opt->Write();
  opf->Close();
    
  std::cout<<std::endl<<"Run "<<run<<" done!"<<std::endl<<std::endl;
  
  return 0;
}

// 
// main.cc ends here
