# HCal_LED_Analysis
## LED health analysis for sPHENIX HCal
Author: Silas 
Last Update: 19 July 2023
Version 1.0

### Purpose 
This analysis is designed to read direct from prdf data files and get pedestal height, pulse width and pulse above pedestal signals to compare health of the HCal SiPMs over time as they are more exposed to beam 

### Files
GetLEDData.cc: Provides a pmonitor based analyis of a single run (two prdfs--one East, one West) that extracts relevant datapoints and outputs root file with relevant Histograms 

CompareLEDs.cc: Allows for comparison across multiple outputs from the GetLEDData.cc

 
