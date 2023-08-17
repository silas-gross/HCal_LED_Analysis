// written by Nikhil Kumar, inital commit 7/21/2023
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include "CompareLEDRuns.h"
R__LOAD_LIBRARY(libCompareLEDRuns.so)
void CompareLEDRuns(std::string infile)
{
	SetsPhenixStyle();
	gStyle->SetOptStat(0);
}
