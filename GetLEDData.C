ReadInput(){
	Fun4AllServer *se =Fun4AllServer::instance();
	se->Verbosity(0);
	Fun4AllPrdfInputPoolManager* in= new Fun4AllPrdfInputPoolManager("in");
	in->AddPrdfInputList(filename);
	se->registerInputManager(in);
	
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
	LEDRunData* singlerun= new LEDRunData*(towermap, filename, &datahists);
	se->RegisterSubSystem(singlerun);
	se->run();
}

