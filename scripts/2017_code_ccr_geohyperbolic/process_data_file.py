import sys
import time
import itertools
import numpy as np
from collections import defaultdict
from matplotlib import pyplot as plt

def get_stats(filename):
    data_dict = defaultdict(dict)

    line_count = 0
    success_paths = 0
    fail_paths = 0
    delay_stretches = []
    first_stretches = []

    worst_path = 0.0
    worst_pair = None
    worst_hops = str()
    current_hr_path = str()
    current_dijkstra_path = str()

    with open("delay_stretches.dat", 'w') as f2:
        with open(filename, 'r') as f:
            for line in f:
                if len(line) > 3:
                    if line[0] == "(":
                        line_count += 1
                        line = line.strip()
                        line_nodes = line.split(",")
                        node_i = line.strip().split(",")[0][1:]
                        node_j = line.strip().split(",")[1][:-1]
                        current_pair = (node_i, node_j)
                        data_dict[current_pair] = {"GHR Status": None, "Delay Stretch": None, "Stretch1": None, "GHR Pathlength": None,\
                                                    "GHR Delay": None, "Dijkstra Delay": None, "Shortest Pathlength": None}
                    elif line[0:2] == "HG":
                        if "FAIL" in line:
                            data_dict[current_pair]["GHR Status"] = 0
                        else:
                            data_dict[current_pair]["GHR Status"] = 1
                            data_dict[current_pair]["GHR Pathlength"] = len(line.strip().split()[2:]) - 1
                            current_hr_path = " ".join(line.strip().split()[2:])
                    elif line[0:3] == "Dij":
                        current_dijkstra_path = " ".join(line.strip().split()[3:])

                    elif line[0:2] == "Un":
                        data_dict[current_pair]["Shortest Pathlength"] = len(line.strip().split()[3:]) - 1

                    elif "(greedy)" in line:
                        data_dict[current_pair]["GHR Delay"] = float(line.strip().split()[-1])

                    elif "(Dijkstra)" in line:
                        data_dict[current_pair]["Dijkstra Delay"] = float(line.strip().split()[-1])
                        if data_dict[current_pair]["Dijkstra Delay"]:
                            #compute everything here for this pair and delete it from the dictionary not to overload memory
                            if data_dict[current_pair]["GHR Status"] == 0:
                                fail_paths += 1
                            else:
                                success_paths += 1
                                first_stretches.append(float(data_dict[current_pair]["GHR Pathlength"]) / data_dict[current_pair]["Shortest Pathlength"])
                                f2.write("(" + str(current_pair[0]) + "," + str(current_pair[1]) + "): " + str(data_dict[current_pair]["GHR Delay"] / data_dict[current_pair]["Dijkstra Delay"]) + "\n")
                                delay_stretches.append(data_dict[current_pair]["GHR Delay"] / data_dict[current_pair]["Dijkstra Delay"])
                                if worst_path < data_dict[current_pair]["GHR Delay"] / data_dict[current_pair]["Dijkstra Delay"]:
                                    worst_path = data_dict[current_pair]["GHR Delay"] / data_dict[current_pair]["Dijkstra Delay"]
                                    worst_pair = current_pair
                                    worst_hops = current_dijkstra_path + " | " + current_hr_path
                            data_dict.pop(current_pair)

    print ("Data is loaded, calculating statistics...")
    print ("Success Ratio: "+str(float(success_paths) / line_count))
    print ("Stretch 1: "+str(np.mean(first_stretches)))
    print ("Average Delay Stretch: "+str(np.mean(delay_stretches)))
    
    return delay_stretches, float(success_paths) / line_count, worst_path


# function to calculate cumulative ping distributions along all greedy paths
def get_cumulative_ping_distribution(file_name, coordinates_file, edgelist_file):
    # dictionary to keep angular coordinates of the nodes
    geo_coordinates_dict = defaultdict(list)

    # calculate number of nodes in the network using the coordinates file
    with open(coordinates_file, "r") as f:
        for line in f:
            node, radial, theta, phi = line.strip().split()
            geo_coordinates_dict[node] = [float(theta), float(phi)] #theta, phi
    f.close()
    N = len(geo_coordinates_dict.keys())
    
    # create an edgelist-weight dictionary
    weight_dict = defaultdict(float)
    with open(edgelist_file, "r") as f:
        for line in f:
            node1, node2, weight = line.strip().split()
            weight_dict[(node1, node2)] = float(weight)
    f.close()

    # create an empty array of floats to store the cumulative pings
    cumulative_pings = np.zeros((N*N, 1))
    geo_delays = np.zeros((N*N, 1))
    i = -1
    with open(file_name, "r") as f:
        for line in f:
            if line[0] == "(":
                node_i = line.strip().split(",")[0][1:]
                node_j = line.strip().split(",")[1][:-1]
                current_pair = (node_i, node_j)  
                [theta_i, phi_i] = geo_coordinates_dict[node_i]
                [theta_j, phi_j] = geo_coordinates_dict[node_j]
                current_geodesic_distance = 6371.0*np.arccos(np.cos(theta_i)*np.cos(theta_j) + np.sin(theta_i)*np.sin(theta_j)*np.cos(phi_i - phi_j))          
                i += 1
            elif line[0:2] == "HG":
                if "FAIL" in line:
                    current_geodesic_distance = -1.
                else:
                    path = line.strip().split()[2:]
                    #iterate over consecutive pairs
                    cumulative_ping = 0.0
                    for node1, node2 in zip(path, path[1:]):
                        if (node1, node2) in weight_dict:
                            cumulative_ping += weight_dict[(node1, node2)]
                        elif (node2, node1) in weight_dict:
                            cumulative_ping += weight_dict[(node2, node1)]
                        else:
                            print ("Some problem occurred with a pair", (node1, node2), "aborting...")
                            return None
                    cumulative_pings[i,0] = cumulative_ping
                    geo_delays[i,0] = round(49.4306893399 * float(cumulative_ping) / (current_geodesic_distance + 1164.87669839), 5)
                    if geo_delays[i,0] < 1:
                        print ("HERE ---->", current_pair, cumulative_ping, geo_delays[i,0])

    
    geo_delays = geo_delays[np.nonzero(geo_delays)] # treat zero geo delays
    return geo_delays


if __name__ == '__main__':

    coordinates_file = "coordinates_embedding.txt"
    edgelist_file = "weighted_enumerated_edgelist.net"

    
    if len(sys.argv) != 2:
        print ("Please provide path to the file with navigation data.")
        sys.exit()
    else:
        file_name = sys.argv[1]
        delay_stretches, success_ratio, worst_path = get_stats(file_name)
        geo_delays = get_cumulative_ping_distribution(file_name, coordinates_file, edgelist_file)
        
        with open("./results_stats.dat", "w") as f:
            average_gs = np.mean(geo_delays)
            average_ds = np.mean(delay_stretches)
            f.write("Success Ratio: " + str(success_ratio) + "\n")
            f.write("Average ODS: " + str(average_ds) + "\n")
            f.write("Average UDS: " + str(average_gs) + "\n")
            f.write("Worst path ODS: " + str(worst_path) + "\n")

            percentiles = [50, 75, 80, 90, 95, 99]
            for p in percentiles:
                ds_percentile = np.percentile(delay_stretches, p)
                f.write("ODS "+str(p)+"-th percentile: " + str(ds_percentile) + "\n")
            for p in percentiles:
                gs_percentile = np.percentile(geo_delays, p)
                f.write("UDS "+str(p)+"-th percentile: " + str(gs_percentile) + "\n")
        f.close()
