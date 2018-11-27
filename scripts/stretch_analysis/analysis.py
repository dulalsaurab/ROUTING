# -*- Mode:python; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
#
# Copyright (C) 2015-2018, The University of Memphis
# Author: @Saurab Dulal

import sys
import argparse
from numpy import sin, cos, sinh, cosh, arccos, arccosh
from collections import defaultdict
import math
import numpy as np
import os
import re

def parseInput(args):
    parser = argparse.ArgumentParser()
    parser._action_groups.pop()
    required = parser.add_argument_group('required arguments')
    required.add_argument("--f", dest="topology_file", required=True,
                        help="file containing the map between HR/RGH nodes with corresponding city data i.e. lati, longi, population")
    required.add_argument("--work_dir", dest="work_dir", required=True, help="ndnping experiment data folder")
    return parser.parse_args()


#read the topology file and find
# city data should be of the form: unique_name population latitude longitude region unique_id

def readTopologyFile(datafile):
    #escape first line - metadata
    # data_dict = defaultdict(list) # as_id : [r, theta]
    #short these nodes, cities on the basis of population, the third column is population
    data_dict = []
    with open(datafile, 'r') as f:
        # next(f)
        rows = (line.split() for line in f)
        for node, radius, theta, population, lati, tmp_longi in rows:
            # lati to radians
            lati = (-float(lati) + 90.0) * np.pi / 180.0

            # calculate longi in radian
            tmp_longi = float(tmp_longi)
            if float(tmp_longi) < 0.0:
                longi = 2 * np.pi - np.abs(tmp_longi * np.pi / 180.0)
            else:
                longi = tmp_longi * np.pi / 180.0
            data_dict.append([node, radius, theta, population, float(lati), float(longi)])
    print("Hyperbolic file read successful")
    #sort the data on the basis of radii
    data_dict.sort(key=lambda k: k[3], reverse=False)
    return data_dict


# function to find hyperbolic distance between two nodes
def getHyperbolicDistance (r1, th1, phi1, r2, th2, phi2):
    zeta = 1.0
    dtheta = arccos(cos(th1)*cos(th2) + sin(th1)*sin(th2)*cos(phi1 - phi2)) # angular distance on S2
    x_ij = (1./zeta) * arccosh(cosh(zeta*r1)*cosh(zeta*r2) - sinh(zeta*r1)*sinh(zeta*r2)*cos(dtheta))
    return x_ij

# function to find geographic distance between two nodes
def getGeographicalDistance (th1, phi1, th2, phi2):
    R_earth = 6371.0
    dtheta = arccos(cos(th1)*cos(th2) + sin(th1)*sin(th2)*cos(phi1 - phi2))
    x_ij = R_earth * dtheta
    return x_ij

def getUnderlayDelay (th1, phi1, th2, phi2):
    return (getGeographicalDistance(th1, phi1, th2, phi2) + 1164.87669839) / 49.4306893399

def computeUnderlayDelay (topo):
    underlay_delay = defaultdict(dict)
    for j,element in enumerate(topo):
        for k in range(j+1, len(topo)):
            geo_delay = getUnderlayDelay(topo[j][4], topo[j][5], topo[k][4], topo[k][5])
            underlay_delay[topo[j][0]][topo[k][0]] = geo_delay
    return underlay_delay

def read_dir_get_delay(work_dir):
    delay_ggr_path = defaultdict(dict)
    walk_dir = os.path.abspath(work_dir)
    nodes = []
    for root, subdir, files in os.walk(walk_dir):
        if 'ping-data' in root:
            parent_node = root.split("/")[10]
            nodes.append(parent_node)
            for file in files:
                destination_node = file.split(".")[0]
                with open (root+"/"+file, 'r') as f:
                    for line in f:
                        if re.search('rtt', line):
                            average_delay = line.split("=")[1].strip().split("/")[1]
                            delay_ggr_path[parent_node][destination_node] = (float(average_delay)/2.0)
    return delay_ggr_path, nodes

#read the ping data files and compute delay along the ggr path

def getUDS(ggr_delay, underlay_delay, nodes):
    stretch = []
    for node in nodes:
        #others = [k for k in nodes if node not in k]
        for others in nodes:
            if node==others:
                continue
            else:
                print(node, others) 
                try:
                    temp_stretch = ggr_delay[node][others]/underlay_delay[node][others]
                    print(ggr_delay[node][others], underlay_delay[node][others])
                except:
                    temp_stretch = ggr_delay[node][others]/underlay_delay[others][node]
                    print(ggr_delay[node][others], underlay_delay[others][node])

if __name__=='__main__':
    #parse argument
    args = parseInput(sys.argv)
    file_name = args.topology_file
    work_dir = args.work_dir
    topo_data = readTopologyFile(file_name)
    underlay_approx = computeUnderlayDelay(topo_data)
    ggr_delay,nodes = read_dir_get_delay(work_dir)
    nodes.sort()
    print(ggr_delay)
    getUDS(ggr_delay, underlay_approx, nodes)
    #try:
        #if underlay_approx['byu']['indonesia']:
        #print(underlay_approx['byu']['indonesia'])
    #except Exception as e:
        #print(underlay_approx['indonesia']['byu'])
    #print(ggr_delay['byu']['byu'])
    #print(ggr_delay['indonesia']['byu'])
