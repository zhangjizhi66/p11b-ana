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
#include "TDirectoryFile.h"

#include "UserDefine.hh"
#include "TMath.h"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DataAnalysis::Init()
{
  if(ipt == NULL) return;
  ipt->SetBranchAddress("vsi", &br_vsi);

  benchmark = new TBenchmark;
  gr = new TRandom3(0);
    
  std::ifstream ifs("../3TargetDetCal/2PixelPosCal/pixelpos.txt");  // pixel info
    for (int dssd = 1; dssd <= nDSSD; dssd++)
        for (int xstrip = 0; xstrip < nxstrips; xstrip++)
            for (int ystrip = 0; ystrip < nystrips; ystrip++)
                for (int ipar = 0; ipar < posfileparnum; ipar++)
                    ifs>>pixelpos[dssd][xstrip][ystrip][ipar];
  
  // init TH1/TH2

  hs = new TH1I("hs","",2200,0,22000);
  hsbg = new TH1I("hsbg","",2200,0,22000);
  
  ha1 = new TH1I("ha1","",1000,0,10000);
  ha2 = new TH1I("ha2","",1000,0,10000);

  hcos = new TH1I("hcos","",180,0.0,180.0);
  // TH1I *hcos = new TH1I("hcos","",200,-1.0,1.0);
    
  hts = new TH1I("hts","",1000,-1000,1000);

  hdeq = new TH2I("hdeq","",500,0,20000,1000,-10000,10000);
  hinv_e = new TH2I("hinv_e","",2000,0,20000,1000,0,10000);

  ha1ha2 = new TH2I("ha1ha2","",500,0,10000,500,0,10000);
  ha1ha2_L = new TH2I("ha1ha2_L","",500,0,10000,500,0,10000);

  hthetaphi = new TH2I("hthetaphi","",250,-180,180,250,0,180);
    
  TDirectoryFile *dir_hstheta = new TDirectoryFile("hstheta","");
  dir_hstheta->cd();
  /*
  for (int dssd = 1; dssd <= nDSSD; dssd++)
      for (int itheta = 0; itheta < ntheta; itheta++)
          hsingle[dssd][itheta] = new TH1I(Form("hsingle_dssd%d_%.0fdeg",dssd,mintheta+dtheta*itheta),"",500,0,8000);
  */
  for (int itheta = 0; itheta < ntheta; itheta++){
      hstheta[itheta] = new TH1I(Form("hsingle_%.0fdeg",mintheta+dtheta*itheta),"",enbins,0,15000);
  }
    
  TDirectoryFile *dir_hspixel = new TDirectoryFile("hspixel","");
  TDirectoryFile *dir_hspixel_dssd[nDSSD+1];
  for (int dssd = 1; dssd <= nDSSD; dssd++){
      dir_hspixel_dssd[dssd] = new TDirectoryFile(Form("dssd%d",dssd),"","",dir_hspixel);
      dir_hspixel_dssd[dssd]->cd();
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          for (int ystrip = 0; ystrip < nystrips; ystrip++){
              hspixel[dssd][xstrip][ystrip] = new TH1I(Form("hspixel_d%d_x%d_y%d",dssd,xstrip,ystrip),"",enbins,0,15000);
              hspixel[dssd][xstrip][ystrip]->SetTitle(Form("theta = %.1fdeg",pixelpos[dssd][xstrip][ystrip][0]));
          }
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
      vsi = *br_vsi;
      
      // display progress and time needed
        if (jentry%100000 == 99999){
            stop = clock();
            printf("Process %.2f %%  Time remaining %02d min %02d s                                     \r",double(jentry-startentry)/double(stopentry-startentry)*100.,
                int((stop-start)*(stopentry-jentry)/(jentry-startentry)/1e6/60),
                int((stop-start)*(stopentry-jentry)/(jentry-startentry)/1e6)%60);
            fflush(stdout);
        }
      
      // pixel positions
      for (int idet = 0; idet < int(vsi.size()); idet++)
          hthetaphi->Fill(vsi[idet].phi, vsi[idet].theta);

      // single spectra
      for (int idet = 0; idet < int(vsi.size()); idet++)
          hspixel[vsi[idet].dssd][vsi[idet].xid][vsi[idet].yid]->Fill(vsi[idet].e, 1/pixelpos[vsi[idet].dssd][vsi[idet].xid][vsi[idet].yid][2]);
      
      for (int iv = 1; iv < pow(2, br_vsi->size()); iv++){  // loop of combination
          vsi.clear();
          int index = iv;
          for (int idet = 0; idet < int(br_vsi->size()); idet++){
              if (index%2)
                  vsi.push_back((*br_vsi)[idet]);
              index /= 2;
          }
          if ( vsi.size() != 2 ) continue;
        
          bool coincident = true;
          for (int idet = 0; idet < int(vsi.size()); idet++)
              for (int jdet = idet+1; jdet < int(vsi.size()); jdet++)
                  if ( abs(vsi[idet].ts - vsi[jdet].ts ) > window )
                      coincident = false;
          if ( !coincident ) continue;
          
          ha1ha2_L->Fill(vsi[1].e, vsi[0].e);

              //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

            esum = 0;
            for (int idet = 0; idet < int(vsi.size()); idet++) {
                m[idet] = m_alpha;
                e[idet] = vsi[idet].e;
                //esum += e[idet];
                d[idet].SetX(TMath::Sin(vsi[idet].theta/180.0*TMath::Pi())*TMath::Cos(vsi[idet].phi/180.0*TMath::Pi()));
                d[idet].SetY(TMath::Sin(vsi[idet].theta/180.0*TMath::Pi())*TMath::Sin(vsi[idet].phi/180.0*TMath::Pi()));
                d[idet].SetZ(TMath::Cos(vsi[idet].theta/180.0*TMath::Pi()));
            }

            double invmass = GetInvMass(2,m,e,d,2425*2-4942);
            hs->Fill(invmass);
            for (int idet = 0; idet < int(vsi.size()); idet++)
                hinv_e->Fill(invmass, vsi[idet].e);
        /*
            // center of mass system
            TLorentzVector a1(TMath::Sqrt(2*m[0]*e[0])*d[0],m[0]+e[0]);
            TLorentzVector a2(TMath::Sqrt(2*m[1]*e[1])*d[1],m[1]+e[1]);
            TLorentzVector all = a1+a2;

            a1.Boost(-all.BoostVector()); e[0] = a1.P()*a1.P()/m[0]/2.0;
            a2.Boost(-all.BoostVector()); e[1] = a2.P()*a2.P()/m[1]/2.0;
        */
            for (int idet = 0; idet < int(vsi.size()); idet++)
                e[idet] = e[idet] - 2*sqrt(ma*e[idet]*mp*Ep)/(mp+mt) + ma*mp/(mp+mt)/(mp+mt)*Ep;

            esum = e[0]+e[1];

            hsbg->Fill(esum);
            //hcos->Fill(TMath::ACos(a1.Vect().Unit().Dot(TVector3(0.0,0.0,1.0)))/TMath::Pi()*180);
            // hcos->Fill(a1.Vect().Unit().Dot(TVector3(0.0,0.0,1.0)));

            ha1->Fill(e[0]);
            ha2->Fill(e[1]);

            hdeq->Fill(invmass,e[0]-e[1]);
            //hdeq->Fill(invmass,e[1]-e[0]);
            ha1ha2->Fill(e[1], e[0]);
      }

    //opt->Fill();

    }// loop for entry
    
    stop = clock();
    printf("Process %.2f %%  Total Time %02d min %02d s        \n",100.,int((stop-start)/1e6/60),int((stop-start)/1e6)%60);

  for (int itheta = 0; itheta < ntheta; itheta++){
      int nspec = 0;
      for (int dssd = 1; dssd <= nDSSD; dssd++)
          for (int xstrip = 0; xstrip < nxstrips; xstrip++)
              for (int ystrip = 0; ystrip < nystrips; ystrip++){
                  if ( pixelpos[dssd][xstrip][ystrip][0] - (mintheta+dtheta*itheta) < rangetheta && pixelpos[dssd][xstrip][ystrip][0] - (mintheta+dtheta*itheta) > -rangetheta ){
                      hstheta[itheta]->Add(hspixel[dssd][xstrip][ystrip]);
                      nspec++;
                  }
              }
      hstheta[itheta]->Scale(1/(double)nspec);
  }
    
  // TH1/TH2 write
    /*
  hs->Write();
  hsbg->Write();
  ha1->Write();
  ha2->Write();
  hcos->Write();
  hdeq->Write();
  ha1ha2->Write();

  hthetaphi->Write();
  
  for (int dssd = 1; dssd <= nDSSD; dssd++)
      for (int itheta = 0; itheta < ntheta; itheta++){
          hstheta[dssd][itheta]->Sumw2(0);
          hstheta[dssd][itheta]->Write();
      }
  */
  for (int itheta = 0; itheta < ntheta; itheta++){
      hstheta[itheta]->Sumw2(0);
      //hstheta[itheta]->Write();
  }
    
  for (int dssd = 1; dssd <= nDSSD; dssd++)
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          for (int ystrip = 0; ystrip < nystrips; ystrip++){
              hspixel[dssd][xstrip][ystrip]->Sumw2(0);
              //hspixel[dssd][xstrip][ystrip]->Write();
          }

  benchmark->Show("dataana");//计时结束并输出时间
}

