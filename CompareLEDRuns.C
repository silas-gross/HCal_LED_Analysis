// written by Nikhil Kumar, inital commit 7/21/2023
// headers
#include <CompareLEDRuns.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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
        for (const auto &element : row)
        {
            std::cout << element << " ";
        }




        std::cout << std::endl;
        //move on to next row/line (each specific led run)
    }
    // simple way to compare is make averages & devaition from that avg
    // also trendline of avg and relative error over time
    // std::cout << " Filler" << ;
    file.close();
    return;
}
