
//#define data_B
//#define data_Li
#define data_Au

const int nDSSD = 2;
const int nxstrips = 16;
const int nystrips = 16;

const int Ep = 3000;  // keV
const int Nrun = 1;

//const int run[Nrun] = {2092};              //  675 keV, B
//const int run[Nrun] = {2094};              // 1388 keV, B
//const int run[Nrun] = {2098};              // 2000 keV, B
//const int run[Nrun] = {2101, 2102};          // 2500 keV, B
//const int run[Nrun] = {2074, 2075};          // 2640 keV, B
//const int run[Nrun] = {2081, 2082, 2083, 2084};  // 3000 keV, B

//const int run[Nrun] = {2088};          //  675 keV, Li
//const int run[Nrun] = {2099};          // 2000 keV, Li
//const int run[Nrun] = {2103};          // 2500 keV, Li
//const int run[Nrun] = {2073};          // 2640 keV, Li
//const int run[Nrun] = {2080};          // 3000 keV, Li

//const int run[Nrun] = {2087};          //  675 keV, Au
//const int run[Nrun] = {2097};          // 2000 keV, Au
//const int run[Nrun] = {2072};          // 2640 keV, Au
const int run[Nrun] = {2079};          // 3000 keV, Au

const int mintheta = 50;     // minimum central theta value
const int dtheta = 5;        // step of central theta value
const int ntheta = 17;          // total number of theta value

void add()
{
    TFile *ipf[Nrun];
    TH1I *hs[Nrun];
    TH2I *h1[Nrun];
    TH2I *h1_16[Nrun];
    TH1I *hsingle[Nrun][ntheta];
    TH1I *hspixel[Nrun][nDSSD+1][nxstrips][nystrips];
    
    for (int irun = 0; irun < Nrun; irun++){
#ifdef data_B
        ipf[irun] = new TFile(Form("5Draw_B/ana_%d_W100.root",run[irun]));
#elif defined data_Li
        ipf[irun] = new TFile(Form("5Draw_Li/ana_%d_W100.root",run[irun]));
#elif defined data_Au
        ipf[irun] = new TFile(Form("5Draw_B/ana_%d_W100.root",run[irun]));
#else
        ipf[irun] = new TFile(Form("5Draw/ana_%d_W100.root",run[irun]));
#endif
        
        hs[irun] = (TH1I*)ipf[irun]->Get("hs");
#ifdef data_B
        h1[irun] = (TH2I*)ipf[irun]->Get("h1");
        h1_16[irun] = (TH2I*)ipf[irun]->Get("h1_16");
#endif
        for (int i = 0; i < ntheta; i++)
            hsingle[irun][i] = (TH1I*)ipf[irun]->Get(Form("hstheta/hsingle_%ddeg",mintheta+dtheta*i));
        for (int dssd = 1; dssd <= nDSSD; dssd++)
            for (int xstrip = 0; xstrip < nxstrips; xstrip++)
                for (int ystrip = 0; ystrip < nystrips; ystrip++)
                    hspixel[irun][dssd][xstrip][ystrip] = (TH1I*)ipf[irun]->Get(Form("hstheta/hspixel/dssd%d/hspixel_d%d_x%d_y%d",dssd,dssd,xstrip,ystrip));
    }
    
    for (int irun = 1; irun < Nrun; irun++){
        hs[0]->Add(hs[irun]);
#ifdef data_B
        h1[0]->Add(h1[irun]);
        h1_16[0]->Add(h1_16[irun]);
#endif
        for (int i = 0; i < ntheta; i++)
            hsingle[0][i]->Add(hsingle[irun][i]);
        for (int dssd = 1; dssd <= nDSSD; dssd++)
            for (int xstrip = 0; xstrip < nxstrips; xstrip++)
                for (int ystrip = 0; ystrip < nystrips; ystrip++)
                    hspixel[0][dssd][xstrip][ystrip]->Add(hspixel[irun][dssd][xstrip][ystrip]);
    }
    /*
    for (int i = 0; i < 15; i++){
        ofstream ofs(Form("hstheta_%ddeg.txt",60+5*i));
        ofs<<"E(keV)"<<'\t'<<"Count"<<'\n';
        ofs.setf(ios::fixed);
        ofs.precision(0);
        for (int ibin = 1; ibin <= hsingle[0][i]->GetXaxis()->GetNbins(); ibin++){
            double x = hsingle[0][i]->GetBinLowEdge(ibin) + hsingle[0][i]->GetBinWidth(ibin)/2.;
            double y = hsingle[0][i]->GetBinContent(ibin);
            ofs<<x<<'\t'<<y<<'\n';
        }
        ofs.close();
    }
    */
#ifdef data_B
    TFile *opf = new TFile(Form("anaB_%04dkeV.root",Ep),"RECREATE");
#elif defined data_Li
    TFile *opf = new TFile(Form("anaLi_%04dkeV.root",Ep),"RECREATE");
#elif defined data_Au
    TFile *opf = new TFile(Form("anaAu_%04dkeV.root",Ep),"RECREATE");
#else
    TFile *opf = new TFile(Form("ana_%04dkeV.root",Ep),"RECREATE");
#endif
    
    hs[0]->Write();
#ifdef data_B
    h1[0]->Write();
    h1_16[0]->Write();
#endif
    for (int i = 0; i < ntheta; i++)
        hsingle[0][i]->Write();
    for (int dssd = 1; dssd <= nDSSD; dssd++)
        for (int xstrip = 0; xstrip < nxstrips; xstrip++)
            for (int ystrip = 0; ystrip < nystrips; ystrip++)
                hspixel[0][dssd][xstrip][ystrip]->Write();
    
    opf->Close();
    
    gROOT->ProcessLine(".q");
}