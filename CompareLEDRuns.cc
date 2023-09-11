//written by Nikhil Kumar, inital commit 7/21/2023
// headers
#include "CompareLEDRuns.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <TObject.h>
#include <TFile.h>
#include <TDirectoryFile.h>
#include <TClass.h>
#include <TString.h>
#include <TH1.h>
#include <TH1F.h>//a lot of these maybe not needed.
#include <TH2F.h>
#include <TF1.h>
#include <TF2.h>
#include <TList.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TPad.h>
#include <TText.h>
#include <TStyle.h>
#include <TROOT.h>
R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libfun4allraw.so);
R__LOAD_LIBRARY(libffarawmodules.so);

// I need to characterize: Ped, peak, ped_rms, peak location, peak width

//structure definitions
struct GaussFitResult {
    //custom structure to hold fit parameter data.
    double mean;
    double sigma;
    double amplitude;
};

struct DateRunBeam {
    //custom structure to hold fit parameter data. right now I have field for ihcal and ohcal. I could actually return a vector of my strutcure and only have fields for the parameters in general, but it may be better to keep it as is, since I expect to have ih and oh data for every set.
    std::string date;
    std::string run_number;
    bool Beam; //No->0, Yes->1
    std::string fname="run_"+run_number+".txt";
};

//function declarations
GaussFitResult Hist_Fit_1D(const char* filename, const char* histogramName);// pass a filename and a TH1F histname and recive gaussian fit parameters
float Tower_Slope_Fit(std::vector<float> TowerPeaks);// pass a vector of peak values for a single tower over many runs and recieve the value vs time slope. this will be useful for 2d histograms.
std::vector<TGraph*> CreateTGraphVector(const std::vector<DateRunBeam>& Run_info, const char* histogramName); // pass a vector of all Run_info (defined in the csv) and make tgraphs of gaussian sigma, mean, amp using Hist_Fit_1D
//float Channel_Value_Slope(const std::vector<DateRunBeam>& Run_info, const char* histogramName);// pass a  TH2F histogram name, and list of runs to look at and find the slope value for all eta phi bins
TGraph2D* slope_TGraph_2D(const std::vector<std::vector<float>>& slopes); // pass 2d vector of slope values for a 2d histogram and return a 2dgraph with the same x y structure and slope value as z 

//main program
GaussFitResult Hist_Fit_1D(const char* filename, const char* histogramName){
    //open the root file called filename.root
    TFile *f = new TFile(Form("LED_run_data_%s.root", filename));
    // pull up relevant histograms
    //TH1F *htemp=(TH1F*)f->Get(Form("%s", histogram));// c style cast, should work but I will try something else
    TH1F* htemp = static_cast<TH1F*>(f->Get(histogramName));//c++ style cast
    //Fit the two histograms
    htemp->Fit("gaus");
    //create calls to the fits
    TF1 *fit = htemp->GetFunction("gaus");
    //delete dynamically cast variables
    delete f;
    //store and return parameters of fit. hardcoded to assume gaussian
    GaussFitResult FitParam;
    FitParam.amplitude = fit->GetParameter(0);
    FitParam.mean = fit->GetParameter(1);
    FitParam.sigma = fit->GetParameter(2);
    return FitParam;
}

float Tower_Slope_Fit(std::vector<float> TowerPeaks, int numRuns){
    // create slope variable and number of runs to look at
    float slope;
    const int Number_runs = TowerPeaks.size();
    // make graph with siza=number of runs
    TGraph graph(Number_runs);

    // Fill the Graph with the value for each run number// note this function will be called tower by tower
    for (int j = 0; j < Number_runs; ++j) {
        graph.SetPoint(j, j + 1, TowerPeaks[j]); // 
    }
    //Fit graph with a linear function
    TF1 fit("fit", "pol1", 1, Number_runs); // Linear function: pol1. 1 stands for xmin=1 so we start with the first run.
    graph.Fit(&fit, "Q");

    // Get the slope of the fit (parameter 1 corresponds to the slope)
    slope = fit.GetParameter(1);
    return slope;
}

