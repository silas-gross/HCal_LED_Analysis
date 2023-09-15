#! /bin/bash 

while IFS="," read -r date run beam
do 
   fname="condor_run_$run.job"
   touch $fname
	echo "Universe        = vanilla" > $fname
	echo "Executable 	= /gpfs/mnt/gpfs02/sphenix/user/sgross/HCal_LED_Analysis/CompareLEDs_fullprocess.sh" >> $fname
	
	echo "Arguments       = $run $beam $date " >> $fname 
	echo "Output  	= /gpfs/mnt/gpfs02/sphenix/user/sgross/HCal_LED_Analysis/condor.out" >> $fname
	echo "Error 		=/gpfs/mnt/gpfs02/sphenix/user/sgross/HCal_LED_Analysis/condor.err" >> $fname
	echo "Log  		=/gpfs/mnt/gpfs02/sphenix/user/sgross/HCal_LED_Analysis/condor.log" >> $fname
	echo "Initialdir  	= /gpfs/mnt/gpfs02/sphenix/user/sgross/HCal_LED_Analysis" >> $fname
	echo "PeriodicHold 	= (NumJobStarts>=1 && JobStatus == 1)" >> $fname
	echo "accounting_group = group_phenix.u" >> $fname
	echo "accounting_group_user = sgross" >> $fname
	echo "request_memory = 8192MB" >> $fname
	echo "Priority = 90" >> $fname
	echo "job_lease_duration = 3600" >> $fname
	echo "Queue 1" >> $fname

	if [[ $i -eq "submit" ]]; then 
		condor_submit $fname
	fi 
done <  <(tail -n +2 runs_and_time.csv)
	 

