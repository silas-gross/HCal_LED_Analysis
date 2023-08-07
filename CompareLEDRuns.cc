//written by Nikhil Kumar, inital commit 7/21/2023
// headers
#include <CompareLEDRuns.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TH1F.h>//a lot of these maybe not needed.
#include <TH2F.h>
#include <TF1.h>
#include <TF2.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TPad.h>
#include <TText.h>
#include <TStyle.h>
#include <TRoot.h>


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
};

//function declarations
GaussFitResult Hist_Fit_1D(const char* filename, const char* histogramName);// pass a filename and a TH1F histname and recive gaussian fit parameters
float Tower_Slope_Fit(vector<float> TowerPeaks);// pass a vector of peak values for a single tower over many runs and recieve the  value vs time slope
std::vector<TGraph*> CreateTGraphVector(const std::vector<DateRunBeam>& run_info, const char* histogramName); // pass a vector of all run_info (defined in the csv) and make tgraphs of gaussian sigma, mean, amp using Hist_Fit_1D

//function definitions
GaussFitResult Hist_Fit_1D(const char* filename, const char* histogramName){
    //open the root file called filename.root
    TFile *f = new TFile(Form("run_%s.root", filename));
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

float Tower_Slope_Fit(vector<float> TowerPeaks);{
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

std::vector<TGraph*> CreateTGraphVector(const std::vector<DateRunBeam>& run_info, const char* histogramName){
    std::vector<TGraph*> graphVector;
    // Extract data from the vectors and create arrays
    int numRuns = run_info.size();
    double* Run_Number = new double[numRuns];
    double* Run_mean = new double[numRuns];
    double* Run_sigma = new double[numRuns];
    double* Run_amp = new double[numRuns];
    //-------------------------process run data
    vector<GaussFitResult> GaussFitParam;
    for (int i=0; i<numRuns; i++){//
        GaussFitParam.push_back(Hist_Fit_1D(run_info[i].run_number.c_str(), histogramName)); //execute peak fit for the specific run number and histogramand store results in vector
        Run_Number[i]=std::stoi(run_info[i].run_number);
        Run_mean[i]=GaussFitParam[i].mean;
        Run_sigma[i]=GaussFitParam[i].sigma;
        Run_amp[i]=GaussFitParam[i].amp;  
    }
    // Create graphs to plot the fit parameters against the run numbers
    TGraph* tgraph1 = new TGraph(numRuns, &Run_Number[0], &Run_mean[0]);
    graph1->SetTitle("Mean Value vs Run Number");
    graph1->GetXaxis()->SetTitle("Run Number");
    graph1->GetYaxis()->SetTitle("Mean Value");
    //graph1->Draw("AP"); // Draw the graph as points 'P', Draw and scale axis 'A'

    TGraph* tgraph2 = new TGraph(numRuns, &Run_Number[0], &Run_sigma[0]);
    graph2->SetTitle("Sigma vs Run Number");
    graph2->GetXaxis()->SetTitle("Run Number");
    graph2->GetYaxis()->SetTitle("Sigma");
    //graph2->Draw("AP");  

    TGraph* tgraph3 = new TGraph(numRuns, &Run_Number[0], &Run_amp[0]);
    graph3->SetTitle("Amplitude vs Run Number");
    graph3->GetXaxis()->SetTitle("Run Number");
    graph3->GetYaxis()->SetTitle("Amplitude");
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

//main program
int main{
    //--------------------------histograms

    //-----------------parse csv
    std::ifstream file("runs_and_time.csv");//specify csv file with format (expected):Date, Run Number, Post-Beam (Y/N) 
    std::string line;
    std::istringstream iss(line); // Use a stringstream to split the lines into inputs
    std::string cell;
    std::vector<std::string> row; // make a vector of strings called "row"

    std::vector<DateRunBeam> Run_info;// remember vector.push_back({el1,el2,el3});
    //this struct is {string, int, bool}
    //-------------------------open file
    if (!file.is_open()){
        std::cout << "Error opening the file." << std::endl;
        return 1;
    }
    //-------------------------loop over rows in file
    //-------------------------read run info csv in to memory
    while (std::getline(file, line)){ //look at each line in the csv and store it in a string
        while (std::getline(iss, cell, ',')){ //read "iss" stringstream using comma as the delimiter and store the value in the string "cell"
            row.push_back(cell); //store each "cell" in the vector "row" 
        }
        // Process the row data as needed
        // Print the elements of each row:      
        // Date, Run Number, Post-Beam (Y/N) 
        for (const auto &element : row){
            std::cout << element << " ";
        }
        std::cout << std::endl;
        bool Beam_On;// this is a big distraction. 
        // maybe it is better to just write beeam status in the csv as 0 and 1. easier to convert that to bool
        if (strcmp(row[2],"N")){
            Beam_On=0;
        }
        else if(strcmp(row[2],"Y")){
            Beam_on=1;
        } 
        else {
            std::cout << "Error: Unexpected Beam Status" << std::endl;
        return 1;
        }
        run_info.push_back({row[0], row[1], Beam_on});
        //move on to next row/line (each specific led run)
    }
    const char* ohcalhistname="h_peak_ohcal";
    const char* ihcalhistname="h_peak_ihcal";
    std::vector<TGraph*> gauss_Peak_graphs_ohcal = CreateTGraphVector(run_info, ohcalhistname);
    std::vector<TGraph*> gauss_Peak_graphs_ihcal = CreateTGraphVector(run_info, ihcalhistname);
    // Create a TCanvas to draw the TGraphs
    TCanvas* c1 = new TCanvas("canvas1", "Peak Parameter graphs", 1800, 1500);
    c1->Divide(1,2);//divide in to two pads. one for ohcal. one for ihcal
    // columns, rows// need 3 columns each for ohcal and ihcal
    //create a vector of tgraphs for sigma, mean, amp
    
    /* // maybe I can clean this up and do it in a loop later
    for (size_t i = 0; i < myGraphs.size(); ++i) {
        canvas->cd(i+1); // Activate the canvas
        CreateTGraphVector[i]->Draw((i == 0) ? "APL" : "PL"); // Use "APL" for the first graph, "PL" for the rest
    }*/

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
    
    //-------------------------------------------
    // Keep the canvas open to display the graph
    c1->Update();
    //canvas->Modified();
    c1->WaitPrimitive();//wait for the user to close the program
    //*
    

    //clean up dynamically allocated memory
    for (const auto& graph : gauss_Peak_graph_ohcal) {
        delete graph;
    }
    for (const auto& graph : gauss_Peak_graph_ihcal) {
        delete graph;
    }
    delete canvas1;
    delete ohcalhistname;
    delete ihcalhistname;
    //*/
    file.close();
    return 1;
}
void RunForEach(std::string fname)
{
	GetLedData* data=new GetLedData(towermap, fname);
	data->ReadInput();
	Fun4AllServer *se =Fun4AllServer::instance();
	se->Verbosity(0);
	Fun4AllPrdfInputPoolManager* in= new Fun4AllPrdfInputPoolManager("in");
	in->AddPrdfInputList(filename);
	se->registerInputManager(in);
	se->registerSubsystem(data);
	se->run();
	data->FileOutput();
       	std::map<int, std::vector<towerinfo>> sector_towers;
	for(auto t:data->towermaps){
	       	data->CalculateChannelData(t); 
		sector_towers[t.sector].push_back(t); 
	}
	for( auto s:sector_towers) data->CalculateSectorData(s);
	auto sectordata=data->sector_datapts;

}	
