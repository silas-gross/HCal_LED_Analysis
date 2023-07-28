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


//written like this to start. To be moved later
//function declarations
std::vector<float> Peak_Hist_Fit(const char* filename);//call function wih filename, and recieve fit parameters for ohcal and ihcal peak



//function definitions
std::vector<float> Peak_Hist_Fit(const char* filename){
    //open the root file called filename.root
    TFile *f=new TFile(Form("%s.root", filename));
    // pull up relevant histograms
    TH1F *h_peak_ohcal=(TH1F*)f->Get("h_peak_ohcal");
    TH1F *h_peak_ihcal=(TH1F*)f->Get("h_peak_ihcal");
    //Fit the two histograms
    h_peak_ohcal->Fit("gaus");
    h_peak_ihcal->Fit("gaus");
    //create calls to the fits
    TF1 *fit_oh = h_peak_ohcal->GetFunction("gaus");
    TF1 *fit_ih = h_peak_ihcal->GetFunction("gaus");
    //read out parameters
    //-----outer hcal
    float Mean_oh = fit_oh->GetParameter(0);
    float Sigma_oh = fit_oh->GetParameter(0);
    float Amplitude_oh = fit_oh->GetParameter(0);
    //-----inner hcal
    float Mean_ih = fit_ih->GetParameter(0);
    float Sigam_ih = fit_ih->GetParameter(0);
    float Amplitude_ih = fit_ih->GetParameter(0);

    // can do the same for fit errors
    //float e1 = fit->GetParError(0);// error of the first parameter

    //delete un-needed things. I wonder if I actually need to do this explicity.
    //f->Close();
    delete f;
    delete h_peak_ohcal;
    delete h_peak_ihcal;
    delete fit_oh;
    delete fit_ih;
    //delete canvas;

    //fill and return vector of parameters
    std::vector<float> vec_Param;
    vec_Param.push_back(Mean_oh);
    vec_Param.push_back(Sigma_oh);
    vec_Param.push_back(Amplitude_oh);

    vec_Param.push_back(Mean_ih);
    vec_Param.push_back(Sigma_ih);
    vec_Param.push_back(Amplitude_ih);
    return vec_Param;
}


void main{
    std::ifstream file("runs_and_time.csv");//specify csv file with format (expected):Date, Run Number, Post-Beam (Y/N) 
    std::string line;
    std::istringstream iss(line); // Use a stringstream to split the lines into inputs
    std::string cell;
    std::vector<std::string> row; // make a vector of strings called "row"
    //--------------open file
    if (!file.is_open()){
        std::cout << "Error opening the file." << std::endl;
        return 1;
    }
    //-------------loop over rows in file
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
        
        //move on to next row/line (each specific led run)
    }
    // simple way to compare is make averages & devaition from that avg
    // also trendline of avg and relative error over time
    // maybe averages for each day since there are multiple runs per day? + general average for all before beam + avg for each month(added bonus of nicely seperating pre and post beam months)
    // std::cout << " Filler" << ;
    file.close();
    return;
}
