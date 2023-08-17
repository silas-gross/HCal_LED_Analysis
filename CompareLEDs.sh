#! /bin/bash

#Run analysis over a set of runs, taken from csv file
runs=$( more $1 | cut -d "," -f2 )
declare -a runnumbers 
readarray -t runnumbers <<< $runs

for ra in ${runnumbers[@]}; do 
	ls /sphenix/lustre01/sphnxpro/commissioning/HCal/led/led_*"$ra"*  >> run_"$ra".txt
done

	