void DataAnalysis::BranchOpt()
{
  br_vsi = 0;
    
  //opt->Branch("vsi", &vsi);
}

Double_t DataAnalysis::GetInvMass(Int_t npar, Double_t *m0, Double_t *Ek, TVector3 *direction,Double_t Eth)
{
    const Int_t nparticle = npar;

    Double_t E[nparticle];
    Double_t pmag[nparticle];
    Double_t theta[nparticle];
    Double_t phi[nparticle];
    TVector3 p[nparticle];

    Double_t Etot = 0;
    Double_t m0tot = 0;
    TVector3 ptot;      //By default all components are initialized to zero 
    Double_t minv = 0;
    Double_t Erel = 0;
    Double_t Eex = 0;
    Double_t ptotmag = 0;

    for (Int_t i = 0; i < nparticle; i++){
        E[i] = m0[i]+Ek[i];
        pmag[i] = TMath::Sqrt(Ek[i]*Ek[i]+2*m0[i]*Ek[i]);
        theta[i] = direction[i].Theta();
        phi[i] = direction[i].Phi();
        p[i].SetMagThetaPhi(pmag[i],theta[i],phi[i]);

        Etot += E[i];
        ptot += p[i];
        m0tot += m0[i];
    }

    ptotmag = ptot.Mag();
    minv = TMath::Sqrt(Etot*Etot-ptotmag*ptotmag);
    Erel = minv - m0tot;
    Eex = Erel + Eth;

    return Eex;
}

// 
// event.cc ends here