std::vector<TGraph*> CreateTGraphVector(const std::vector<DateRunBeam>& Run_info, const char* histogramName){
    std::vector<TGraph*> graphVector;
    // Extract data from the vectors and create arrays
    int numRuns = Run_info.size();
    double* Run_Number = new double[numRuns];
    double* Run_mean = new double[numRuns];
    double* Run_sigma = new double[numRuns];
    double* Run_amp = new double[numRuns];
    //-------------------------process run data
    std::vector<GaussFitResult> GaussFitParam;
    for (int i=0; i<numRuns; i++){//
        GaussFitParam.push_back(Hist_Fit_1D(Run_info[i].run_number.c_str(), histogramName)); //execute peak fit for the specific run number and histogramand store results in vector
        Run_Number[i]=std::stoi(Run_info[i].run_number);
        Run_mean[i]=GaussFitParam[i].mean;
        Run_sigma[i]=GaussFitParam[i].sigma;
        Run_amp[i]=GaussFitParam[i].amplitude;  
    }
    // Create graphs to plot the fit parameters against the run numbers
    TGraph* tgraph1 = new TGraph(numRuns, &Run_Number[0], &Run_mean[0]);
    tgraph1->SetTitle("Mean Value vs Run Number");
    tgraph1->GetXaxis()->SetTitle("Run Number");
    tgraph1->GetYaxis()->SetTitle("Mean Value");
    //graph1->Draw("AP"); // Draw the graph as points 'P', Draw and scale axis 'A'

    TGraph* tgraph2 = new TGraph(numRuns, &Run_Number[0], &Run_sigma[0]);
    tgraph2->SetTitle("Sigma vs Run Number");
    tgraph2->GetXaxis()->SetTitle("Run Number");
    tgraph2->GetYaxis()->SetTitle("Sigma");
    //graph2->Draw("AP");  

    TGraph* tgraph3 = new TGraph(numRuns, &Run_Number[0], &Run_amp[0]);
    tgraph3->SetTitle("Amplitude vs Run Number");
    tgraph3->GetXaxis()->SetTitle("Run Number");
    tgraph3->GetYaxis()->SetTitle("Amplitude");
    //graph3->Draw("AP");
    //store graphs in vector
    graphVector.push_back(tgraph1);
    graphVector.push_back(tgraph2);
    graphVector.push_back(tgraph3);
    //delete dynamically assigned memory
    delete[] Run_Number;
    delete[] Run_mean;
    delete[] Run_sigma;
    delete[] Run_amp;
    //return graphs
    return graphVector;
} 

std::vector<std::vector<float>> Channel_Value_Slope(const std::vector<DateRunBeam>& Run_info, const char* histogramName){

    // one hist for each run we look over
    int numRuns = Run_info.size();
    double* Run_Number = new double[numRuns];
    
    // Create a vector of vectors to store the values for each bin
    std::vector<std::vector<float>> ValuesPerRun;
    ValuesPerRun.resize(numRuns, std::vector<float>(64 * 24, 0.0));
    //std::vector<std::vector<float>> ValuesPerRun(numRuns, std::vector<float>(64 * 24));//rows x columns
    //open file
    

    // pull up relevant histograms and store data
    for (int l = 0; l < numRuns; ++l) {

        //open each file and load the relevant histogram
        TFile *f = new TFile(Form("run_%s.root", Run_info[l].run_number.c_str()));
        TH2F* htemp = static_cast<TH2F*>(f->Get(histogramName));

        // Loop over each bin and store the value in the corresponding vector element
        for (int row = 0; row < 64; ++row) {
            for (int col = 0; col < 24; ++col) {

                int bin = htemp->GetBin(row + 1, col + 1); // Get the bin number (note: ROOT bins start from 1)
                float value = htemp->GetBinContent(bin); // Get the value of the histogram bin
                ValuesPerRun[l][row * 24 + col] = value; // Store the value in the vector
            }
        }
        f->Close();
        delete f;
    }

    // loop over all bins again and find slopes for each
    std::vector<std::vector<float>> Histogram_eta_phi_slopes(64, std::vector<float>(24, 0.0)); // Initialize to 0.0
    for (int row = 0; row < 64; ++row) {
        for (int col = 0; col < 24; ++col) {
            // Pass the values for each run in that specific bin to Tower_Slope_Fit function
            // Create a vector to store the values for the current bin from each run
            std::vector<float> valuesForBin;
            //valuesForBin.reserve(numRuns);

            // Extract the values for the current bin from each run and store them in valuesForBin
            for (int run = 0; run < numRuns; ++run) {
                valuesForBin.push_back(ValuesPerRun[run][row * 24 + col]);
            }

            Histogram_eta_phi_slopes[row][col]=Tower_Slope_Fit(valuesForBin,numRuns);
        }
    }
    return Histogram_eta_phi_slopes;
}

