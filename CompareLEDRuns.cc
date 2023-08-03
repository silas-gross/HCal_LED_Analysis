//written by Nikhil Kumar, inital commit 7/21/2023
// headers
#include <CompareLEDRuns.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TH1F.h>//maybe not needed.
#include <TH2F.h>
#include <TF1.h>
#include <TF2.h>
#include <TCanvas.h>
#include <TGraph.h>
// I need to characterize: Ped, peak, ped_rms, peak location, peak width
//structures here for now. I can move it later.
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

//function declarations//written like this to start. Can be moved later
GaussFitResult Hist_Fit_1D(const char* filename, TH1F* histogram);
float Tower_Slope_Fit(vector<float> TowerPeaks);

//function definitions
GaussFitResult Hist_Fit_1D(const char* filename, TH1F* histogram){
    //open the root file called filename.root
    TFile *f = new TFile(Form("run_%s.root", filename));
    // pull up relevant histograms
    //TH1F *htemp=(TH1F*)f->Get(Form("%s", histogram));// c style cast, should work but I will try something else
     TH1F* htemp = static_cast<TH1F*>(f->Get(histogram->GetName()));//c++ style cast
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
        run_info.push_back({row[0],row[1],Beam_on});
        //move on to next row/line (each specific led run)
    }
    // std::cout << " Filler" << ;

    // Extract data from the vectors and create arrays
    int numRuns = run_info.size();
    double* Run_Number = new double[numRuns];
    double* Run_mean = new double[numRuns];
    double* Run_sigma = new double[numRuns];
    double* Run_amp = new double[numRuns];
    //-------------------------process run data
    vector<GaussFitResult> GaussFitParam;
    for (int i=0; i<numRuns; i++){//
        GaussFitParam.push_back(Hist_Fit_1D(run_info[i,1].c_str())); //execute peak fit for the specific run number and store results in vector
        Run_Number[i]=std::stoi(run_info[i].run_number);
        Run_mean[i]=run_info[i].mean_ih;
        Run_sigma[i]=run_info[i].sigma_ih;
        Run_amp[i]=run_info[i].amplitude_ih;
    }
    // Create graphs to plot the fit parameters against the run numbers
    TGraph* tgraph1 = new TGraph(numRuns, &Run_Number[0], &Run_mean[0]);
    TGraph* tgraph2 = new TGraph(numRuns, &Run_Number[0], &Run_sigma[0]);
    TGraph* tgraph3 = new TGraph(numRuns, &Run_Number[0], &Run_amp[0]);
    // Create a canvas to display the graph
    TCanvas* canvas1 = new TCanvas("canvas1", "Fit Parameters vs Run Number", 800, 600);
    graph1->SetTitle("Mean Value vs Run Number");
    graph1->GetXaxis()->SetTitle("Run Number");
    graph1->GetYaxis()->SetTitle("Mean Value");
    graph2->SetTitle("Sigma vs Run Number");
    graph2->GetXaxis()->SetTitle("Run Number");
    graph2->GetYaxis()->SetTitle("Sigma");
    graph3->SetTitle("Amplitude vs Run Number");
    graph3->GetXaxis()->SetTitle("Run Number");
    graph3->GetYaxis()->SetTitle("Amplitude");
    graph1->Draw("AP"); // Draw the graph as points 'P', Draw and scale axis 'A'
    graph2->Draw("AP");  
    graph3->Draw("AP");

    // Keep the canvas open to display the graph
    //canvas->Update();
    //canvas->Modified();
    //canvas->WaitPrimitive();
    

    file.close();
    return 1;
}
