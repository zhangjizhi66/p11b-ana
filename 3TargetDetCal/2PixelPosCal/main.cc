
#include "UserDefine.hh"

#include "TFile.h"
#include "TString.h"
#include "TF2.h"
#include "TH2.h"
#include "TGraph.h"
#include "Math/EulerAngles.h"
#include "Math/Vector3Dfwd.h"
#include <fstream>
#include <string>

TH1D *hElas;
TGraph *grElas;
bool reject;

ROOT::Math::XYZVector GetPixelPos(int dssd, int xstrip, int ystrip, double x, double y, double z, double phi, double theta, double psi)
{
    // center of DSSD, relative to Au target
    ROOT::Math::XYZVector vc0(x, y, z);
    
    // rotation matrix of DSSD, in euler angles
    ROOT::Math::EulerAngles euler(phi, theta, psi);
    
    // coordinate inside DSSD
    ROOT::Math::XYZVector vx0(dux[dssd], duy[dssd], duz[dssd]);  
    ROOT::Math::XYZVector vy0(dvx[dssd], dvy[dssd], dvz[dssd]);
    vx0 = vx0.Unit();
    vy0 = vy0.Unit();
    
    // relative to DSSD center
    ROOT::Math::XYZVector vp0 = (xstrip-(nxstrips-1)/2.)*pixel_lx*vx0 + (ystrip-(nystrips-1)/2.)*pixel_ly*vy0;
    
    // relative to target
    ROOT::Math::XYZVector vp = euler(vp0) + vc0;
    
    return vp;
}

Double_t func(Double_t *val, Double_t *par)  // TF1::RejectPoint();
{
    Int_t xstrip = val[0];
    Int_t ystrip = val[1];
    
    // fit range
    if ( reject && ( xstrip < xminfitrange || xstrip > xmaxfitrange || ystrip < yminfitrange || ystrip > ymaxfitrange ) ){
        TF1::RejectPoint();
        return 0;
    }
    
    Int_t dssd = par[0];
    Double_t A = par[1];
    Double_t x = par[2];
    Double_t y = par[3];
    Double_t z = par[4];
    Double_t phi = par[5]/180.*TMath::Pi();
    Double_t theta = par[6]/180.*TMath::Pi();
    Double_t psi = par[7]/180.*TMath::Pi();
    
    // rotation matrix of DSSD, in euler angles
    ROOT::Math::EulerAngles euler(phi, theta, psi);
    
    // normal vector of DSSD
    ROOT::Math::XYZVector vn0(dnx[dssd], dny[dssd], dnz[dssd]);
    ROOT::Math::XYZVector vn = euler(vn0);
    
    // pixel position
    ROOT::Math::XYZVector vp = GetPixelPos(dssd, xstrip, ystrip, xx0[dssd]+x, yy0[dssd]+y, zz0[dssd]+z, phi, theta, psi);      

    Double_t Sigma = hElas->Interpolate(vp.theta()/TMath::Pi()*180);       // elastic scattering cross-section (G4)
    //Double_t Sigma = grElas->Eval(vp.theta()/TMath::Pi()*180);       // elastic scattering cross-section (LISE++)
    Double_t Area = pixel_lx*pixel_ly;   // area of each pixel
    Double_t Cos = abs(vp.Dot(vn))/sqrt(vp.Mag2()*vn.Mag2());
    Double_t R2 = vp.Mag2();  // distance to target
    
    Double_t N = A*Sigma*Area*Cos/R2;
    
    return N;
}

