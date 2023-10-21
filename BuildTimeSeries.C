#include <string>
#include <vector>
#include <sstream>

void BuildTimeSeries(std::string filename)
{
	//Take a file and add gaussian parameters for fitting the led 
	//
	TFile* f=new TFile(filename.c_str(), "READ");
	std::stringstream sst (filename);
	std::string run;
	typedef struct {Double_t mean,sigma, height;} FITDATA;
	TTree* tt;
	bool isrun=false;
	int run_number; 
	while(std::getline(sst,run, '_')){
		if(isrun){
			run_number=std::stof(run);
			isrun=false;
		}
		if(run.find("data") != std::string::npos) isrun=true;
	}
		 	
	std::vector<TH1F*> led_channels_peak, led_channel_pos;
	TList* lk=f->GetListOfKeys();
	int date; 
	for(auto k:*lk){
		std::string objecttitle=k->GetTitle();
		TKey* k1=(TKey*)k;
		std::string st(k1->ReadObj()->ClassName()); 
		if(st.find("TH1") ==std::string::npos){
			auto tn=(TNtuple*)(TKey*)k1->ReadObj();
			date=tn->GetBranch("date")->GetEntry();
			continue;
		}
		else{
			TH1F* h=(TH1F*)(TKey*)k1->ReadObj();
			if(h->GetEntries() == 0 ) continue;
			if(objecttitle.find("Peak Value ") != std::string::npos ) led_channels_peak.push_back(h);
			else if(objecttitle.find("Position") != std::string::npos) led_channel_pos.push_back(h); 
			else continue;
		}
	}
	TFile* fout=new TFile("Time_Series.root", "UPDATE");
	fout->cd();
	TList* outlist=fout->GetListOfKeys();
	std::cout<<led_channels_peak.size()<<std::endl;
	std::vector <FITDATA*> peakfits;
	int xpos=date+(run_number%100);
	if(outlist->FindObject("DATA_TREE")) tt=(TTree*)((TKey*)outlist->FindObject("DATA_TREE"))->ReadObj();
	else tt=new TTree("DATA_TREE", "data tree");
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
		//std::cout<<"packet, channel found: " <<tower_name <<std::endl;
		try{
			double_t m=h->GetMean();
			auto gt=h->Fit("gaus", "S");
			gt->Parameter(2);
		
		}
		catch(std::exception& e){
				continue;}	
		TFitResultPtr gf=h->Fit("gaus", "S");
		//Double_t chi2 = gf->Chi2();
		//std::cout<<"chi squared " <<std::endl;
		Double_t sigma1 = gf->Parameter(2);
		Double_t height1 = gf->Parameter(0);
		Double_t mean1 = gf->Parameter(1); 
		FITDATA peak {mean1, sigma1, height1};
		peakfits.push_back(&peak);
		if(outlist->FindObject("DATA_TREE")){
			tt->SetBranchAddress(tower_name.c_str(), &peakfits.at(-1));
			/*TGraph* g1=(TGraph*)((TKey*)outlist->FindObject(h->GetName()))->ReadObj();
			g1->AddPoint(xpos, mean);
			TGraph* g2=(TGraph*)((TKey*)outlist->FindObject(tower_height.c_str()))->ReadObj();
			TGraph* g3=(TGraph*)((TKey*)outlist->FindObject(tower_sigma.c_str()))->ReadObj();
			g2->AddPoint(xpos, height);
			g3->AddPoint(xpos, sigma);
			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();*/
		}
	        else{
			tt->Branch(tower_name.c_str(), &peakfits.at(-1));
		/*	TGraph* g1=new TGraph();
			g1->SetNameTitle(h->GetName(), Form("Mean peak height in %s", tower_name.c_str()));
			g1->AddPoint(xpos, mean);
	
			TGraph* g2=new TGraph();
			g2->SetNameTitle(tower_height.c_str(), Form("Height of peak distro in %s", tower_name.c_str()));
			g2->AddPoint(xpos, height);
			TGraph* g3=new TGraph();
			g3->SetNameTitle(tower_sigma.c_str(), Form("Standar Deviation in peak of %s", tower_name.c_str()));
			g3->AddPoint(xpos, sigma);

			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();*/
		}
		//std::cout<<"finished running the mean graphs"<<std::endl;
	}
	tt->Fill();
	tt->Write(0,TObject::kWriteDelete,0);
/*	for(int i=0; i<led_channel_pos.size(); i++){
		TH1F* h=led_channel_pos.at(i);
		std::string tow, tower_height, tower_sigma, tower_name;
		std::istringstream sst (h->GetName());
		bool packet=false, chn=false; 
		while(std::getline(sst,tow,'_'))
		{
	 		if(packet){
				tower_name="packet_"+tow;
				tower_height="pos_height_packet_"+tow;
			std_dev_packet8002_channel_191	tower_sigma="pos_std_dev_packet"+tow;
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
		try{ 
			auto gt=h->Fit("gaus", "S");
			gt->Parameter(2);
		 }
		catch(std::exception& e){continue;}	
		TFitResultPtr gf=h->Fit("gaus", "S");
		Double_t chi2 = gf->Chi2();
		Double_t sigma = gf->Parameter(2);
		Double_t height = gf->Parameter(0);
		Double_t mean = gf->Parameter(1); 
		if(outlist->FindObject(h->GetName())){
			TGraph* g1=(TGraph*)outlist->FindObject(h->GetName());
			g1->AddPoint(xpos, mean);
			TGraph* g2=(TGraph*)outlist->FindObject(tower_height.c_str());
			TGraph* g3=(TGraph*)outlist->FindObject(tower_sigma.c_str());
			g2->AddPoint(xpos, height);
			g3->AddPoint(xpos, sigma);
			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();
		}
	        else{
			TGraph* g1=new TGraph();
			g1->SetNameTitle(h->GetName(), Form("Mean position in %s", tower_name.c_str()));
			g1->AddPoint(xpos, mean);
	
			TGraph* g2=new TGraph();
			g2->SetNameTitle(tower_height.c_str(), Form("Height position in %s", tower_name.c_str()));
			g2->AddPoint(xpos, height);
			TGraph* g3=new TGraph();
			g3->SetNameTitle(tower_sigma.c_str(), Form("Standar Deviation in position of %s", tower_name.c_str()));
			g3->AddPoint(xpos, sigma);

			g1->Write();
			g2->Write();
			g3->Write();
			fout->Write();
		}
	}*/
	fout->Write();
	fout->Close();	
}
