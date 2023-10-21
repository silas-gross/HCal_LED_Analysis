source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
for i in LED_run_data_*.root ; do 
	root -q ../BuildTimeSeries.C\(\"$i\"\) 
done
