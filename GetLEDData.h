#ifndef __GETLEDDATA_H__
#define __GETLEDDATA_H__

#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/SubsysReco.h>
//#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <vector>
#include <map>
#include <string>
#include <TH1.h>
#include <math.h>
#include <algorithm>
#include <utility>

class LEDRunData: public SubsysReco
{
	private:
	//tower data struture with definition of tower, taken from the HCal base class
		int getPedestal(std::vector<int> chl_data); 
		std::vector <float> getPeak(std::vector<int> chl_data, int pedestal);
		float FindWaveForm(std::vector <int> *chl_data, int pos); 
		float Heuristic(std::vector<int> data, std::vector<int>wf, int ndf);
	public:
	//available global variable
		struct towerinfo {
			bool inner_outer; //false for inner, true for outer
			bool north_south; //false for North, true for south
			int sector; 	//Sector 0-31
			int channel;	//Channels 0-23
			int packet; 	//packet is shared between 4 sectors
			int etabin;	//pseudorapidity bin
			int phibin;	//phi bin
			float eta;	//psedorapidity value
			float phi;	//phi value
			std::string label;	//label for tower to quick parse
			};
		
		 int run_number=1; 
		 std::string runfiles="run_21951.txt";
		 int date=19072023; 
		 std::vector<float> data_points;
		 std::map < std::pair< int, int > , std::map<std::string, float> > tower_datapts;  
		 std::map < std::pair< bool, int> , std::vector<float> > sector_datapts;
		 std::map < std::pair< int, int > , towerinfo > towermaps; //look up table for towers
		 std::map < std::pair< int, int > , std::vector< TH1F* > > datahists;
		 std::vector < int > packets; 
	// methods to run
		LEDRunData(std::string filename){ 
			runfiles=filename;
			for(int i=0; i<16; i++){ packets.push_back(i+7001+int(i/8)*1000);}
		};
		LEDRunData(std::map<std::pair<int, int>, towerinfo> towermap, std::string filename){
			towermaps=towermap; 
			runfiles=filename;
			for(int i=0; i<16; i++) packets.push_back(i+7001+int(i/8)*1000);
		};
		~LEDRunData();	
		int process_event (Event *e); 
	        void CalculateChannelData(towerinfo tower);
		void CalculateSectorData(std::vector<towerinfo> sector);
		void CalculateMPODData(int InnerOuter, int MPODBoard);
		void FileOutput();
		void ReadInput();
	//get passed mapping such that the class 
};
#endif 
