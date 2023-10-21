#include "GetLEDData.h"
#include <TFile.h>
#include <TString.h>
R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libfun4allraw.so);
int n_evt=0;
float LEDRunData::Heuristic(std::vector<int> data, std::vector<int> wf, int npr)
{
	//use chi_squared/ndf as a fitting heuristic
	float chi=0;
	int ndf=data.size()-npr;
	for(int i=0; i<data.size(); i++) chi+=pow(data[i]-wf[i],2)/data[i];
	chi=chi/ndf; 
	return chi;
}
float LEDRunData::FindWaveForm(std::vector <int> *chl_data, int pos)
{
//	std::cout<<"Starting A* search for waveform" <<std::endl;
	int n_params=1, width=1; 
	if (chl_data->size() <= pos) return 0; 
//	std::cout<<"The channel has size of " <<chl_data->size() <<" we look at entry " <<pos <<std::endl;
	std::vector<int> model (chl_data->size(), chl_data->at(pos)), data=*chl_data; //model of just peak value
	std::vector<int>param {pos};
//	std::cout<<"Data has size of " <<data.size() <<std::endl;
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
		float temp_heur=Heuristic(data, temp_model, params.size());
		child_models[temp_heur]=std::make_pair(params, temp_model);
	}
//	std::cout<<"There are " <<child_models.size() <<" child models" <<std::endl;	
	while(child_models.size() > 0)
	{
		auto md=child_models.begin();
		float parent_heur=md->first; 
		auto vals=md->second;
//		std::cout<<"Looking at model with " <<vals.first.size() <<" many parameters" <<std::endl;
		child_models.erase(md);
		std::sort(vals.first.begin(), vals.first.end());
		if(vals.first.size() > vals.second.size()/3)
		{
			child_models.clear();
			break;
		}
	//	std::cout<<"There are " <<child_models.size() <<" models remaining to look at" <<std::endl;
		#pragma omp parallel for private(i,j,k,temp_model)
		for(int i=0; i<vals.first.size(); i++)
			{
			if(i==0 && vals.first.at(i) != 0)
			{
				for(int j=0; j<vals.first.at(i); j++)
				{
					bool pres=false;
					for(int k:vals.first)
					{
						 if(j==k)
							{
								pres=true; 
								break;
							}
					}
					if(pres) continue;
						
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
					//std::cout<<"There is a new model with heuristic " <<th <<std::endl; 
					if(th < parent_heur && th>1) child_models[th]=std::make_pair(temp_params, temp_model); 
					else continue;	
				}
			}
			else if(i==vals.first.size() && vals.first.at(i) != vals.second.size()-1)
			{
				//std::cout<<"in here" <<std::endl;
				for(int j=vals.first.at(i)+1; j<vals.second.size(); j++)
				{
					bool pres=false;
					for(int k:vals.first)
					{
						 if(j==k)
							{
								pres=true; 
								break;
							}
					}
					if(pres) continue;
					float slope=(data[vals.first.at(i)]-data[j])/(vals.first.at(i)-j);
					float ints=data[j]-slope*vals.first.at(i); 
					std::vector<int>temp_model=vals.second; 
					for(int k=vals.first.at(i); k<vals.second.size(); k++){
					       	float mp=slope*k+ints;
						if(temp_model.size() < k) continue; 
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
				//std::cout<<"in a few other things" <<std::endl;
				int end_pos;
				if( vals.first.size() <= i+1) end_pos =vals.second.size(); 
				else end_pos=vals.first.at(i+1); 
				for(int j=vals.first.at(i)+1; j<end_pos; j++)
				{
					bool pres=false;
					for(int k:vals.first)
					{
						if(j==k) pres=true;
					}
					if(pres==true) continue; 
					//std::cout<<"Trying inserting into postion " <<j<<std::endl;
					float slope1=(data[vals.first.at(i)]-data[j])/(vals.first.at(i)-j);
					float ints1=data[j]-slope1*vals.first.at(i); 
					float slope2=(data[end_pos]-data[j])/(end_pos-j);
					float ints2=data[j]-slope2*end_pos; 
					std::vector<int>temp_model=vals.second; 
					for(int k=vals.first.at(i); k<j; k++){
					       	float mp=slope1*k+ints1;
						temp_model.at(k)=(int)mp;
					}
					//std::cout<<"hey, whats going on?"<<std::endl;
					for(int k=j; k<vals.first.at(i); k++){
					       	float mp=slope2*k+ints2;
						temp_model.at(k)=(int)mp;
					}
					std::vector<int> temp_params=vals.first;
					temp_params.push_back(j);
				       	float th=Heuristic(data, temp_model, temp_params.size()); 
					if(th < parent_heur && th > 1) child_models[th]=std::make_pair(temp_params, temp_model); 
					else continue;	
				}
				
			}
		
			else continue;
		}
		if(child_models.size()==0)
		{
		       	model=vals.second;
			param=vals.first;
			heuristic=parent_heur;
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
int LEDRunData::getPedestal(std::vector<int> chl_data) //just gets the pedestal value from first 3 samples
{
	int pv, d1=chl_data[0], d2=chl_data[1], d3=chl_data[2];
	pv=(d1+d2+d3)/3;
	return pv; 
	//can break this out to do more clever in the future
}
std::vector<float> LEDRunData::getPeak(std::vector<int> chl_data, int pedestal) //gets peak value, peak position, peak width, pedestal rms
{
//	std::cout<<"Im getting the peak right now" <<std::endl;
	std::vector<float> peak_data;
	float full_val=0, pos=0, width=0, rms=0, pk=0; 
	for(int sp=0; sp<chl_data.size(); sp++){
		full_val+=(chl_data[sp]-pedestal);
		if(chl_data[sp]>pk){
			pk=chl_data[sp];
			pos=sp;
		}
	}
//	std::cout<<"Found peak at sample number " <<pos <<" with height " <<pk <<std::endl;
	pk+=-pedestal;
	peak_data.push_back(pk);
	peak_data.push_back(pos);
	//now need to do waveform fitting, just going to do a very quick a* search
//	std::cout<<"Peak data has size " <<peak_data.size() <<std::endl;
	if(use_template){
		//put in the template fit from the fun4all fitting function
		//need to use a better template for led data instead of test beam
	//just copy thte function over and use it that way, do the interperloation in a basic way
		TF1* template_function=new TF1*("tf", "[0]*pow(x-[1], [2])*exp([2])*exp(-[2]*(x-[1])/[3])*((1-[4])/pow([5],[2])*exp(-[2]*(x-[1])/[3])+[4]/pow([3],[2])+[pedestal]", 0, chl_data.size());
		TH1F* ch_hist=new TH1F*("ch_hist", "temp", chl_data.size(), -0.5, chl_data.size()+0.5);
		for(auto sp:chl_data) ch_hist->Fill(sp);
		template_function->FixParameter("pedestal", pedestal);
		ch_hist->Fit(template_function);
		ch_hist->Add(template_function, -1);
		int le=0, ge=0;
		for(int sp=0; sp<chl_data.size(); sp++)
		{
			if(chl_data.at(sp)>= chl_data.at(pos)*0.475+0.515*pedestal && chl_data.at(sp)<=chl_data.at(pos)*0.515+0.475*pedestal && ge==0) le=sp; 
			if(chl_data.at(sp)>=chl_data.at(pos)*0.95) ge=sp; 
			if(chl_data.at(sp)>= chl_data.at(pos)*0.475+0.515*pedestal && chl_data.at(sp)<=chl_data.at(pos)*0.515+0.475*pedestal && ge!=0) ge=sp; 
			
		}
		width=ge-le;
		float rms=0;
		for(int i=0; i<ch_hist->GetNbinsX(); i++) rms+=pow(ch_hist->GetBinContent(i), 2);
		float erms=sqrt(1/(chl_data.size())*rms);
		peak_data.push_back(erms);
		delete ch_hist;
		delete template_fuction;
		chl_data.clear();
		 
	}
	else{ if(_fullform && n_evt<1000 ) width=FindWaveForm(&chl_data, (int)pos);
	else{
		int le=0, ge=0;
		for(int sp=0; sp<chl_data.size(); sp++)
		{
			if(chl_data.at(sp)>= chl_data.at(pos)*0.475+0.515*pedestal && chl_data.at(sp)<=chl_data.at(pos)*0.515+0.475*pedestal && ge==0) le=sp; 
			if(chl_data.at(sp)>=chl_data.at(pos)*0.95) ge=sp; 
			if(chl_data.at(sp)>= chl_data.at(pos)*0.475+0.515*pedestal && chl_data.at(sp)<=chl_data.at(pos)*0.515+0.475*pedestal && ge!=0) ge=sp; 
			
		}
		width=ge-le;
	}		
//	std::cout<<"Waveform has been found" <<std::endl;
	peak_data.push_back(width);
	if(!_fullform) for(int sp=0; sp<5; sp++) rms+=pow(chl_data[sp]-pedestal, 2);		
	else for(int sp=0; sp<chl_data.size(); sp++) rms+=pow(chl_data[sp]-pedestal, 2);
	float erms=sqrt(1/(chl_data.size())*rms);
	peak_data.push_back(erms);
	chl_data.clear();
	}
	return peak_data;
}	
int LEDRunData::process_event(PHCompositeNode *topNode){
//	std::vector<Event *> subeventeventvec; 
	try{ 
	n_evt++;
	const std::string &inputnodename="PRDF";
	Event* e = findNode::getClass<Event>(topNode, inputnodename);
//	std::cout<<"Hit a new event"<<std::endl;
	for(auto pid:packets){
		//std::cout<<pid<<std::endl;
		float evtval=0; 
		try{
			e->getPacket(pid);
		}
		catch(std::exception* e) {
			std::cout<<"no packet with number " <<pid <<std::endl; 
			continue;
		}
		Packet* p=e->getPacket(pid);
		if(!p) continue;
	       #pragma opm for private(c, channel_data)
		for(int c=0; c<p->iValue(0, "CHANNELS"); c++){
		 std::vector<int> channel_data;	
		 for(auto s=0; s<31; s++){
	 		evtval+=p->iValue(s, c);
			channel_data.push_back(p->iValue(s,c)); 	
		}
//		std::cout<<"have loaded in the data"<<std::endl;
		if (channel_data.size()<3) continue;
		int pedestal=getPedestal(channel_data);
//		std::cout<<"have the pedestal" <<std::endl;
		std::vector<float>pk_data=getPeak(channel_data, pedestal);
//		std::cout<<"peak data found" <<std::endl;
		std::pair<int, int> location (pid,c);
//		std::cout<<"have the location pair, the datahists have size" << datahists.size() <<std::endl;
		datahists[location][0]->Fill(evtval); 
		datahists[location][1]->Fill(pk_data[0]);
		datahists[location][2]->Fill(pedestal);
		datahists[location][3]->Fill(pk_data[3]);
		datahists[location][4]->Fill(pk_data[1]);
		datahists[location][5]->Fill(pk_data[2]);
		channel_data.clear();
	       }
	}	       
	return 1;
	}
	catch(std::exception* e1) {return 1;}
}
void LEDRunData::ReadInput(){
	
	for(auto pid:packets)
		for(int c=0; c<192; c++){
			std::pair<int, int> id (pid,c);
			int sector=0, ioi=pid/1000, tower=c%48; 
			std::string InnerOuter;
			if(ioi%2) InnerOuter="Outer"; 
			else InnerOuter="Inner";
			sector=pid%8+c/48;
			TH1F* hnew=new TH1F(Form("hcal_packet_%d_channel_%d",pid, c), Form("Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower), 1000, 0, 5000); 
			//Right now this is hard coded, but I should really use the tower map structure
			//Implement as a lookup table
			TH1F* hnew1=new TH1F(Form("hcal_packet_%d_channel_%d_peak",pid, c), Form("Peak Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower), 1000, 0, 5000); 
			TH1F* hnew2=new TH1F(Form("hcal_packet_%d_channel_%d_pedestal",pid, c), Form("Pedestal Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower), 1000, 0, 5000); 
			TH1F* hnew3=new TH1F(Form("hcal_packet_%d_channel_%d_rms",pid, c), Form("Pedestal RMS Value for %s HCal, sector %d, tower %d; Energy [ADC Counts]; n Events", InnerOuter, sector, tower), 1000, 0, 100); 
			TH1F* hnew4=new TH1F(Form("hcal_packet_%d_channel_%d_peak_location",pid, c), Form("Peak Location for %s HCal, sector %d, tower %d; Time Sample; n Events", InnerOuter, sector, tower), 31, 0, 31); 
			TH1F* hnew5=new TH1F(Form("hcal_packet_%d_channel_%d_peak_width",pid, c), Form("Peak width for %s HCal, sector %d, tower %d; Time Sample; n Events", InnerOuter, sector, tower), 31, 0, 31); 
			datahists[id].push_back(hnew);
			datahists[id].push_back(hnew1); 
			datahists[id].push_back(hnew2); 
			datahists[id].push_back(hnew3);	
			datahists[id].push_back(hnew4);
			datahists[id].push_back(hnew5);

		}
}

void LEDRunData::FileOutput(){
//	TFile* f =new TFile("dummy.root", "RECREATE");
	TFile* f;
	if(_fullform) {
		f=new TFile(Form("run_data_full/LED_run_data_%d.root", run_number), "RECREATE");
	}
	else{
		f=new TFile(Form("run_data_fast/LED_run_data_%d.root", run_number), "RECREATE");
	}	
	//f->cd();
	std::cout<<"File created" <<std::endl;
	for(auto a:datahists) for(auto h:a.second) h->Write();
	TNtuple* head=new TNtuple("header", "header", "run:date");
	head->Fill(run_number, date);
	head->Write();
	std::cout<<"wrote data to file" <<std::endl;
	f->Close();
}

void LEDRunData::CalculateChannelData(towerinfo tower){
	int packet=tower.packet, channel=tower.channel, sector=tower.sector;
	int sc=sector%4; 
	channel=sc*16+channel%16+64*channel/16;
	auto d=datahists[std::make_pair(packet, channel)];
	std::map<std::string, float> twr_mean;
	if(d.size() >=6){
//	std::cout<<"There are " <<d.size()<<" many histograms in the channel data"<<std::endl;
	try{
		twr_mean["Value"]=d[0]->GetMean();
		twr_mean["Peak"]=d[1]->GetMean();
		twr_mean["Pedestal"]=d[2]->GetMean();
		twr_mean["Pedestal RMS"]=d[2]->GetRMS();
		twr_mean["Peak Location"]=d[4]->GetMean();
		twr_mean["Peak Width"]=d[5]->GetMean();
//		if(d[1]->GetMean() != 0) std::cout<<"Got the data, inserting into the tower at " <<packet<<" , " <<channel <<std::endl;
		tower_datapts[std::make_pair(packet, channel)]=twr_mean;
	//	for(auto h:d) h->Delete();
	}
	catch(std::exception* e){}
	}
		
}
void LEDRunData::CalculateSectorData(std::vector<towerinfo> sector){
	std::vector<float> sector_vals (5,0); 
	int sector_numb=0;
	bool sector_io=false;
	for(auto tower:sector) 
	{
		sector_numb=tower.sector;
		sector_io=tower.inner_outer;
		int packet=tower.packet, channel=tower.channel;
		auto d=datahists[std::make_pair(packet, channel)];
		sector_vals.at(0)+=d[0]->GetMean();
		sector_vals.at(1)+=d[1]->GetMean();
		sector_vals.at(2)+=d[2]->GetMean();
		sector_vals.at(3)+=d[3]->GetMean();
		sector_vals.at(4)+=d[4]->GetMean();
	}
	for(int i=0; i<sector_vals.size(); i++) sector_vals.at(i)=sector_vals.at(i)/sector.size();
	sector_datapts[std::make_pair(sector_io, sector_numb)]=sector_vals;
}
void LEDRunData::CalculateMPODData(int InnerOuter, int MPODBoard){
	int a;
} //Not yet on this, will work on after first data

