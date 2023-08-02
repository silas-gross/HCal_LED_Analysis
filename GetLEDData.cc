#include "GetLEDData.h"
#include <TFile.h>
#include <TString.h>
LEDRunDtaa::Heuristic(std::vector<int> data, std::vector<int> wf, int npr)
{
	//use chi_squared/ndf as a fitting heuristic
	float chi=0;
	int ndf=data.size()-npr;
	for(int i=0; i<data.size(); i++) chi+=pow(data[i]-wf[i],2)/data[i];
	chi=chi/ndf; 
	return chi;
}
LEDRunData::FindWaveForm(std::vector*<int> chl_data, int pos)
{
	int n_params=1, width=1; 
	std::vector<int> model (chl_data->size(), chl_data->at(pos)), data=*chl_data; //model of just peak value
	std::vector<int>param {pos};
	float heuristic=Heuristic(data, model, n_params);
	std::map<float, std::pair<std::vector<int>, std::vector<int>>> child_models; //this will be used to generate the queue
	for(int i=0; i<data.size(); i++) //generate the first set of children, linear models
	{
		if(i==pos) continue;
		std::vector<int> params {pos, i};
		float slope=(data[pos]-data[i])/(pos-i);
		float ints=data[pos]-slope*i;
		std::vector<int>temp_model; 
		for(int j=0; j<data.size(); j++){
		       float mp=slope*j+ints;
		       temp_model.push_back((int)mp);
		}
		float temp_heur=Heutristic(data, temp_model, params.size());
		child_models[temp_heur]=std::make_pair(params, temp_model);
	}	
	while(child_models.size() > 0)
	{
		auto md=child_models.begin();
		float parent_heur=md->first; 
		auto vals=md->second;
		child_models->erase(md);
		std::sort(vals.first.begin(), vals.first.end());
		for(int i=0; i<vals.first.size(); i++)
		{
			if(i==0 && vals.first.at(i) != 0)
			{
				for(int j=0; j<vals.first.at(i); j++)
				{
					float slope=(data[vals.first.at(i)]-data[j])/(vals.first.at(i)-j);
					float ints=data[j]-slope*vals.first.at(i); 
					std::vector<int>temp_model=vals.second; 
					for(int k=0; k<vals.first.at(i); k++){
					       	float mp=slope*k+ints;
						temp_model.at(k)=(int)(mp);
					}
					std::vector<int> temp_params=vals.first;
					temp_params.push_back(j);
				       	float th=Heuristic(data, temp_model, temp_params.size()); 
					if(th < parent_heur && th>1) child_models[th]=std::make_pair(temp_params, temp_model); 
					else continue;	
				}
			}
			else if(i==vals.first.size() && vals.first.at(i) != vals.second.size()-1)
			{
				for(int j=<vals.first.at(i)+1; j<vals.second.size(); j++)
				{
					float slope=(data[vals.first.at(i)]-data[j])/(vals.first.at(i)-j);
					float ints=data[j]-slope*vals.first.at(i); 
					std::vector<int>temp_model=vals.second; 
					for(int k=vals.first.at(i); k<vals.second.size(); k++){
					       	float mp=slope*k+ints;
						temp_model.at(k)=(int)(mp);
					}
					std::vector<int> temp_params=vals.first;
					temp_params.push_back(j);
				       	float th=Heuristic(data, temp_model, temp_params.size()); 
					if(th < parent_heur && th>1) child_models[th]=std::make_pair(temp_params, temp_model); 
					else continue;	
				}
				
			}
			else if( vals.first.at(i) !=0 && vals.first.at(i) != vals.second.size()-1) 
			{
				for(int j=vals.first.at(i)+1; j<vals.first.at(i+1); j++)
				{
					float slope1=(data[vals.first.at(i)]-data[j])/(vals.first.at(i)-j);
					float ints1=data[j]-slope*vals.first.at(i); 
					float slope2=(data[vals.first.at(i+1)]-data[j])/(vals.first.at(i+1)-j);
					float ints2=data[j]-slope*vals.first.at(i+1); 
					std::vector<int>temp_model=vals.second; 
					for(int k=vals.first.at(i); k<j; k++){
					       	float mp=slope1*k+ints1;
						temp_model.at(k)=(int)(mp);
					}
					for(int k=j; k<vals.first.at(i); k++){
					       	float mp=slope2*k+ints2;
						temp_model.at(k)=(int)(mp);
					}
					std::vector<int> temp_params=vals.first;
					temp_params.push_back(j);
				       	float th=Heuristic(data, temp_model, temp_params.size()); 
					if(th < parent_heur && th>1) child_models[th]=std::make_pair(temp_params, temp_model); 
					else continue;	
				}
				
			}
		
			else continue;
		}
		if(child_models.size()==0)
		{
		       	model=vals.second;
			param=vals.first;
			heuristic=patent_heur;
		}

	}
	int le=0, ge=0;
	for(int sp=0; sp<chl_data->size(); sp++)
	{
	       	chl_data->at(sp)-=model.at(sp);
		if(model.at(sp)>= data.at(pos)*0.475 && model.at(sp)<=data.at(pos)*0.515 && ge==0) le=sp; 
	       	if(model.at(sp)>=data.at(pos)*0.6) ge=sp; 
		if(model.at(sp)>= data.at(pos)*0.475 && model.at(sp)<=data.at(pos)*0.515 && ge!=0) ge=sp; 
		
	}
	width=ge-le;
	return width;
	
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
	for(int sp=0; sp<chl_data.size(); sp++){
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
	rms=sqrt(1/(chl_data.size())*rms);
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
	for(auto e:filenames) process_event(&e); //not quite sure if I'm doing this right, should figure that out 
}
LEDRunData::FileOutput(){
	TFile* f=new TFile(Form("LED_run_data_%s.root", run_number).c_str(), "REMAKE");
	for(auto a:datahists) for(auto h:a.second) h->Write();
	f->Close();
}
LEDRunData::CalculateChannelData(towerinfo tower){
	int packet=tower.packet, channel=tower.channel;
	auto d=datahists[std::makepair(packet, channel)];
	std::map<std::string, float> twr_mean;
	twr_mean["Value"]=d[0]->GetMean();
	twr_mean["Peak"]=d[1]->GetMean();
	twr_mean["Pedestal"]=d[2]->GetMean();
	twr_mean["Pedestal RMS"]=d[3]->GetMean();
	twr_mean["Peak Location"]=d[4]->GetMean();
	twr_mean["Peak Width"]=d[5]->GetMean();
	tower_datapts[std::makepair(packet, channel)]=twr_mean;
		
}
LEDRunData::CalculateSectorData(std::vector<towerinfo> sector){
	std::vector<float> sector_vals (5,0); 
	int sector_numb=0;
	bool sector_io=false;
	for(auto tower:sector) 
	{
		sector_numb=tower.sector;
		sector_io=tower.inner_outer;
		int packet=tower.packet, channel=tower.channel;
		auto d=datahists[std::makepair(packet, channel)];
		sector_vals.at(0)+=d[0]->GetMean();
		sector_vals.at(1)+=d[1]->GetMean();
		sector_vals.at(2)+=d[2]->GetMean();
		sector_vals.at(3)+=d[3]->GetMean();
		sector_vals.at(4)+=d[4]->GetMean();
	}
	for(int i=0; i<sector_vals.size(); i++) sector_vals.at(i)=sector_vals.at(i)/sector.size();
	sector_datapts[std::makepair(sector_io, sector_numb)]=sector_vals;
}
LEDRunData::CalculateMPODData(int InnerOuter, int MPODBoard){
	int packet=tower.packet, channel=tower.channel;
	a;
} //Not yet on this, will work on after first data
