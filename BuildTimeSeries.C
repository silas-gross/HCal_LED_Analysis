#include <string>
#include <vector>
#include <sstream>

void BuildTimeSeries(std::string filename)
{
	//Take a file and add gaussian parameters for fitting the led 
	//
	TFile* f=new TFile(filename.c_str(), "UPDATE");
	std::stringstream sst (filename);
	std::string run;
	bool isrun=false;
	float run_number; 
	while(std::getline(sst,run, '_')){
		if(isrun){
			run_number=std::stof(run);
			isrun=false;
		}
		if(run.find("data") != std::string::npos) isrun=true;
	}
		 	
	std::vector<TH1F*> led_channels_peak, led_channel_pos;
	TList* lk=f->GetListOfKeys();
	for(auto k:*lk){
		std::string objecttitle=k->GetTitle();
		if(objecttitle.find("Peak Value ") != std::string::npos ) led_channels_peak.push_back((TH1F*)k);
		if(objecttitle.find("Position") != std::string::npos) led_channel_pos.push_back((TH1F*)k); 
	}
	TFile* fout=new TFile("Time_Series.root", "UPDATE");
	fout->cd();
	TList* outlist=fout->GetListOfKeys();
	std::cout<<led_channels_peak.size()<<std::endl;
	for(int i=0; i<led_channels_peak.size(); i++){
		TH1F* h = led_channels_peak.at(i);
		std::string tow, tower_height, tower_sigma, tower_name;
		std::istringstream sst (h->GetName());
		bool packet=false, chn=false; 
		while(std::getline(sst,tow,'_'))
		{
	 		if(packet){
				tower_name="packet_"+tow;
				tower_height="height_packet_"+tow;
				tower_sigma="std_dev_packet"+tow;
				packet=false;
			}
			if(chn){
				tower_name+="_channel_"+tow;
				tower_height+="_channel_"+tow;
				tower_sigma+="_channel_"+tow;
				chn=false;
			}
			if(tow.find("packet") != std::string::npos) packet=true;
			if(tow.find("channel") != std::string::npos) chn=true;
		}
		TFitResultPtr gf=h->Fit("gaus", "S");
		Double_t chi2 = gf->Chi2();
		Double_t sigma = gf->Parameter(2);
		Double_t height = gf->Parameter(0);
		Double_t mean = gf->Parameter(1); 
		if(outlist->FindObject(h->GetName())){
			TGraph* g1=(TGraph*)outlist->FindObject(h->GetName());
			g1->AddPoint(run_number, mean);
			TGraph* g2=(TGraph*)outlist->FindObject(tower_height.c_str());
			TGraph* g3=(TGraph*)outlist->FindObject(tower_sigma.c_str());
			g2->AddPoint(run_number, height);
			g3->AddPoint(run_number, sigma);
			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();
		}
	        else{
			TGraph* g1=new TGraph();
			g1->SetNameTitle(h->GetName(), Form("Mean peak height in %s", tower_name.c_str()));
			g1->AddPoint(run_number, mean);
	
			TGraph* g2=new TGraph();
			g2->SetNameTitle(tower_height.c_str(), Form("Height of peak distro in %s", tower_name.c_str()));
			g2->AddPoint(run_number, height);
			TGraph* g3=new TGraph();
			g3->SetNameTitle(tower_sigma.c_str(), Form("Standar Deviation in peak of %s", tower_name.c_str()));
			g3->AddPoint(run_number, sigma);

			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();
		}
	}
	for(int i=0; i<led_channel_pos.size(); i++){
		TH1F* h=led_channel_pos.at(i);
		std::string tow, tower_height, tower_sigma, tower_name;
		std::istringstream sst (h->GetName());
		bool packet=false, chn=false; 
		while(std::getline(sst,tow,'_'))
		{
	 		if(packet){
				tower_name="packet_"+tow;
				tower_height="pos_height_packet_"+tow;
				tower_sigma="pos_std_dev_packet"+tow;
				packet=false;
			}
			if(chn){
				tower_name+="_channel_"+tow;
				tower_height+="_channel_"+tow;
				tower_sigma+="_channel_"+tow;
				chn=false;
			}
			if(tow.find("packet") != std::string::npos) packet=true;
			if(tow.find("channel") != std::string::npos) chn=true;
		}
		TFitResultPtr gf=h->Fit("gaus", "S");
		Double_t chi2 = gf->Chi2();
		Double_t sigma = gf->Parameter(2);
		Double_t height = gf->Parameter(0);
		Double_t mean = gf->Parameter(1); 
		if(outlist->FindObject(h->GetName())){
			TGraph* g1=(TGraph*)outlist->FindObject(h->GetName());
			g1->AddPoint(run_number, mean);
			TGraph* g2=(TGraph*)outlist->FindObject(tower_height.c_str());
			TGraph* g3=(TGraph*)outlist->FindObject(tower_sigma.c_str());
			g2->AddPoint(run_number, height);
			g3->AddPoint(run_number, sigma);
			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();
		}
	        else{
			TGraph* g1=new TGraph();
			g1->SetNameTitle(h->GetName(), Form("Mean position in %s", tower_name.c_str()));
			g1->AddPoint(run_number, mean);
	
			TGraph* g2=new TGraph();
			g2->SetNameTitle(tower_height.c_str(), Form("Height position in %s", tower_name.c_str()));
			g2->AddPoint(run_number, height);
			TGraph* g3=new TGraph();
			g3->SetNameTitle(tower_sigma.c_str(), Form("Standar Deviation in position of %s", tower_name.c_str()));
			g3->AddPoint(run_number, sigma);

			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();
		}
	}
	fout->Write();
	fout->Close();	
}