int main(int argc, char* argv[])
{
    if (argc != 2){
        std::cout << "eg. " << argv[0] << "  [run number]" <<std::endl;
        return 1;
    }

    TString RunNumber(argv[1]);
    int run = RunNumber.Atoi();
    
    // Elastic scattering cross section (G4 simulation)
    TFile *ipf_Elas = new TFile("hElas.root");
    hElas = (TH1D*)ipf_Elas->Get("hElas");
    
    // Elastic scattering cross section (LISE++)
    grElas = new TGraph;
    grElas->SetName("grElas");
    std::ifstream ifs(TString::Format("%s",SCATTERFILEPATH).Data());
    std::string ss;
    double theta, sigma, a, b;
    for (int i = 0; i < 5; i++)
        getline(ifs, ss);
    int ipnt = 0;
    while ( !ifs.eof() ){
        ifs >> a >> b >> theta >> sigma;
        getline(ifs, ss);
        if ( ifs.eof() ) break;
        grElas->SetPoint(ipnt++, theta, sigma);
    }
    ifs.close();
std::cout<<"hh"<<'\n';
    // Fit
    TFile *ipf = new TFile(TString::Format("%s%s_%04d_W%d.root", RAWFILEPATH, RAWFILENAME, run, EVENTTIMEWINDOWSWIDTH).Data());
    TH2D *hstat[nDSSD+1];
    TF2 *f2[nDSSD+1];
    reject = true;
    for (int dssd = 1; dssd <= nDSSD; dssd++){
        f2[dssd] = new TF2(Form("f2_dssd%d",dssd), func, 0, nxstrips, 0, nystrips, 8);
        f2[dssd]->FixParameter(0, dssd);
        f2[dssd]->SetParameter(1, 1);
        
        f2[dssd]->SetParLimits(2, -posparlimits, posparlimits);
        //f2[dssd]->FixParameter(2, 0);
        f2[dssd]->SetParLimits(3, -posparlimits, posparlimits);
        //f2[dssd]->FixParameter(3, 0);
        f2[dssd]->SetParLimits(4, -posparlimits, posparlimits);
        //f2[dssd]->FixParameter(4, 0);
        
        //f2[dssd]->SetParLimits(5, -angparlimits, angparlimits);
        f2[dssd]->FixParameter(5, 0);
        //f2[dssd]->SetParLimits(6, -angparlimits, angparlimits);
        f2[dssd]->FixParameter(6, 0);
        //f2[dssd]->SetParLimits(7, -angparlimits, angparlimits);
        f2[dssd]->FixParameter(7, 0);
        
        hstat[dssd] = (TH2D*)ipf->Get(Form("hstat_dssd%d",dssd));
        hstat[dssd]->Fit(f2[dssd], "0L");   // L - Maximum Likelihood
    }
    
    // Output pixel positions
    std::ofstream ofs("pixelpos.txt");
    ofs<<std::setprecision(precision);
    for (int dssd = 1; dssd <= nDSSD; dssd++){
        double xc = xx0[dssd] + f2[dssd]->GetParameter(2);
        double yc = yy0[dssd] + f2[dssd]->GetParameter(3);
        double zc = zz0[dssd] + f2[dssd]->GetParameter(4);
        double phi = f2[dssd]->GetParameter(5);
        double theta = f2[dssd]->GetParameter(6);
        double psi = f2[dssd]->GetParameter(7);
        
        ROOT::Math::XYZVector vn0(dnx[dssd], dny[dssd], dnz[dssd]);
        ROOT::Math::EulerAngles euler(phi, theta, psi);
        ROOT::Math::XYZVector vn = euler(vn0);
        
        for (int xstrip = 0; xstrip < nxstrips; xstrip++)
            for (int ystrip = 0; ystrip < nystrips; ystrip++){
                ROOT::Math::XYZVector vp = GetPixelPos(dssd, xstrip, ystrip, xc, yc, zc, phi, theta, psi);
                
                Double_t Area = pixel_lx*pixel_ly;   // area of each pixel
                Double_t Cos = abs(vp.Dot(vn))/sqrt(vp.Mag2()*vn.Mag2());
                Double_t R2 = vp.Mag2();  // distance to target
                double solid = Area*Cos/R2;
                
                ofs<<vp.theta()/TMath::Pi()*180<<'\t'<<vp.phi()/TMath::Pi()*180<<'\t'<<solid<<'\n';
            }
        
        ofs<<'\n';
    }

    // output fit diagrams
    auto opf = new TFile(TString::Format("%s%s_cal_W%d.root", ROOTFILEPATH, ROOTFILENAME, EVENTTIMEWINDOWSWIDTH).Data(), "RECREATE");
    
    reject = false;
    
    TH2D *hstat_sim[nDSSD+1];
    for (int dssd = 1; dssd <= nDSSD; dssd++){
        hstat_sim[dssd] = new TH2D(Form("hstat_sim_dssd%d",dssd), Form("statistics fitted for dssd%d",dssd), nxstrips, 0, nxstrips, nystrips, 0, nystrips);
        for (int xstrip = 0; xstrip < nxstrips; xstrip++)
            for (int ystrip = 0; ystrip < nystrips; ystrip++)
                hstat_sim[dssd]->SetBinContent(hstat_sim[dssd]->FindBin(xstrip, ystrip), f2[dssd]->Eval(xstrip, ystrip));
        
        TH1F *tpjx, *tpjy;
        
        hstat[dssd]->Write();
        hstat[dssd]->ProjectionX(Form("tpjx_dssd%d",dssd));
        tpjx = (TH1F*)gROOT->FindObject(Form("tpjx_dssd%d",dssd));
        tpjx->SetLineColor(kBlack);
        tpjx->Write();
        hstat[dssd]->ProjectionY(Form("tpjy_dssd%d",dssd));
        tpjy = (TH1F*)gROOT->FindObject(Form("tpjy_dssd%d",dssd));
        tpjy->SetLineColor(kBlack);
        tpjy->Write();
        
        hstat_sim[dssd]->Write();
        hstat_sim[dssd]->ProjectionX(Form("tpjx_sim_dssd%d",dssd));
        tpjx = (TH1F*)gROOT->FindObject(Form("tpjx_sim_dssd%d",dssd));
        tpjx->SetLineColor(kRed);
        tpjx->Write();
        hstat_sim[dssd]->ProjectionY(Form("tpjy_sim_dssd%d",dssd));
        tpjy = (TH1F*)gROOT->FindObject(Form("tpjy_sim_dssd%d",dssd));
        tpjy->SetLineColor(kRed);
        tpjy->Write();
    }
    grElas->Write();
    opf->Close();
    return 0;
}