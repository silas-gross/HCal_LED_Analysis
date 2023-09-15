#! /bin/bash
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
#while IFS="," read -r date run beam
#do 
#Run analysis over a set of runs, taken from csv file
	./CompareLEDRuns "full" $1 $2 $3
#done <  <(tail -n +2 runs_and_time.csv)
#runs=$( more $1 | cut -d "," -f2 )
#declare -a runnumbers 
#readarray -t runnumbers <<< $runs

#for ra in ${runnumbers[@]}; do 
#	ls /sphenix/lustre01/sphnxpro/commissioning/HCal/led/led_*"$ra"*  >> run_"$ra".txt
#done

	
