#!/bin/bash
#Run mini-ndn experiments
#Automate minindn experiment

root_path="/home/mini-ndn/sdulal"
exp_root="$root_path/automate_experiments"
path_to_conf_file="$exp_root/3d.conf5loss"

graphing_script_home="/home/mini-ndn/"
pings="300"
workd_dir="/mnt/ramdisk/"
minindn_flags=""
exp_name="pingall"
declare -a experiments=("hr_vs_ls_br_0_"$exp_name "hr_vs_ls_asf_2_"$exp_name "hr_vs_ls_asf_4_"$exp_name "hr_vs_ls_asf_0_"$exp_name)

#declare -a experiments=("hr_vs_ls_br_0_pingall")

#This will remove the experiment folder, stop and clean mini-ndn, and stop nfd if it's running not stopped
clean(){
   rm -rf /mnt/ramdisk/*
   mn -c
   nfd-stop
}
#create symlink folder which will be used by the graphing scripts
create_sym_link(){

    rm -rf hrls/* ?
    ln -s  $exp_root/$1/ls/link-state hrls/ls
    ln -s $exp_root/$1/hr hrls/hr
}

#create stretch graphs
run_graphing_scrips(){

    $graphing_script_home/process/convergedata-bytime-new-ping.sh $exp_root/hrls
    $graphing_script_home/process/4-get-failure-stretch.sh $exp_root/hrls
    $graphing_script_home/plot/plot-failure-stretch.sh $exp_root/hrls
}


run_mini_ndn(){

  resultDir=$1
  nfaces=$2

  #pingall experiment best-route link-state for a given number of face
  clean
  minindn  --faces $nfaces --nPings=$pings --experiment=pingall --no-cli $path_to_conf_file --work-dir=$workd_dir --result-dir=$resultDir/ls

  #experiment with hyperbolic routing, asf strategy and for a given number of face
  clean
  minindn --ctime 90 --faces $nfaces --nPings=$pings --routing hr --strategy asf --experiment=pingall --no-cli $path_to_conf_file --work-dir=$workd_dir --result-dir=$resultDir/
  clean
  sleep 30

  create_sym_link $resultDir
  run_graphing_scrips $resultDir
  mkdir -p $exp_root/results/$resultDir && cp $exp_root/hrls/* $_

}

for exp in "${experiments[@]}"; do

 nface=$(echo | awk -v r=$exp '{split(r,a,"_"); print a[5]}') #parse experiment list 
 _exp=$(echo | awk -v r=$exp '{split(r,a,"_"); print a[6]}')
 echo "nface:" $nface " Desc: " $exp " Exp type: " $_exp
 #run_mini_ndn $exp $f $_exp

done
