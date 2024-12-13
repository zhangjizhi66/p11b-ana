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
  ipt->SetBranchAddress("vsifv", &br_vsifv);
  ipt->SetBranchAddress("vsifh", &br_vsifh);
  ipt->SetBranchAddress("vsibv", &br_vsibv);
  ipt->SetBranchAddress("vsibh", &br_vsibh);

  benchmark = new TBenchmark;
  gr = new TRandom3(0);
  
  // init TH1/TH2
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
      AnaMultiHit(vsifh, vsifv, ereso[dssd]);
      
      dssd = 2;
      AnaMultiHit(vsibh, vsibv, ereso[dssd]);
      
      //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

      //if ( vsifh.size()>0 || vsifv.size()>0 || vsibh.size()>0 || vsibv.size()>0 ) 
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
  br_vsifh = 0;
  br_vsifv = 0;
  br_vsibh = 0;
  br_vsibv = 0;
    
  opt->Branch("vsifh", &vsifh);
  opt->Branch("vsifv", &vsifv);
  opt->Branch("vsibh", &vsibh);
  opt->Branch("vsibv", &vsibv);
}

bool DataAnalysis::Single(std::vector<dets> &vec)    // judge and merge
{
    if ( vec.size() == 1 )
        return true;
    
    if ( vec.size() == 2 && abs(vec[0].id - vec[1].id) == 1 ){  // adjacent
        if ( vec[0].e < vec[1].e )
            vec[0].id = vec[1].id;
        vec[0].e += vec[1].e;
        vec[0].raw = -1;
        vec.pop_back();
        return true;
    }
    
    if ( vec.size() == 3 && abs(vec[0].id - vec[1].id) + abs(vec[0].id - vec[2].id) + abs(vec[1].id - vec[2].id) == 4 ){ // adjacent and centered
        for ( int i = 0; i < 3; i++ ){
            int sum = 0;
            for ( int j = 0; j < 3 && j != i; j++)
                sum += abs(vec[i].id - vec[j].id);
            
            if ( sum == 2 ){
                for ( int j = 0; j < 3 && j != i; j++)
                    if ( vec[i].e < vec[j].e )
                        return false;
                vec[0].id = vec[i].id;
                vec[0].e += vec[1].e + vec[2].e;
                vec[0].raw = -1;
                vec.pop_back();
                vec.pop_back();
                return true;
            }
        }
    }
    
    return false;
}

bool DataAnalysis::Merge(std::vector<dets> &vec)
{
    for (int iv = 1; iv < pow(2, vec.size()); iv++){  // loop of combination
        std::vector<dets> temp1, temp2;
        int index = iv;
        for (int idet = 0; idet < int(vec.size()); idet++){
            if (index%2)
                temp1.push_back(vec[idet]);
            else
                temp2.push_back(vec[idet]);
            index /= 2;
        }
        
        if ( temp1.size() == 3 ){
            bool good = Single(temp1);
            if ( good ){
                Merge(temp2);
                vec = temp1;
                vec.insert(vec.end(), temp2.begin(), temp2.end());
                return true;
            }
        }
    }
    
    for (int iv = 1; iv < pow(2, vec.size()); iv++){  // loop of combination
        std::vector<dets> temp1, temp2;
        int index = iv;
        for (int idet = 0; idet < int(vec.size()); idet++){
            if (index%2)
                temp1.push_back(vec[idet]);
            else
                temp2.push_back(vec[idet]);
            index /= 2;
        }
        
        if ( temp1.size() == 2 ){
            bool good = Single(temp1);
            if ( good ){
                Merge(temp2);
                vec = temp1;
                vec.insert(vec.end(), temp2.begin(), temp2.end());
                return true;
            }
        }
    }
    
    return false;  // no merging
}

bool DataAnalysis::Match(std::vector<dets> &vfront, std::vector<dets> &vback, double reso)
{
#ifdef ALLSINGLE
    if ( vfront.size() == 1 && vback.size() == 1 )
        return true;
#endif
    
    // energy match
    double efront = 0, eback = 0;
    for (int idet = 0; idet < int(vfront.size()); idet++)
        efront += vfront[idet].e;
    for (int idet = 0; idet < int(vback.size()); idet++)
        eback += vback[idet].e;
    if ( abs(efront-eback) > reso )
        return false;
    
    std::vector<dets> *v1, *v2;       // v1-single particle, v2-multi particles
    double e1, e2;
    std::vector<dets> vfront_copy = vfront;
    std::vector<dets> vback_copy = vback;
    if ( Single(vfront_copy) ){
        v1 = &vfront;
        v2 = &vback;
        e1 = efront;
        e2 = eback;
    }
    else if ( Single(vback_copy) ){
        v1 = &vback;
        v2 = &vfront;
        e1 = eback;
        e2 = efront;
    }
    else
        return false;

    for (int ifront = 1; ifront < pow(2, vfront.size()); ifront++){
        std::vector<dets> fronttemp;
        int indexfront = ifront;
        for (int idet = 0; idet < int(vfront.size()); idet++){
            if (indexfront%2)
                fronttemp.push_back(vfront[idet]);
            indexfront /= 2;
        }
        for (int iback = 1; iback < pow(2, vback.size()); iback++){
            std::vector<dets> backtemp;
            int indexback = iback;
            for (int idet = 0; idet < int(vback.size()); idet++){
                if (indexback%2)
                    backtemp.push_back(vback[idet]);
                indexback /= 2;
            }
                
            if ( ifront == int(pow(2, vfront.size()))-1 && iback == int(pow(2, vback.size()))-1 )
                continue;

            if ( Match(fronttemp, backtemp, reso) )
                return false;
        }
    }
    
    Merge(vfront);
    Merge(vback);
    
    for ( int idet = 0; idet < int(v2->size()); idet++ ){
        dets det = (*v1)[0];
        det.raw = -1;
        det.e = (*v2)[idet].e*e1/e2;
        
        v1->push_back(det);
    }
    v1->erase(v1->begin());
    
    return true;
}