TGraph2D* slope_TGraph_2D(const std::vector<std::vector<float>>& slopes){
    int numRows = slopes.size();
    int numCols = slopes[0].size();

    // Convert the 2D vector of slopes into a 1D vector
    std::vector<Float_t> slopeValues;
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            slopeValues.push_back(slopes[row][col]);
        }
    }
    // Create a TGraph2D with the slope values
    TGraph2D *graph2D = new TGraph2D();
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            float x = static_cast<float>(col); // X-coordinate (column index)
            float y = static_cast<float>(row); // Y-coordinate (row index)
            float z = slopes[row][col]; // Z-coordinate (slope value)
            graph2D->SetPoint(graph2D->GetN(), x, y, z);
        }
    }
    return graph2D;
}


void RunForEach(std::string fname, std::vector <TH1F*> * histos, bool beam, int run, bool process_full)
{
 //does the full waveform processing, turn to false to run faster
	std::fstream dummy(fname);
	if(! dummy.is_open()) return;
	else dummy.close();
	LEDRunData* leddata=new LEDRunData(towermaper, fname, process_full, run);
	std::cout<<"Run " <<run <<" with beam status " <<beam<<std::endl;
	leddata->ReadInput();
	Fun4AllServer *se =Fun4AllServer::instance();
	se->Verbosity(0);
	std::string inputname("in_run_%d", run);
	Fun4AllPrdfInputPoolManager *in= new Fun4AllPrdfInputPoolManager(inputname);
	try{ 
	in->AddPrdfInputList(fname)->MakeReference(true);
	se->registerInputManager(in);
	se->registerSubsystem(leddata);
	se->run();
	}
	catch (std::exception& e) {return;}

	//leddata->FileOutput();
       	std::map<int, std::vector<LEDRunData::towerinfo>> sector_towers;
	std::cout<<"there are " <<towermaper.size() <<"many channels in play" <<std::endl;
	for(auto t:leddata->towermaps){
	//	std::cout<<"working on packet " <<t.first.first <<" channel " <<t.first.second<<std::endl;
		leddata->CalculateChannelData(t.second); 
		if (leddata->tower_datapts[t.first]["Peak"] == 0){
	//		std::cout<<"No data in packet " <<t.first.first <<" channel " <<t.first.second <<std::endl;
			continue;
		}
		//else std::cout<<"Data in " <<t.first.first <<" channel " <<t.first.second <<std::endl;
	//	std::cout<<"Have finished getting the channel data"<<std::endl;
		//sector_towers[t.second.sector].push_back(t.second); 
		if (beam){
	//		 std::cout<<"Have found the beam" <<std::endl;
			 histos->at(1)->Fill(leddata->tower_datapts[t.first]["Peak"]);
			 histos->at(3)->Fill(leddata->tower_datapts[t.first]["Peak Width"]);
			 histos->at(5)->Fill(leddata->tower_datapts[t.first]["Pedestal RMS"]);
		}
		else{ 
	//		 std::cout<<"No beam" <<std::endl;
			 histos->at(0)->Fill(leddata->tower_datapts[t.first]["Peak"]);
			 histos->at(2)->Fill(leddata->tower_datapts[t.first]["Peak Width"]);
			 histos->at(4)->Fill(leddata->tower_datapts[t.first]["Pedestal RMS"]);
		    }
	}
        leddata->FileOutput();
	/*for( auto s:sector_towers) data->CalculateSectorData(s.second);
	auto sectordata=data->sector_datapts;
	for( auto s:sectordata){
		if (beam) histos->at(7)->SetBinContent(int(s.first.first)*32+s.first.second+1, s.second.at(1));
		else histos->at(6)->SetBinContent(int(s.first.first)*32+s.first.second+1, s.second.at(1));
	}*/
	std::cout<<"Have finished processing run " <<fname <<std::endl;
	//for(auto d:data->datahists) for(auto h:d) h->Delete();
//	in->ResetFileList();
	se->Reset();
	delete leddata; 
//	delete in;
	//delete se;
}

