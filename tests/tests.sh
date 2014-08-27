#! /usr/bin/sh

echo "Running tests with hazard application from in $HAZAPPS";

# Change the temporary file location for the tests.
#
# This way we can hold the temp files and compare them as well
#export TMPHOLDER=$TMPDIR
#export TMPDIR=`pwd`

echo $TMPDIR

# Start running the tests.
# Change this to a for loop (foreach .sas FILE, run and compare.)
echo "ac.death.AVC.sas..."
sas ac.death.AVC.sas
 
echo "hz.death.AVC.sas..."
sas hz.death.AVC.sas
 
echo "hp.death.AVC.hm1.sas..."
sas hp.death.AVC.hm1.sas
 
echo "hp.death.AVC.hm2.sas..."
sas hp.death.AVC.hm2.sas
 
echo "hp.death.AVC.sas..."
sas hp.death.AVC.sas
 
echo "hs.death.AVC.hm1.sas..."
sas hs.death.AVC.hm1.sas
 
echo "hm.death.AVC.sas..."
sas hm.death.AVC.sas
 

echo "hz.deadp.KUL.sas..."
sas hz.deadp.KUL.sas
 

echo "hz.te123.OMC.sas..."
sas hz.te123.OMC.sas
 
echo "hz.tm123.OMC.sas..."
sas hz.tm123.OMC.sas
 

echo "hp.death.COMPARISON.sas..."
sas hp.death.COMPARISON.sas
 
echo "hp.dthip.PAIVS.time.sas..."
sas hp.dthip.PAIVS.time.sas
 

echo "hs.dthar.TGA.setup.sas..."
sas hs.dthar.TGA.setup.sas
 

echo "hs.dthar.TGA.hs1.sas..."
sas hs.dthar.TGA.hs1.sas
 
echo "hm.deadp.VALVES.sas..."
sas hm.deadp.VALVES.sas
 


echo "hm.death.patient.sas..."
sas hm.death.patient.sas
 


echo "Done"


#export TMPDIR=$TMPHOLDER

