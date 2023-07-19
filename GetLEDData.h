#ifndef __GETLEDDATA_H__
#define __GETLEDDATA_H__

#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <vector>
#include <map>

private:
	std::map<std::pair<int, int>, std::vector<int>> tower_mapping; 
	std::map<std::pair<int, int>, std::vector<int>> generate_tower_mapping(); 
public:
//available global variable
	int run_number=1; 
	float date=19072023; 
	std::vector<float> data_points;
// methods to run	
	int process_event (Event *e); 
	std::vector<float> CalculateChannelData(int InnerOuter, int sector, int tower);
	std::vector<float> CalculateSectorData(int InnerOuter, int sector);
	std::vector<float> CalculateMPODData(int InnerOuter, int MPODBoard);
	void fileoutput();


#endif 