void BuildTowerMap()
{
	for(int i=0; i<32; i++)
	{
		int packetb=i/4+1;
		int packet=packetb;
		for(int j=0; j<48; j++)
		{
			int chn=(i%4)*48+j; 
			bool inout, ns;
			if(j<24) ns=true;
		        else ns=false;
			int etabin, phibin;
			//channel%64 gives the sector, first 16 are lowest sector in ADC and so forth
			//then odds are farther form phi=0
			//get eta using chn%16 to get to the specific position in the cable,
			//then take chn%16/2 to account for doubling in phi 
			//for cable position, chn/64*8 gives the offset  
			etabin=(chn%16)/2+chn/64*8;
			phibin=chn%2+2*i;
		        float eta=(etabin-12)/12+1/24; 
			float phi=(phibin-1)/(2*3.1415);
			for(int k=0; k<=1; k++){
				std::string label;
				if(k==0){
					label=("Inner HCal sector %i, Channel %i", i, chn);
				       	inout=true;
					packet=7000+packetb;
				}
				else{
					label=("outer HCal sector %i, Channel %i", i, chn);
				       	inout=false;//I can try to make new methods
				       	packet=8000+packetb;	
				}
				LEDRunData::towerinfo tower { inout, ns, i, j/2, packet, etabin, phibin, eta, phi, label }; 
				towermaper[std::make_pair(packet, chn)]=tower; 
//				std::cout<<"Packet " <<packet <<std::endl;
			}	
		}
	}
}

