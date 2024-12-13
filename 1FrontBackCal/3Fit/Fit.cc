
#include "UserDefine.hh"

#include <iostream>
#include <fstream>

#include "TGraph.h"
#include "TFile.h"
#include "TF1.h"
#include "TH2.h"

TGraph *gr[nDSSD+1][nxstrips][nystrips];   // gr[dssd][xstrip][ystrip], dssd=0 is not used

double xpar[nDSSD+1][nxstrips][3];   // xpar[dssd][strip][fitparameter], dssd=0 is not used
double ypar[nDSSD+1][nystrips][3];   // ypar[dssd][strip][fitparameter], dssd=0 is not used

std::ofstream ofs_par;                 // fit parameter output
std::ofstream ofs_log;                       // log output

void xcali(int dssd, int xstrip, int ymin, int ymax);  // calibrate xstrip using ystrips (ymin,ymax)
void ycali(int dssd, int ystrip, int xmin, int xmax);  // calibrate ystrip using xstrips (xmin,xmax)

TH2D* hall[nDSSD+1];
TH2D* hresall[nDSSD+1];

void fit(TFile *ipf, TFile *opf, int run)
{
    for (int dssd=1; dssd<=nDSSD; dssd++)
      for (int xstrip=0; xstrip<nxstrips; xstrip++)
          for (int ystrip=0; ystrip<nystrips; ystrip++){
              gr[dssd][xstrip][ystrip] = (TGraph*)ipf->Get(Form("gr_dssd%d_x%d_y%d",dssd,xstrip,ystrip));
          }
    
    ofs_par.open(Form("fitparameter_%04d.txt",run), std::ios::out);                 // fit parameter output
    ofs_log.open(Form("fit.log"), std::ios::out);
    
    // log output
    std::cout<<std::setw(8)<<"dssd   "<<"strip   "<<"strip   "<<"chi2/ndf   "<<"point_max   "<<"point_number"<<'\n';
    ofs_log<<std::setw(8)<<"dssd   "<<"strip   "<<"strip   "<<"chi2/ndf   "<<"point_max   "<<"point_number"<<'\n';
    
    // calibrate from dssd's center to margin
    if ( xy ){
        for (int dssd=1; dssd<=nDSSD; dssd++){
            xcali(dssd, xstrip0[dssd], ystrip0[dssd], ystrip0[dssd]);
            for (int width=1; width<=std::max(nxstrips,nystrips); width++){
                xcali(dssd, xstrip0[dssd]+width, ystrip0[dssd]-(width-1), ystrip0[dssd]+(width-1));
                xcali(dssd, xstrip0[dssd]-width, ystrip0[dssd]-(width-1), ystrip0[dssd]+(width-1));
                ycali(dssd, ystrip0[dssd]+width, xstrip0[dssd]-(width), xstrip0[dssd]+(width));
                ycali(dssd, ystrip0[dssd]-width, xstrip0[dssd]-(width), xstrip0[dssd]+(width));
            }
            ypar[dssd][ystrip0[dssd]][1] = 1;
        }
    }
    else {
        for (int dssd=1; dssd<=nDSSD; dssd++){
            ycali(dssd, ystrip0[dssd], xstrip0[dssd], xstrip0[dssd]);
            for (int width=1; width<=std::max(nxstrips,nystrips); width++){
                ycali(dssd, ystrip0[dssd]+width, xstrip0[dssd]-(width-1), xstrip0[dssd]+(width-1));
                ycali(dssd, ystrip0[dssd]-width, xstrip0[dssd]-(width-1), xstrip0[dssd]+(width-1));
                xcali(dssd, xstrip0[dssd]+width, ystrip0[dssd]-(width), ystrip0[dssd]+(width));
                xcali(dssd, xstrip0[dssd]-width, ystrip0[dssd]-(width), ystrip0[dssd]+(width));
            }
            xpar[dssd][xstrip0[dssd]][1] = 1;
        }
    }
    
    // fit parameter output
    ofs_par<<std::setprecision(parameterprecision);
    for (int dssd=1; dssd<=nDSSD; dssd++)
        for (int par=0; par<3; par++){
            for (int strip=0; strip<nxstrips; strip++)
                ofs_par<<xpar[dssd][strip][par]<<'\n';
            for (int strip=0; strip<nystrips; strip++)
                ofs_par<<ypar[dssd][strip][par]<<'\n';
        }
    
    for (int dssd=1; dssd<=nDSSD; dssd++){
        hresall[dssd] = new TH2D(Form("hresall_dssd%d",dssd),Form("hresall of dssd%d",dssd), resbins,-resrange,resrange,ybins,minfitrange,maxfitrange);
        hall[dssd] = new TH2D(Form("hall_dssd%d",dssd),Form("hall of dssd%d",dssd), xbins,minfitrange,maxfitrange,ybins,minfitrange,maxfitrange);
        for (int xstrip=0; xstrip<nxstrips; xstrip++)
            for (int ystrip=0; ystrip<nystrips; ystrip++){
                if (!gr[dssd][xstrip][ystrip]) continue;
                for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
                    double x = gr[dssd][xstrip][ystrip]->GetX()[i];
                    double y = gr[dssd][xstrip][ystrip]->GetY()[i];
                    hresall[dssd]->Fill(x-y,y);
                    hall[dssd]->Fill(x,y);
                }
            }
        hresall[dssd]->Write();
        hall[dssd]->Write();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void xcali(int dssd, int xstrip, int ymin, int ymax)   // given calibrated ystrips within (ymin,ymax)
{
    if (ymin<0) ymin = 0;
    if (ymax>=nystrips) ymax = nystrips-1;
    if (xstrip<0 || xstrip>=nxstrips) return;
    
    // merge gr[dssd][xstrip][ymin] to gr[dssd][xstrip][ymax] as grtmp
    double xmax = 0, xymax = 0;
    for (int ystrip=ymin; ystrip<=ymax; ystrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            if (x > xmax) xmax = x;
            if (x+y > xymax) xymax = x+y;
        }
    }
    double bound = xymax*percentage[dssd];
    
    int nbig = 0;
    for (int ystrip=ymin; ystrip<=ymax; ystrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            
            if (x+y>bound) nbig++;
        }
    }
    
    TGraph *grtmp = new TGraph;
    int n = 0, nsmall = 0;
    for (int ystrip=ymin; ystrip<=ymax; ystrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            
            if (x+y<bound){
                nsmall++;
                if (nsmall > nbig && balance) continue;
            }
            grtmp->SetPoint(n++,x,y);
        }
    }
    
    if (grtmp->GetN() < 2){
        std::cout<<"dssd"<<dssd<<'\t'<<'\t'<<"x"<<xstrip<<'\t'<<'\t'<<"y"<<ymin<<"-"<<ymax<<'\t'<<'\t'<<"too few points to fit !"<<'\n';
        return;
    }
    
    // fit and write merged TGraph grtmp
    TF1 *f2 = new TF1("fpol2","[0]+[1]*x+[2]*x*x",minfitrange,maxfitrange);
    if (linear) f2->FixParameter(2,0);
    grtmp->Fit("fpol2",Form("Q ROB"));
    f2->GetParameters(&xpar[dssd][xstrip][0]);
    grtmp->SetName(Form("grfit_dssd%d_x%d",dssd,xstrip));
    grtmp->SetTitle(Form("calibrate dssd%d's xstrip %d with ystrip %d-%d",dssd,xstrip,ymin,ymax));
    grtmp->Write();
    
    // calibrate all gr[dssd][xstrip][0-15] related to xstrip
    for (int ystrip=0; ystrip<nystrips; ystrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            x = xpar[dssd][xstrip][0] + xpar[dssd][xstrip][1]*x + xpar[dssd][xstrip][2]*x*x;
            gr[dssd][xstrip][ystrip]->SetPoint(i,x,y);
        }
    }
    
    std::cout<<"dssd"<<dssd<<'\t'<<'\t'<<"x"<<xstrip<<'\t'<<'\t'<<"y"<<ymin<<"-"<<ymax<<'\t'<<'\t'
        <<f2->GetChisquare()/f2->GetNDF()<<'\t'<<'\t'<<xmax<<'\t'<<'\t'<<grtmp->GetN()<<'\n';
    ofs_log<<"dssd"<<dssd<<'\t'<<'\t'<<"x"<<xstrip<<'\t'<<'\t'<<"y"<<ymin<<"-"<<ymax<<'\t'<<'\t'
        <<f2->GetChisquare()/f2->GetNDF()<<'\t'<<'\t'<<xmax<<'\t'<<'\t'<<grtmp->GetN()<<'\n';
}

