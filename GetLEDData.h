#ifndef __GETLEDDATA_H__
#define __GETLEDDATA_H__

#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <vector>
#include <map>
#include <string>

class LEDRunData
{
	private:
	//tower data struture with definition of tower, taken from the HCal base class
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
	public:
	//available global variable
		
		int run_number=1; 
		std::string runfiles="run_21951.txt"
		float date=19072023; 
		std::vector<float> data_points;
		std::map<std::string, std::map<std::string, float>> tower_datapts;  
		std::vector<towerinfo> towermaps;
	// methods to run
		LEDRunData(std::string filename){ runfiles=filename;};
		LEDRunData(std::vector<towerinfo> towermap, std::string filename){
			towermaps=towermap; 
			runfiles=filename;
		};
		~LEDRunData();	
		int process_event (Event *e); 
		std::vector<float> CalculateChannelData(towerinfo tower);
		std::vector<float> CalculateSectorData(std::vecto<towerinfo> sector);
		std::vector<float> CalculateMPODData(int InnerOuter, int MPODBoard);
		void fileoutput();
	//get passed mapping such that the class 
}
#endif 