bool DataAnalysis::AnaMultiHit(std::vector<dets> &vfront, std::vector<dets> &vback, double reso)
{
    if ( vfront.size() != 0 && vback.size() != 0 ){
    
        for (int ifront = 1; ifront < pow(2, vfront.size()); ifront++){
            std::vector<dets> fronttemp1, fronttemp2;
            int indexfront = ifront;
            for (int idet = 0; idet < int(vfront.size()); idet++){
                if (indexfront%2)
                    fronttemp1.push_back(vfront[idet]);
                else
                    fronttemp2.push_back(vfront[idet]);
                indexfront /= 2;
            }
            for (int iback = 1; iback < pow(2, vback.size()); iback++){
                std::vector<dets> backtemp1, backtemp2;
                int indexback = iback;
                for (int idet = 0; idet < int(vback.size()); idet++){
                    if (indexback%2)
                        backtemp1.push_back(vback[idet]);
                    else
                        backtemp2.push_back(vback[idet]);
                    indexback /= 2;
                }

                if ( Match(fronttemp1, backtemp1, reso) ){
                    AnaMultiHit(fronttemp2, backtemp2, reso);
                    
                    vfront = fronttemp1;
                    vfront.insert(vfront.end(), fronttemp2.begin(), fronttemp2.end());
                    vback = backtemp1;
                    vback.insert(vback.end(), backtemp2.begin(), backtemp2.end());
                    
                    return true;
                }
            }
        }
        
    }
    
    std::vector<dets> vempty;
    vfront = vempty;
    vback = vempty;
    return false;
}

void DataAnalysis::test()
{
    std::cout<<'\n'<<"Test Mode!"<<'\n'<<'\n';
    
    int reso = 150;
    
    /*
    std::vector<int>   idfront{   7,   1,   9};
    std::vector<double> efront{ 730, 981, 790};
    
    std::vector<int>    idback{   9,  10,   2,   8,   7};
    std::vector<double>  eback{ 420, 337, 869, 797, 111};
    */
    
    
    std::vector<int>   idfront{   7};
    std::vector<double> efront{ 700};
    
    std::vector<int>    idback{   9};
    std::vector<double>  eback{ 1000};
    
    
    std::vector<dets> vfront;
    for (int idet = 0; idet < int(std::min(idfront.size(), efront.size())); idet++){
        dets det;
        det.id = idfront[idet];
        det.e  =  efront[idet];
        vfront.push_back(det);
    }
    
    std::vector<dets> vback;
    for (int idet = 0; idet < int(std::min(idback.size(), eback.size())); idet++){
        dets det;
        det.id = idback[idet];
        det.e  =  eback[idet];
        vback.push_back(det);
    }
    
    std::cout<<"original front"<<'\n';
    std::cout<<"id:"<<'\t';
    for (dets det : vfront)
        std::cout<<det.id<<'\t';
    std::cout<<'\n';
    std::cout<<"e:"<<'\t';
    for (dets det : vfront)
        std::cout<<det.e<<'\t';
    std::cout<<'\n'<<'\n';
    
    std::cout<<"original back"<<'\n';
    std::cout<<"id:"<<'\t';
    for (dets det : vback)
        std::cout<<det.id<<'\t';
    std::cout<<'\n';
    std::cout<<"e:"<<'\t';
    for (dets det : vback)
        std::cout<<det.e<<'\t';
    std::cout<<'\n'<<'\n';
    
    AnaMultiHit(vfront, vback, reso);
    
    std::cout<<"processed front"<<'\n';
    std::cout<<"id:"<<'\t';
    for (dets det : vfront)
        std::cout<<det.id<<'\t';
    std::cout<<'\n';
    std::cout<<"e:"<<'\t';
    for (dets det : vfront)
        std::cout<<det.e<<'\t';
    std::cout<<'\n'<<'\n';
    
    std::cout<<"processed back"<<'\n';
    std::cout<<"id:"<<'\t';
    for (dets det : vback)
        std::cout<<det.id<<'\t';
    std::cout<<'\n';
    std::cout<<"e:"<<'\t';
    for (dets det : vback)
        std::cout<<det.e<<'\t';
    std::cout<<'\n'<<'\n';
}

// 
// event.cc ends here
