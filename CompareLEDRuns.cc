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
#include <TF1.h>
#include <TCanvas.h>

//structures here for now. I can move it later.
struct GaussFitResult {
    //custom structure to hold fit parameter data. right now I have field for ihcal and ohcal. I could actually return a vector of my strutcure and only have fields for the parameters in general, but it may be better to keep it as is, since I expect to have ih and oh data for every set.
    double mean_ih;
    double sigma_ih;
    double amplitude_ih;
    double mean_oh;
    double sigma_oh;
    double amplitude_oh;
};

struct DateRunBeam {
    //custom structure to hold fit parameter data. right now I have field for ihcal and ohcal. I could actually return a vector of my strutcure and only have fields for the parameters in general, but it may be better to keep it as is, since I expect to have ih and oh data for every set.
    std::string date;
    std::string run_number;
    bool Beam; //No->0, Yes->1
};

//written like this to start. Can be moved later
//function declarations
//std::vector<float> Peak_Hist_Fit(const char* filename);//call function wih filename, and recieve fit parameters for ohcal and ihcal peak
GaussFitResult Peak_Hist_Fit(const char* filename);//new version of above using structure to store the fit parameters.


//function definitions
//std::vector<float> Peak_Hist_Fit(const char* filename){
GaussFitResult Peak_Hist_Fit(const char* filename){// use my custom structure, should I re do this to return the gauss fit paramaters for a specific histogram in a specific file?
    //open the root file called filename.root
    TFile *f=new TFile(Form("run_%s.root", filename));
    // pull up relevant histograms
    TH1F *h_peak_ohcal=(TH1F*)f->Get("h_peak_ohcal");
    TH1F *h_peak_ihcal=(TH1F*)f->Get("h_peak_ihcal");
    //Fit the two histograms
    h_peak_ohcal->Fit("gaus");
    h_peak_ihcal->Fit("gaus");
    //create calls to the fits
    TF1 *fit_oh = h_peak_ohcal->GetFunction("gaus");
    TF1 *fit_ih = h_peak_ihcal->GetFunction("gaus");


    /* redundant section. I made a structure to hold the information.
    //read out parameters
    //-----outer hcal
    float Mean_oh = fit_oh->GetParameter(0);
    float Sigma_oh = fit_oh->GetParameter(1);
    float Amplitude_oh = fit_oh->GetParameter(2);
    //-----inner hcal
    float Mean_ih = fit_ih->GetParameter(0);
    float Sigam_ih = fit_ih->GetParameter(1);
    float Amplitude_ih = fit_ih->GetParameter(2);
    */
    // can do the same for fit errors
    //float e1 = fit->GetParError(0);// error of the first parameter

    //delete un-needed things. I wonder if I actually need to do this explicity.
    // I guess I only need to delete the dynamically assigned (new) variables
    //f->Close();
    delete f;// f is dynamically assigned so I should manually delete it.
    //delete h_peak_ohcal;
    //delete h_peak_ihcal;
    //delete fit_oh;
    //delete fit_ih;
    //delete canvas;

    //fill and return vector of parameters
    /*
    std::vector<float> vec_Param;
    vec_Param.push_back(Mean_oh);
    vec_Param.push_back(Sigma_oh);
    vec_Param.push_back(Amplitude_oh);

    vec_Param.push_back(Mean_ih);
    vec_Param.push_back(Sigma_ih);
    vec_Param.push_back(Amplitude_ih);
    return vec_Param;
    */
    GaussFitResult FitParam;

    FitParam.mean_ih = fit_ih->GetParameter(1);
    FitParam.sigma_ih = fit_ih->GetParameter(2);
    FitParam.amplitude_ih = fit_ih->GetParameter(0);

    FitParam.mean_oh = fit_oh->GetParameter(1);
    FitParam.sigma_oh = fit_oh->GetParameter(2);
    FitParam.amplitude_oh = fit_oh->GetParameter(0);

    return FitParam;
}


int main{
    //--------------------------histograms
    // I should really define these in the header if possible.

    //these would make more sense as Tgraphs...
    /*
    TH1F* hPeakInnerAmp=new TH1F("hPeakInnerAmp","Inner HCal: Peak Amplitude", 100,0,100); 
    TH1F* hPeakInnerSigma=new TH1F("hPeakInnerSigma","Inner HCal: Peak Sigma", 100,0,100);
    TH1F* hPeakInnerMean=new TH1F("hPeakInnerMean","Inner HCal: Peak Mean", 100,0,100);
    TH1F* hPeakOuterAmp=new TH1F("hPeakOnnerAmp","Outer HCal: Peak Amplitude", 100,0,100); 
    TH1F* hPeakOuterSigma=new TH1F("hPeakOnnerSigma","Outer HCal: Peak Sigma", 100,0,100);
    TH1F* hPeakOuterMean=new TH1F("hPeakOnnerMean","Outer HCal: Peak Mean", 100,0,100);
    *///
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
    // simple way to compare is make averages & devaition from that avg
    // also trendline of avg and relative error over time
    // maybe averages for each day since there are multiple runs per day? + general average for all before beam + avg for each month(added bonus of nicely seperating pre and post beam months)
    // std::cout << " Filler" << ;

    //-------------------------process run data
    vector<GaussFitResult> GaussFitParam;
    for (int i=0; i<run+_info.size(); i++){
        GaussFitParam.push_back(Peak_Hist_Fit(run_info[i,1].c_str()));
        //hPeakInnerAmp.Fill();   
    }
    //I may need to convert the date to an int


    //auto g1 = new TGraph(run_info[:,0],y);
    //auto g2 = new TGraph(run_info[:,0],y);
    //auto g3 = new TGraph(run_info[:,0],y);


    file.close();
    return 1;
}