int main(int argc, const char* argv[]){
    bool full=false;
    std::string mode (argv[1]);
    if( mode.find("full") != std::string::npos) full=true;
    else full=false;
    std::cout <<"Passed argument is " <<argv[1]<<std::endl;
    std::cout << "Running with the full waveform fitting method :" << full <<std::endl;
    int run=std::stoi(argv[2]);
    bool Beam=false;
    if ( std::string(argv[3]).find("Y") != std::string::npos) Beam=true;
    std::string date (argv[4]);
    BuildTowerMap();
    //--------------------------histograms
    //-----------------parse csv
   /* std::ifstream file("runs_and_time.csv");//specify csv file with format (expected):Date, Run Number, Post-Beam (Y/N) 
    std::string line;
    std::string cell;
    std::vector<std::string> row; // make a vector of strings called "row"
    BuildTowerMap();
    std::vector<int> packets;
    for(auto t:towermaper){
		if(packets.size()==0) packets.push_back(t.first.first);
		else{
			bool unique=true;
			for(auto p:packets) if(t.first.first==p) unique=false;
			if(unique) packets.push_back(t.first.first);
		}
	}
    std::cout<<"Built tower map"<<std::endl;
    std::cout<<"have packet numbers: " <<std::endl;
    for(auto p:packets) std::cout<<p<<std::endl;
    std::vector<DateRunBeam> Run_info;// remember vector.push_back({el1,el2,el3});
    //this struct is {string, int, bool}
    //-------------------------open file
    //SetsPhenixStyle();
    if (!file.is_open()){
        std::cout << "Error opening the file." << std::endl;
        return 1;
    }
    //-------------------------loop over rows in file
    //-------------------------read run info csv in to memory
    while (std::getline(file, line)){ //look at each line in the csv and store it in a string
    	std::istringstream iss(line); // Use a stringstream to split the lines into inputs 
	row.clear();
	while (std::getline(iss, cell, ',')){ //read "iss" stringstream using comma as the delimiter and store the value in the string "cell"
            row.push_back(cell); //store each "cell" in the vector "row"
        }
        // Process the row data as needed
        // Print the elements of each row:      
        // Date, Run Number, Post-Beam (Y/N) 
        for (const auto element : row){
            std::cout << element << " ";
        }
        if(row.size()==0) continue;
        bool Beam_On;// this is a big distraction. 
        // maybe it is better to just write beeam status in the csv as 0 and 1. easier to convert that to bool
        try{ 
		if (row[2]=="N"){
           		 Beam_On=false;
        	}
        	else if(row[2]=="Y"){
            		Beam_On=true;
        	} 
        	else {
            		std::cout << "Error: Unexpected Beam Status" << std::endl;
			continue;
        		//return 1;
        	}
        	Run_info.push_back({row[0], row[1], Beam_On});
	}
	catch(std::exception& e)
	{
		std::cout<<"offending line has exception  " <<Run_info.size() <<std::endl;
	}
        //move on to next row/line (each specific led run)
    }*/
    //create graphs to characterize data
    //-------------------------------------------------------------------
    //1d histograms
    std::cout<<"Processed all files" <<std::endl;
    std::string runfilename="run_data_"+mode+"/avgs/LEDdata_"+std::to_string(run)+"_"+mode+".root";
    //TFile* runfile;
    //runfile->Open(runfilename.c_str(), "RECREATE");
    //TList* l=(TList *) runfile->GetListOfKeys();
    //l->Print();
    TH1F *NoBeamPeak, *BeamPeak, *NoBeamPeakWidth, *BeamPeakWidth, *NoBeamPedestalRMS, *BeamPedestalRMS;
    TH1F *SectorNBPeaks, *SectorBPeaks;
    std::cout<<"Searching for the hists" <<std::endl;
    
    NoBeamPeak=new TH1F("NBP", "LED Peak Height, before Beam; Energy Per Tower [ADC Counts]; N_counts", 200, 0, 4000);
    BeamPeak=new TH1F("BP", "LED Peak Height, after Beam; Energy Per Tower [ADC Counts]; N_counts", 200, 0, 4000);
    NoBeamPeakWidth=new TH1F("NBPW", "LED Peak Width, before Beam; Samples; N_counts", 31, 0, 31);
    BeamPeakWidth=new TH1F("BPW", "LED Peak Width, after Beam; Samples; N_counts", 31, 0, 31);
    NoBeamPedestalRMS=new TH1F("NBPRMS", "LED Pedestal RMS, before Beam; Energy Per Tower [ADC Counts]; N_counts", 100, 0, 25);
    BeamPedestalRMS=new TH1F("BPRMS", "LED Pedestal RMS, after Beam; Energy per Tower [ADC Counts]; N_counts", 100, 0, 25);
    SectorNBPeaks=new TH1F("SNBP", "LED Peak rms before beam in sector; Sector Number; Energy [ADC Counts]", 64, 0, 64);
    SectorBPeaks=new TH1F("SBP", "LED Peak rms after beam in sector; Sector Number; Energy [ADC Counts]", 64, 0, 64);

    std::vector<TH1F*> acc_data {NoBeamPeak, BeamPeak, NoBeamPeakWidth, BeamPeakWidth, NoBeamPedestalRMS, BeamPedestalRMS, SectorNBPeaks, SectorBPeaks};
   std::cout <<"Booked histos"<<std::endl;
  // std::cout<<"nRuns " <<Run_info.size() <<std::endl;
   DateRunBeam Run_info {date,std::to_string(run), Beam};
    // Pull data from the GetLEDData class 
   try{
	//for(auto run:Run_info){
		 std::cout<<"Working on run " <<Run_info.fname<<" with beam status " <<Run_info.Beam <<std::endl;
		 RunForEach(Run_info.fname, &acc_data, Run_info.Beam, run, full); //want to make this command line for the false in a few 
		
	}
   catch(std::exception& e) {}
//   file.close();
    
   TFile* runfile =new TFile(runfilename.c_str(), "RECREATE");
   runfile->cd();
   for(auto h:acc_data) h->Write();
   std::cout<<"Now trying to interact with the file itself"<<std::endl;
    runfile->Write();
   std::cout<<"Wrote histograms to the file" <<std::endl;
    runfile->Close();
    std::cout<<"Closed the file" <<std::endl;
   /* const char* ohcalhistname="h_peak_ohcal";
    const char* ihcalhistname="h_peak_ihcal";
    std::vector<TGraph*> gauss_Peak_graphs_ohcal = CreateTGraphVector(Run_info, ohcalhistname);//create a vector of tgraphs for sigma, mean, amp
    std::vector<TGraph*> gauss_Peak_graphs_ihcal = CreateTGraphVector(Run_info, ihcalhistname);
    // 2d histograms. add any you want
    //These are the ones of interest from silas' branch of hanpu's code
    
    const char *h_2D_pedestal_ohcal;
    const char *h_2D_pedestal_ihcal;
    const char *h_2D_pederms_ohcal;
    const char *h_2D_pederms_ihcal;
    const char *h_2D_peak_ohcal;
    const char *h_2D_peak_ihcal;
    const char *h_2D_peakstd_ohcal;
    const char *h_2D_peakstd_ihcal;
    //    
    
    std::vector<std::vector<float>> pedestal_ohcal_slopes_2D=Channel_Value_Slope(Run_info, h_2D_pedestal_ohcal);
    std::vector<std::vector<float>> pedestal_ihcal_slopes_2D=Channel_Value_Slope(Run_info, h_2D_pedestal_ihcal);

    std::vector<std::vector<float>> pederms_ohcal_slopes_2D=Channel_Value_Slope(Run_info, h_2D_pederms_ohcal);
    std::vector<std::vector<float>> pederms_ihcal_slopes_2D=Channel_Value_Slope(Run_info, h_2D_pederms_ihcal);

    std::vector<std::vector<float>> peak_ohcal_slopes_2D=Channel_Value_Slope(Run_info, h_2D_peak_ohcal);
    std::vector<std::vector<float>> peak_ihcal_slopes_2D=Channel_Value_Slope(Run_info, h_2D_peak_ihcal);

    //std::vector<std::vector<float>> 2D_peakstd_ohcal_slopes=Channel_Value_Slope(Run_info, const char* h_2D_peakstd_ohcal);
    //std::vector<std::vector<float>> 2D_peakstd_ihcal_slopes=Channel_Value_Slope(Run_info, const char* h_2D_peakstd_ihcal);    
    //loop over bins and make 2d histograms for each.
    /*
    TH2F *h_2D_peak_ohcal = new TH2F("h_2D_peak_ohcal","",24,0.,24.,64,0.,64.);
	h_2D_peak_ohcal->GetXaxis()->SetTitle("ieta");
	h_2D_peak_ohcal->GetYaxis()->SetTitle("iphi");
	TH2F *h_2D_peak_ihcal = new TH2F("h_2D_peak_ihcal","",24,0.,24.,64,0.,64.);
	h_2D_peak_ihcal->GetXaxis()->SetTitle("ieta");
	h_2D_peak_ihcal->GetYaxis()->SetTitle("iphi");
	TH2F *h_2D_peakstd_ohcal = new TH2F("h_2D_peakstd_ohcal","",24,0.,24.,64,0.,64.);
	h_2D_peakstd_ohcal->GetXaxis()->SetTitle("ieta");
	h_2D_peakstd_ohcal->GetYaxis()->SetTitle("iphi");
	TH2F *h_2D_peakstd_ihcal = new TH2F("h_2D_peakstd_ihcal","",24,0.,24.,64,0.,64.);
	h_2D_peakstd_ihcal->GetXaxis()->SetTitle("ieta");
	h_2D_peakstd_ihcal->GetYaxis()->SetTitle("iphi");
    TH2F *h_2D_pedestal_ohcal = new TH2F("h_2D_pedestal_ohcal","",24,0.,24.,64,0.,64.);
	h_2D_pedestal_ohcal->GetXaxis()->SetTitle("ieta");
	h_2D_pedestal_ohcal->GetYaxis()->SetTitle("iphi");
	TH2F *h_2D_pedestal_ihcal = new TH2F("h_2D_pedestal_ihcal","",24,0.,24.,64,0.,64.);
	h_2D_pedestal_ihcal->GetXaxis()->SetTitle("ieta");
	h_2D_pedestal_ihcal->GetYaxis()->SetTitle("iphi");
	//h_2D_pederms_ohcal = new TH2F("h_2D_pederms_ohcal","",24,0.,24.,64,0.,64.);
	//h_2D_pederms_ohcal->GetXaxis()->SetTitle("ieta");
	//h_2D_pederms_ohcal->GetYaxis()->SetTitle("iphi");
	//h_2D_pederms_ihcal = new TH2F("h_2D_pederms_ihcal","",24,0.,24.,64,0.,64.);
	//h_2D_pederms_ihcal->GetXaxis()->SetTitle("ieta");
	//h_2D_pederms_ihcal->GetYaxis()->SetTitle("iphi");

    for (int row = 0; row < 64; ++row) {
        for (int col = 0; col < 24; ++col) {
            TH2F *h_2D_pedestal_ohcal;
            TH2F *h_2D_pedestal_ihcal;
            TH2F *h_2D_pederms_ohcal;
            TH2F *h_2D_pederms_ihcal;
            TH2F *h_2D_peak_ohcal;
            TH2F *h_2D_peak_ihcal;
        }
    }
    
    TGraph2D *pedestal_ohcal_graph2D = slope_TGraph_2D(pedestal_ohcal_slopes_2D);
    TGraph2D *pedestal_ihcal_graph2D = slope_TGraph_2D(pedestal_ihcal_slopes_2D);
    TGraph2D *pederms_ohcal_graph2D = slope_TGraph_2D(pederms_ohcal_slopes_2D);
    TGraph2D *pederms_ihcal_graph2D = slope_TGraph_2D(pederms_ihcal_slopes_2D);
    TGraph2D *peak_ohcal_graph2D = slope_TGraph_2D(peak_ohcal_slopes_2D);
    TGraph2D *peak_ihcal_graph2D = slope_TGraph_2D(peak_ihcal_slopes_2D);
    //------------------------------------------------------------------
    // Create a TCanvas to draw the TGraphs
    // canvas for 1d histograms
    TCanvas* c1 = new TCanvas("canvas1", "Peak Parameter graphs", 1800, 1500);
    c1->Divide(1,2);//divide in to two pads. one for ohcal. one for ihcal
    // columns, rows// need 3 columns each for ohcal and ihcal
    
    
    /* // maybe I can clean this up and do it in a loop later
    for (size_t i = 0; i < myGraphs.size(); ++i) {
        canvas->cd(i+1); // Activate the canvas
        CreateTGraphVector[i]->Draw((i == 0) ? "APL" : "PL"); // Use "APL" for the first graph, "PL" for the rest
    }


    //-------------------------------------------
    //top pad will be ihcal
    TPad* TopPad = (TPad*)c1->cd(1);
    //divide in to 3 columns for peak, sigma, amp
    TopPad->Divide(3,1);
    //ihcal peak mean
    TopPad->cd(1);
    gPad->SetTopMargin(0.12);
    gPad->SetFillColor(33);
    gauss_Peak_graphs_ihcal[1]->Draw("A");
    //ihcal peak sigma
    TopPad->cd(2);
    gPad->SetTopMargin(0.12);
    gPad->SetLeftMargin(0.15);
    gPad->SetFillColor(33);
    gauss_Peak_graphs_ihcal[2]->Draw("A");
    //ihcal peak amplitude
    TopPad->cd(3);
    gPad->SetTopMargin(0.12);
    gPad->SetLeftMargin(0.15);
    gPad->SetFillColor(33);
    gauss_Peak_graphs_ihcal[3]->Draw("A");
    
    //-----------------------------------------
    //bottom pad is ohcal
    TPad* BotPad = (TPad*)c1->cd(2);
    //divide in to 3 columns for peak, sigma, amp
    BotPad->Divide(3,1);
    //ihcal peak mean
    BotPad->cd(1);
    gPad->SetTopMargin(0.12);
    gPad->SetFillColor(33);
    gauss_Peak_graphs_ihcal[1]->Draw("A");
    //ihcal peak sigma
    BotPad->cd(2);
    gPad->SetTopMargin(0.12);
    gPad->SetLeftMargin(0.15);
    gPad->SetFillColor(33);
    gauss_Peak_graphs_ihcal[2]->Draw("A");
    //ihcal peak amplitude
    BotPad->cd(3);
    gPad->SetTopMargin(0.12);
    gPad->SetLeftMargin(0.15);
    gPad->SetFillColor(33);
    gauss_Peak_graphs_ihcal[3]->Draw("A");
    //-----------------------------------------
    // Keep the canvas open to display the graph
    c1->Update();
    //canvas->Modified();
    c1->WaitPrimitive();//wait for the user to close the program
    //*
    c1->Print("hcal_peak_parameters.pdf");

    //-------------------------------------------
    //canvas for 2d histograms/tgraphs.
    TCanvas* c2 = new TCanvas("canvas2", "2D Slope Graphs", 1800, 1500);
    c2->Divide(1,3);//divide in to three pads. one for each set of (ohcal+ihcal) 2d histograms
    //-------------------------------------------
    //left pad will be h_2D_pedestal_ohcal
    TPad* LeftPad = (TPad*)c2->cd(1);
    //divide in to 3 columns for peak, sigma, amp
    LeftPad->Divide(1,2);
    //ihcal peak mean
    LeftPad->cd(1);
    gPad->SetTopMargin(0.12);
    gPad->SetFillColor(33);   
    pedestal_ohcal_graph2D->GetXaxis()->SetTitle("Column Index");
    pedestal_ohcal_graph2D->GetYaxis()->SetTitle("Row Index");
    pedestal_ohcal_graph2D->GetZaxis()->SetTitle("Slope Value");
    pedestal_ohcal_graph2D->Draw("COLZ");
    //ihcal peak sigma
    LeftPad->cd(2);
    gPad->SetTopMargin(0.12);
    gPad->SetLeftMargin(0.15);
    gPad->SetFillColor(33);
    pedestal_ihcal_graph2D->GetXaxis()->SetTitle("Column Index");
    pedestal_ihcal_graph2D->GetYaxis()->SetTitle("Row Index");
    pedestal_ihcal_graph2D->GetZaxis()->SetTitle("Slope Value");
    pedestal_ihcal_graph2D->Draw("COLZ");  
    //-----------------------------------------

    //middle pad will be h_2D_pederms_ohcal_graph2D
    TPad* MiddlePad = (TPad*)c2->cd(2);
    //divide in to 3 columns for peak, sigma, amp
    MiddlePad->Divide(1,2);
    //ihcal peak mean
    MiddlePad->cd(1);
    gPad->SetTopMargin(0.12);
    gPad->SetFillColor(33);   
    pederms_ohcal_graph2D->GetXaxis()->SetTitle("Column Index");
    pederms_ohcal_graph2D->GetYaxis()->SetTitle("Row Index");
    pederms_ohcal_graph2D->GetZaxis()->SetTitle("Slope Value");
    pederms_ohcal_graph2D->Draw("COLZ");
    //ihcal peak sigma
    MiddlePad->cd(2);
    gPad->SetTopMargin(0.12);
    gPad->SetLeftMargin(0.15);
    gPad->SetFillColor(33);
    pederms_ihcal_graph2D->GetXaxis()->SetTitle("Column Index");
    pederms_ihcal_graph2D->GetYaxis()->SetTitle("Row Index");
    pederms_ihcal_graph2D->GetZaxis()->SetTitle("Slope Value");
    pederms_ihcal_graph2D->Draw("COLZ");  
    //-----------------------------------------

    //right pad will be h_2D_peak_ohcal_graph2D
    TPad* RightPad = (TPad*)c2->cd(3);
    //divide in to 3 columns for peak, sigma, amp
    RightPad->Divide(1,2);
    //ihcal peak mean
    RightPad->cd(1);
    gPad->SetTopMargin(0.12);
    gPad->SetFillColor(33);   
    peak_ohcal_graph2D->GetXaxis()->SetTitle("Column Index");
    peak_ohcal_graph2D->GetYaxis()->SetTitle("Row Index");
    peak_ohcal_graph2D->GetZaxis()->SetTitle("Slope Value");
    peak_ohcal_graph2D->Draw("COLZ");
    //ihcal peak sigma
    RightPad->cd(2);
    gPad->SetTopMargin(0.12);
    gPad->SetLeftMargin(0.15);
    gPad->SetFillColor(33);
    peak_ohcal_graph2D->GetXaxis()->SetTitle("Column Index");
    peak_ohcal_graph2D->GetYaxis()->SetTitle("Row Index");
    peak_ohcal_graph2D->GetZaxis()->SetTitle("Slope Value");
    peak_ohcal_graph2D->Draw("COLZ");  
    //-----------------------------------------

    c2->Update();
    //c2->Modified();
    //c2->WaitPrimitive();//wait for the user to close the program
    //*
    c2->Print("hcal_2D_Histogram_Slopes.pdf");
    //-------------------------------------------
    //clean up dynamically allocated memory
    for (const auto& graph : gauss_Peak_graphs_ohcal) {
        delete graph;
    }
    for (const auto& graph : gauss_Peak_graphs_ihcal) {
        delete graph;
    }
    delete c1;
    delete c2;
    delete ohcalhistname;
    delete ihcalhistname; 
*/
    std::cout<<"End of File" <<std::endl;
    return 0;
}

