#include "GetLEDData.h"
#include <TFile.h>
#include <TString.h>

LEDRunData::getPedestal(std::vector<int> chl_data) //just gets the pedestal value from first 3 samples
{
	a; 
}
LEDRunData::getPeak(std::vector<int> chl_data, int pedestal) //gets oeak value, peak position, peak width, pedestal rms
{
	a;
}
LEDRunData::process_event(Event *e){
	
	for(auto pid:packets){
		float evtval=0; 
		Packet* p=e->GetPacket(p);
		if(!p) continue;
	       for(int c=0; c<p->iValue(0, "CHANNELS"); c++){
		 std::vector<int> channel_data;	
		 for(auto s=0; s<31; s++){
	 		evtval+=p->iValue(s, c);
			channel_data.push_back(p->iValue(s,c); 	
		}
		int pedestal=getPedestal(channel_data);
		std::vector<float> getPeak(channel_data, pedestal);
		std::pair<int, int> location {p,c};
		datahists[location].
	       }
	}	       
}
LEDRunData::ReadInput(){
	pListopen(filename);
	for(auto pid:packets)
		for(int c=0; c<192; c++){
			std::pair<int, int> id {p,c};
			int sector=0, ioi=pid/1000, tower=c%48; 
			std::string InnerOuter;
			if(ioi%2) InnerOuter="Outer"; 
			else InnerOuter="Inner";
			sector=pid%8+c/48;
			TH1F* hnew=new TH1F(Form("hcal_packet_%d_channel_%d",pid, c).c_str(), Form("Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower).c_std(), 1000, 0, 100000); 
			//Right now this is hard coded, but I should really use the tower map structure
			//Implement as a lookup table
			TH1F* hnew1=new TH1F(Form("hcal_packet_%d_channel_%d_peak",pid, c).c_str(), Form("Peak Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower).c_std(), 1000, 0, 100000); 
			TH1F* hnew2=new TH1F(Form("hcal_packet_%d_channel_%d_pedestal",pid, c).c_str(), Form("Pedestal Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower).c_std(), 1000, 0, 100000); 
			TH1F* hnew3=new TH1F(Form("hcal_packet_%d_channel_%d_rms",pid, c).c_str(), Form("Pedestal RMS Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower).c_std(), 1000, 0, 100); 
			TH1F* hnew4=new TH1F(Form("hcal_packet_%d_channel_%d_peak_location",pid, c).c_str(), Form("Peak Location for %s HCal, sector %d, tower %d; Time Sample; n Events", InnerOuter, sector, tower).c_std(), 31, 0, 31); 
			TH1F* hnew5=new TH1F(Form("hcal_packet_%d_channel_%d_peak_width",pid, c).c_str(), Form("Peak width for %s HCal, sector %d, tower %d; Time Sample; n Events", InnerOuter, sector, tower).c_std(), 31, 0, 31); 
			datahists[id].push_back(hnew);
			datahists[id].push_back(hnew1); 
			datahists[id].push_back(hnew2); 
			datahists[id].push_back(hnew3);	
			datahists[id].push_back(hnew4);
			datahists[id].push_back(hnew5);

		}
}
LEDRunData::FileOutput(){
	a;
}
LEDRunData::CalculateChannelData(towerinfo tower){
	a;
}
LEDRunData::CalculateSectorData(std::vector<towerinfo> sector){
	a;
}
LEDRunData::CalculateMPODData(int InnerOuter, int MPODBoard){
	a;
}
