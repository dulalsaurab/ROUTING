#!/bin/bash

#Run mini-ndn experiments
#Automate minindn experiment
#pwd-/home/mini-ndn/sdulal/automate_experiments/scripts/graphing-scripts
root_path="/home/mini-ndn/europa_bkp/mini-ndn/sdulal"
exp_root="$root_path/automate_experiments"
scripts_path="$exp_root/scripts"
path_to_conf_file="$exp_root/topologies_bkp/3d.conf"
graphing_script_home="$scripts_path/graphing-scripts"
pings="900"
workd_dir="/mnt/ramdisk/"
minindn_flags=""
exp_name="pingall"
strategy="asf/%FD%03/probing-interval~10000"
#declare -a experiments=("rgh_pop_asf_n42_0_"$exp_name "rgh_pop_asf_n42_2_"$exp_name "rgh_pop_asf_n42_4_"$exp_name)

declare -a experiments=("rgh_vs_ls_br_0_pingall_latestndndump")

#This will remove the experiment folder, stop and clean mini-ndn, and stop nfd if it's running not stopped
clean(){

   rm -rf /mnt/ramdisk/*
   mn -c
}

#create symlink folder which will be used by the graphing scripts
create_sym_link(){

    #$1 = resultDir
    rm -rf hrls/* ?
    ln -s  $exp_root/$1/ls/link-state $exp_root/hrls/ls
    ln -s $exp_root/$1/hr $exp_root/hrls/hr
}

#create stretch graphs
run_graphing_scrips(){

    $graphing_script_home/process/convergedata-bytime-new-ping.sh $exp_root/hrls
    $graphing_script_home/process/4-get-failure-stretch.sh $exp_root/hrls
    $graphing_script_home/plot/plot-failure-stretch.sh $exp_root/hrls
}

run_mini_ndn(){

  resultDir=$1   #e.g. hr_vs_ls_0_pingall
  nfaces=$2
  echo "Result Dir: " $resultDir "and nfaces: " $nface
  #pingall experiment best-route link-state for a given number of face
  clean
  minindn --faces $nfaces --nPings $pings --experiment pingall --no-cli $path_to_conf_file --work-dir=$workd_dir --result-dir $resultDir/ls

  #experiment with hyperbolic routing, asf strategy and for a given number of face
  clean
  minindn --faces $nfaces --nPings $pings --routing hr --strategy $strategy --experiment pingall --no-cli $path_to_conf_file --work-dir $workd_dir --result-dir=$resultDir/
  clean
  sleep 10

  #create_sym_link $resultDir
  #run_graphing_scrips $resultDir
  #mkdir -p $exp_root/results/$resultDir && cp $exp_root/hrls/* $_

}

echo $graphing_script_home

for exp in "${experiments[@]}"; do

 nface=$(echo | awk -v r=$exp '{split(r,a,"_"); print a[5]}') #parse experiment list
 _exp=$(echo | awk -v r=$exp '{split(r,a,"_"); print a[6]}')
 echo "nface:" $nface " Desc: " $exp " Experiment type: " $_exp
 run_mini_ndn $exp $nface $_exp
 create_sym_link $exp
 run_graphing_scrips $exp
 mkdir -p $exp_root/results/$exp && cp $exp_root/hrls/* $_

done
