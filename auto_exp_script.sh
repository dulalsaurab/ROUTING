#!/bin/bash
# Run mini-ndn experiments

root_path="/home/mini-ndn/sdulal"
path_to_conf_file="$root_path/mini-ndn/current-testbed1.conf"
graph_script="/home/mini-ndn/graphing-scripts"

clean(){

   rm -rf /mnt/ramdisk/*
   mn -c

}

create_sym_link(){

    rm -rf hrls/* ?
    ln -s  $root_path/automate_experiments/$1/ls/link-state hrls/ls
    ln -s $root_path/automate_experiments/$1/hr hrls/hr

}
run_graphing_scrips(){


    echo "$graph_script/process/convergedata-bytime-new-ping.sh $root_path/automate_experiments/$1"
    $graph_script/process/convergedata-bytime-new-ping.sh $root_path/automate_experiments/hrls
    $graph_script/process/4-get-failure-stretch.sh $root_path/automate_experiments/hrls
    $graph_script/plot/plot-failure-stretch.sh $root_path/automate_experiments/hrls
    #sudo ./4-get-failure-stretch.sh ~/sdulal/mini-ndn/saurab/hr/asf/4
    #3sudo ./plot-failure-stretch.sh ~/sdulal/mini-ndn/saurab/hr/asf/4

}

resultDir="hr_vs_ls_br"

#clean
#minindn --faces 0 --experiment=pingall --nPings=5 --no-cli $path_to_conf_file --work-dir=/mnt/ramdisk/ --result-dir=$resultDir/ls
#clean
#minindn --faces 0 --routing hr --experiment=pingall --nPings=5 --no-cli $path_to_conf_file --work-dir=/mnt/ramdisk/ --result-dir=$resultDir/
#clean

create_sym_link $resultDir

run_graphing_scrips $resultDir

# mkdir -p $root_path/automate_experiments/results/0/ && cp $root_path/automate_experiments/hrls/*


#clean
#minindn --faces 2 --experiment=pingall --ctime=30 --nPings=5 --no-cli $path_to_conf_file --work-dir=/mnt/ramdisk/ --result-dir=saurab/ls/br/2
#clean
#minindn --faces 2 --routing hr --experiment=asf-exp --ctime=30 --nPings=5 --no-cli $path_to_conf_file --work-dir=/mnt/ramdisk/ --result-dir=saurab/hr/asf/2
#clean
#create_sym_link "saurab/ls/br/2" "saurab/hr/asf/2/hr"
#mkdir -p $root_path/automate_experiments/results/2/ && cp $root_path/automate_experiments/hrls/*








#sudo minindn --faces 2 --experiment=pingall current-testbed.conf --work-dir=/mnt/ramdisk/ --result-dir=saurab/ls/br/2
#sudo minindn --faces 4 --experiment=pingall current-testbed.conf --work-dir=/mnt/ramdisk/ --result-dir=saurab/ls/br/4

#sudo minindn --faces 0 --routing hr --experiment=pingall current-testbed.conf --work-dir=/mnt/ramdisk/ --result-dir=saurab/hr/br/0 
#sudo minindn --faces 2 --routing hr --experiment=pingall current-testbed.conf --work-dir=/mnt/ramdisk/ --result-dir=saurab/hr/br/2
#sudo minindn --faces 4 --routing hr --experiment=pingall current-testbed.conf --work-dir=/mnt/ramdisk/ --result-dir=saurab/hr/br/4
#asf-exp
