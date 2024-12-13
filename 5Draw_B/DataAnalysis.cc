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

  hs = new TH1I("hs","",2000,0,20000);
  hs1 = new TH1I("hs1","",2000,0,20000);
  hs2 = new TH1I("hs2","",2000,0,20000);
  hs3 = new TH1I("hs3","",2000,0,20000);
  hsbg = new TH1I("hsbg","",2000,0,20000);
  hsp = new TH2I("hsp","",1000,0,20000,250,0,500);
  
  ha1 = new TH1I("ha1","",1000,0,10000);
  ha2 = new TH1I("ha2","",1000,0,10000);
  ha3 = new TH1I("ha3","",1000,0,10000);

  hcos = new TH1I("hcos","",180,0.0,180.0);
  // TH1I *hcos = new TH1I("hcos","",200,-1.0,1.0);
  h1 = new TH2I("h1","",120,-0.6,0.6,120,-0.6,0.6);
  h1_16 = new TH2I("h1_16","",120,-0.6,0.6,120,-0.6,0.6);

  hdeq = new TH2I("hdeq","",500,0,20000,1000,-8000,8000);

  ha1ha2 = new TH2I("ha1ha2","",500,0,10000,500,0,10000);
  ha1ha3 = new TH2I("ha1ha3","",500,0,10000,500,0,10000);
  ha2ha3 = new TH2I("ha2ha3","",500,0,10000,500,0,10000);

  hthetaphi = new TH2I("hthetaphi","",250,-180,180,250,0,180);
    
  TDirectoryFile *dir_hstheta = new TDirectoryFile("hstheta","");
  dir_hstheta->cd();
  /*
  for (int dssd = 1; dssd <= nDSSD; dssd++)
      for (int itheta = 0; itheta < ntheta; itheta++)
          hsingle[dssd][itheta] = new TH1I(Form("hsingle_dssd%d_%.0fdeg",dssd,mintheta+dtheta*itheta),"",500,0,8000);
  */
  for (int itheta = 0; itheta < ntheta; itheta++){
      hstheta[itheta] = new TH1I(Form("hsingle_%.0fdeg",mintheta+dtheta*itheta),"",enbins,0,10000);
  }
    
  TDirectoryFile *dir_hspixel = new TDirectoryFile("hspixel","");
  TDirectoryFile *dir_hspixel_dssd[nDSSD+1];
  for (int dssd = 1; dssd <= nDSSD; dssd++){
      dir_hspixel_dssd[dssd] = new TDirectoryFile(Form("dssd%d",dssd),"","",dir_hspixel);
      dir_hspixel_dssd[dssd]->cd();
      for (int xstrip = 0; xstrip < nxstrips; xstrip++)
          for (int ystrip = 0; ystrip < nystrips; ystrip++){
              hspixel[dssd][xstrip][ystrip] = new TH1I(Form("hspixel_d%d_x%d_y%d",dssd,xstrip,ystrip),"",enbins,0,10000);
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

      if ( run == 2098 ){
          bool skip = false;
          for (int idet = 0; idet < int(br_vsi->size()); idet++)
              if ( (*br_vsi)[idet].ts > 1800e9 ) skip = true;
          if ( skip ) continue;
      }
      
      vsi.clear();
      //vsi = *br_vsi;
      
      // display progress and time needed
        if (jentry%100000 == 99999){
            stop = clock();
            printf("Process %.2f %%  Time remaining %02d min %02d s                                     \r",double(jentry-startentry)/double(stopentry-startentry)*100.,
                int((stop-start)*(stopentry-jentry)/(jentry-startentry)/1e6/60),
                int((stop-start)*(stopentry-jentry)/(jentry-startentry)/1e6)%60);
            fflush(stdout);
        }
      
      // pixel positions
      for (int idet = 0; idet < int(br_vsi->size()); idet++)
          hthetaphi->Fill((*br_vsi)[idet].phi, (*br_vsi)[idet].theta);

      // single spectra
      for (int idet = 0; idet < int(br_vsi->size()); idet++)
          hspixel[(*br_vsi)[idet].dssd][(*br_vsi)[idet].xid][(*br_vsi)[idet].yid]->Fill((*br_vsi)[idet].e, 1/pixelpos[(*br_vsi)[idet].dssd][(*br_vsi)[idet].xid][(*br_vsi)[idet].yid][2]);
      
      //if ( br_vsi->size() != 3 ) continue;
      
      for (int iv = 1; iv < pow(2, br_vsi->size()); iv++){  // loop of combination
          vsi.clear();
          int index = iv;
          for (int idet = 0; idet < int(br_vsi->size()); idet++){
              if (index%2)
                  vsi.push_back((*br_vsi)[idet]);
              index /= 2;
          }
          if ( vsi.size() != 3 ) continue;
        
          bool coincident = true;
          for (int idet = 0; idet < int(vsi.size()); idet++)
              for (int jdet = idet+1; jdet < int(vsi.size()); jdet++)
                  if ( abs(vsi[idet].ts - vsi[jdet].ts) > window )
                      coincident = false;
          if ( !coincident ) continue;
          
          esum = 0;
            for (int idet = 0; idet < int(vsi.size()); idet++) {
                m[idet] = m_alpha;
                e[idet] = vsi[idet].e;
                esum += e[idet];
                d[idet].SetX(TMath::Sin(vsi[idet].theta/180.0*TMath::Pi())*TMath::Cos(vsi[idet].phi/180.0*TMath::Pi()));
                d[idet].SetY(TMath::Sin(vsi[idet].theta/180.0*TMath::Pi())*TMath::Sin(vsi[idet].phi/180.0*TMath::Pi()));
                d[idet].SetZ(TMath::Cos(vsi[idet].theta/180.0*TMath::Pi()));
            }
          
            double invmass = GetInvMass(3,m,e,d,2425*3);
            hs->Fill(invmass);

            TVector3 ptot;
            TVector3 dc[3];
            double ec[3];
            TVector3 Iz; Iz.SetZ(1);
            for (int idet = 0; idet < int(vsi.size()); idet++){
                dc[idet] = d[idet]*sqrt(2*e[idet]/ma) - Iz*(sqrt(2*mp*Ep)/(mp+mt));
                dc[idet] = dc[idet].Unit();
                ec[idet] = e[idet] - 2*sqrt(ma*e[idet]*mp*Ep)/(mp+mt) + ma*mp/(mp+mt)/(mp+mt)*Ep;
                
                ptot = ptot + dc[idet]*sqrt(2*ma*u*ec[idet]/1000);
            }
            
            hsp->Fill(invmass, ptot.Mag());
/*
            double mtemp, etemp;
            TVector3 dtemp;

            double invmass3 = GetInvMass(2,m,e,d,2425*2-4942);
            if ( abs(vsi[2].theta - 90) < 5 )
                hs3->Fill(invmass3);

            ////////////////////////////////////////////////

            mtemp=m[0];m[0]=m[2];m[2]=mtemp;
            etemp=e[0];e[0]=e[2];e[2]=etemp;
            dtemp=d[0];d[0]=d[2];d[2]=dtemp;

            double invmass1 = GetInvMass(2,m,e,d,2425*2-4942);
            if ( abs(vsi[0].theta - 90) < 2.5 )
                hs1->Fill(invmass1);

            mtemp=m[0];m[0]=m[2];m[2]=mtemp;
            etemp=e[0];e[0]=e[2];e[2]=etemp;
            dtemp=d[0];d[0]=d[2];d[2]=dtemp;

            //////////////////////////////////////////////

            mtemp=m[1];m[1]=m[2];m[2]=mtemp;
            etemp=e[1];e[1]=e[2];e[2]=etemp;
            dtemp=d[1];d[1]=d[2];d[2]=dtemp;

            double invmass2 = GetInvMass(2,m,e,d,2425*2-4942);
            if ( abs(vsi[1].theta - 90) < 2.5 )
                hs2->Fill(invmass2);

            mtemp=m[1];m[1]=m[2];m[2]=mtemp;
            etemp=e[1];e[1]=e[2];e[2]=etemp;
            dtemp=d[1];d[1]=d[2];d[2]=dtemp;
*/
            //////////////////////////////////////////////

            // center of mass system
            TLorentzVector a1(TMath::Sqrt(2*m[0]*e[0])*d[0],m[0]+e[0]);
            TLorentzVector a2(TMath::Sqrt(2*m[1]*e[1])*d[1],m[1]+e[1]);
            TLorentzVector a3(TMath::Sqrt(2*m[2]*e[2])*d[2],m[2]+e[2]);
            TLorentzVector all = a1+a2+a3;

            a1.Boost(-all.BoostVector()); e[0] = a1.P()*a1.P()/m[0]/2.0;
            a2.Boost(-all.BoostVector()); e[1] = a2.P()*a2.P()/m[1]/2.0;
            a3.Boost(-all.BoostVector()); e[2] = a3.P()*a3.P()/m[2]/2.0;

            esum = e[0]+e[1]+e[2];

            hsbg->Fill(esum);
            hcos->Fill(TMath::ACos(a1.Vect().Unit().Dot(TVector3(0.0,0.0,1.0)))/TMath::Pi()*180);
            // hcos->Fill(a1.Vect().Unit().Dot(TVector3(0.0,0.0,1.0)));

            ha1->Fill(e[0]);
            ha2->Fill(e[1]);
            ha3->Fill(e[2]);

            // Dalitz diagram
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if ( j != i )
                        h1->Fill((e[i]/esum+2*e[j]/esum-1)/TMath::Sqrt(3.0),e[i]/esum-1.0/3);

            if ( invmass > 15500 && invmass < 18500 && ptot.Mag() < 150 ){
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        if ( j != i )
                            h1_16->Fill((e[i]/esum+2*e[j]/esum-1)/TMath::Sqrt(3.0),e[i]/esum-1.0/3);
            }

            for (int i=0; i<3; i++)
                for (int j=0; j<3; j++)
                    hdeq->Fill(invmass,e[i]-e[j]);
            ha1ha2->Fill(e[1], e[0]);
            ha1ha3->Fill(e[2], e[0]);
            ha2ha3->Fill(e[2], e[1]);
        }

      //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
    

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
      if ( nspec == 0 ){
          std::cout<<"No statistics for "<<mintheta+dtheta*itheta<<" deg"<<'\n';
          continue;
      }
      hstheta[itheta]->Scale(1/(double)nspec);
      std::cout<<mintheta+dtheta*itheta<<" deg: "<<nspec<<" pixels"<<'\n';
  }
    
  // TH1/TH2 write
    /*
  hs->Write();
  hsbg->Write();
  ha1->Write();
  ha2->Write();
  ha3->Write();
  h1->Write();
  h1_16->Write();
  hcos->Write();
  hdeq->Write();
  ha1ha2->Write();
  ha1ha3->Write();
  ha2ha3->Write();

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

Double_t DataAnalysis::GetInvMass(Int_t npar, Double_t *m0, Double_t *Ek, TVector3 *direction, Double_t Eth)
{
    const Int_t nparticle = npar;

    Double_t E[nparticle];
    Double_t pmag[nparticle];
    Double_t theta[nparticle];
    Double_t phi[nparticle];
    TVector3 p[nparticle];

    Double_t Etot = 0;
    Double_t m0tot = 0;
    TVector3 ptot;        //By default all components are initialized to zero 
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
