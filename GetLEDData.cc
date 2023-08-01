#include "GetLEDData.h"
#include <TFile.h>
#include <TString.h>
LEDRunDtaa::Heuristic(std::vector<int> data, std::vector<int> wf, int ndf)
{
	//use chi_squared/ndf as a fitting heuristic
	float chi=0;
	for(int i=0; i<data.size; i++) chi+=pow(data[i]-wf[i],2)/data[i];
	chi=chi/ndf; 
	return chi;
}
LEDRunData::FindWaveForm(std::vector*<int> chl_data, int pos)
{
	int n_params=1; 
	std::vector<int> model (chl_data->size, chl_data->at(pos)), data=*chl_data; //model of just peak value
	std::vector<float>param {data[pos]};
	float heuristic=Heuristic(data, model, n_params);
	std::map<float, std::pair<std::vector<float>, std::vector<int>>> child_models; //this will be used to generate the queue
	for(int i=0; i<data.size; i++) //generate the first set of children, linear models
	{
		if(i==pos) continue;
		std::vector<float> params {data[pos], data[i]};
		float slope=(data[pos]-data[i])/(pos-i);
		float ints=data[pos]-slope*i;
		std::vector<int>temp_model; 
		for(int j=0; j<data.size; j++){
		       float mp=slope*j+ints;
		       temp_model.push_back((int)mp);
		}
		float temp_heur=Heutristic(data, temp_model, params.size);
		child_models[temp_heur]=std::make_pair(params, temp_model);
	}	
	//For the next set, use function pointer to generate the heuristic
	
}
LEDRunData::getPedestal(std::vector<int> chl_data) //just gets the pedestal value from first 3 samples
{
	int pv, d1=chl_data[0], d2=chl_data[1], d3=chl_data[2];
	pv=(d1+d2+d3)/3;
	return pv; 
	//can break this out to do more clever in the future
}
LEDRunData::getPeak(std::vector<int> chl_data, int pedestal) //gets peak value, peak position, peak width, pedestal rms
{
	std::vector<float> peak_data;
	float full_val=0, pos=0, width=0, rms=0, pk=0; 
	for(int sp=0; sp<chl_data.size; sp++){
		full_val+=(chl_data[sp]-pedestal);
		if(chl_data[sp]>pk){
			pk=chl_data[sp];
			pos=sp+1;
		}
	}
	peak_data.push_back(full_val);
	peak_data.push_back(pos);
	//now need to do waveform fitting, just going to do a very quick a* search
	width=FindWaveForm(&chl_data, (int)pos);
	peak_data.push_back(width);
	for(int sp:chl_data) rms+=pow(sp-pedestal, 2);
	rms=sqrt(1/(chl_data.size)*rms);
	peak_data.push_back(rms);
	return peak_data;
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
		std::vector<float>pk_data=getPeak(channel_data, pedestal);
		std::pair<int, int> location {p,c};
		datahists[location][0]->Fill(evtval); 
		datahists[location][1]->Fill(pk[0]);
		datahists[location][2]->Fill(pedestal);
		datahists[location][3]->Fill(pk[3]);
		datahists[location][4]->Fill(pk[1]);
		datahists[location][5]->Fill(pk[2]);
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