void ycali(int dssd, int ystrip, int xmin, int xmax)   // given calibrated xstrips within (xmin,xmax)
{
    if (xmin<0) xmin = 0;
    if (xmax>=nxstrips) xmax = nxstrips-1;
    if (ystrip<0 || ystrip>=nystrips) return;
    
    // merge gr[dssd][xmin][ystrip] to gr[dssd][xmax][ystrip] as grtmp
    double ymax = 0, xymax = 0;
    for (int xstrip=xmin; xstrip<=xmax; xstrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            
            if (y > ymax) ymax = y;
            if (x+y > xymax) xymax = x+y;
        }
    }
    double bound = xymax*percentage[dssd];
    
    int nbig = 0;
    for (int xstrip=xmin; xstrip<=xmax; xstrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            
            if (x+y>bound) nbig++;
        }
    }
    
    TGraph *grtmp = new TGraph;
    int n = 0, nsmall = 0;
    for (int xstrip=xmin; xstrip<=xmax; xstrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            
            if (x+y<bound){
                nsmall++;
                if (nsmall > nbig && balance) continue;
            }
            
            grtmp->SetPoint(n++,y,x);  // swap x y
        }
    }
    
    if (grtmp->GetN() < 2){
        std::cout<<"dssd"<<dssd<<'\t'<<'\t'<<"y"<<ystrip<<'\t'<<'\t'<<"x"<<xmin<<"-"<<xmax<<'\t'<<"too few points to fit !"<<'\n';
        return;
    }
        
    // fit and write merged TGraph grtmp
    TF1 *f2 = new TF1("fpol2","[0]+[1]*x+[2]*x*x",minfitrange,maxfitrange);
    if (linear) f2->FixParameter(2,0);
    grtmp->Fit("fpol2",Form("Q ROB"));
    f2->GetParameters(&ypar[dssd][ystrip][0]);
    grtmp->SetName(Form("grfit_dssd%d_y%d",dssd,ystrip));
    grtmp->SetTitle(Form("calibrate dssd%d's ystrip %d with xstrip %d-%d",dssd,ystrip,xmin,xmax));
    grtmp->Write();
    
    // calibrate all gr[dssd][0-15][ystrip] related to ystrip
    for (int xstrip=0; xstrip<nxstrips; xstrip++){
        if (!gr[dssd][xstrip][ystrip]) continue;
        for (int i=0; i<gr[dssd][xstrip][ystrip]->GetN(); i++){
            double x = gr[dssd][xstrip][ystrip]->GetX()[i];
            double y = gr[dssd][xstrip][ystrip]->GetY()[i];
            y = ypar[dssd][ystrip][0] + ypar[dssd][ystrip][1]*y + ypar[dssd][ystrip][2]*y*y;
            gr[dssd][xstrip][ystrip]->SetPoint(i,x,y);
        }
    }
    
    std::cout<<"dssd"<<dssd<<'\t'<<'\t'<<"y"<<ystrip<<'\t'<<'\t'<<"x"<<xmin<<"-"<<xmax<<'\t'<<'\t'
        <<f2->GetChisquare()/f2->GetNDF()<<'\t'<<'\t'<<ymax<<'\t'<<'\t'<<grtmp->GetN()<<'\n';
    ofs_log<<"dssd"<<dssd<<'\t'<<'\t'<<"y"<<ystrip<<'\t'<<'\t'<<"x"<<xmin<<"-"<<xmax<<'\t'<<'\t'
        <<f2->GetChisquare()/f2->GetNDF()<<'\t'<<'\t'<<ymax<<'\t'<<'\t'<<grtmp->GetN()<<'\n';
}

