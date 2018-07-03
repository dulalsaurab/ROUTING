import sys
import argparse
import random
import numpy as np
from numpy import sin, cos, sinh, cosh, arccos, arccosh
from collections import defaultdict

def parse_input(args):
    
    parser = argparse.ArgumentParser()

    parser._action_groups.pop()
    required = parser.add_argument_group('required arguments')
    optional = parser.add_argument_group('optional arguments')

    required.add_argument("-n", dest="n_nodes", type=int, required=True,
                        help="Number of nodes in final network")
    required.add_argument("-m", dest="m_links", type=int, required=True,
                        help="Number of new links per new node")
    required.add_argument("-g", dest="network_model", required=True,
                        help="Network generation model")
    required.add_argument("-c", dest="city_data", required=True,
                        help="Path to the formatted city data")
    optional.add_argument("-t", dest="t0", type=int, default=5,
                        help="Initial time when network growth starts (default=5)")
    optional.add_argument("-b", dest="cities_subset",
                        help="Path to the subset of cities to be used (optional)")
    optional.add_argument("-r", dest="randomized", action="store_true",
                        help="Flag if arrivals are randomized (optional)")
    optional.add_argument("-s", dest="rgh_n_seeds", type=int, default=5,
                        help="Number of local hubs per region (only for RGH, default=5)")

    return parser.parse_args()


def compute_RGH_radii(data_dict, sorted_cities, n_seeds):

    dim = 2
    zeta = 1.0
    
    rgh_r = dict()  # city -> r
    
    # create region -> city mapping
    region_city_map = defaultdict(set)
    for city in data_dict:
        region_city_map[data_dict[city][4]].add(city)


    print("Number of regions in the current city partition:", len(region_city_map))
    # create seed nodes dictionary to keep track of already passed seed nodes
    seed_dict = defaultdict(list)

    # fill each region with N_seed nodes
    while all(len(v) < n_seeds for v in seed_dict.values()):
        for city, _ in sorted_cities:
            region = data_dict[city][4]
            if len(seed_dict[region]) >= n_seeds:
                pass
            else:
                seed_dict[region].append(city)

    # assign coordinates of seed nodes
    nodes_marked = set()
    for iteration in range(n_seeds):
        for region in seed_dict.keys():
            if iteration < len(seed_dict[region]):
                new_node = seed_dict[region][iteration]
                radius = (2. / (dim * zeta)) * np.log(t_start + iteration)
                rgh_r[new_node] = radius
                nodes_marked.add(new_node)

    # print out seed nodes populations
    seed_pops = []
    for city in nodes_marked:
        seed_pops.append(data_dict[city][1])
        # print "Region", data_dict[city][4], "City:", data_dict[city][0], "Pop:", data_dict[city][1]
        


    # assign coordinates of new nodes
    t = n_seeds + t_start            # start with time = N_seeds + initial time
    for city, _ in sorted_cities:    # by the population
        if city not in nodes_marked: # ignore seed nodes here
            new_node = city
            radius = (2. / (dim * zeta)) * np.log(t)
            rgh_r[new_node] = radius
            nodes_marked.add(new_node)
            t += 1

    # now check if all cities got their radii
    for city in data_dict:
        if city not in rgh_r.keys():
            print("City "+data_dict[city][0]+" did not get radial coordinate, aborting...")
            sys.exit()

    return rgh_r


# function to find hyperbolic distance between two nodes
def get_hyperbolic_distance(r1, th1, phi1, r2, th2, phi2):
    zeta = 1.0
    dtheta = arccos(cos(th1)*cos(th2) + sin(th1)*sin(th2)*cos(phi1 - phi2)) # angular distance on S2
    x_ij = (1./zeta) * arccosh(cosh(zeta*r1)*cosh(zeta*r2) - sinh(zeta*r1)*sinh(zeta*r2)*cos(dtheta))
    return x_ij


# function to find geographic distance between two nodes
def get_geographic_distance(th1, phi1, th2, phi2):
    R_earth = 6371.0
    dtheta = arccos(cos(th1)*cos(th2) + sin(th1)*sin(th2)*cos(phi1 - phi2))
    x_ij = R_earth * dtheta
    return x_ij


def get_distance(model, coors):
    [r1, th1, phi1, r2, th2, phi2] = map(float, coors)
    if model.lower() == "h3" or model.lower() == "rgh":
        return get_hyperbolic_distance(r1, th1, phi1, r2, th2, phi2)
    elif model.lower() == "geo":
        return get_geographic_distance(th1, phi1, th2, phi2)
    else:
        print("Model mispecified! "+model)
        sys.exit()

        
# city data should be of the form: unique_name population latitude longitude region unique_id 
def read_city_data_from_file(datafile):

    # load city data into a dictionary
    data_dict = defaultdict(list) # city_id : [name, population, theta, phi, region, radius]
    with open(datafile, 'r') as f:
        rows = (line.split() for line in f)
        for city_name, population, theta, tmp_phi, region, unique_id in rows:

            # theta to radians
            theta = (-float(theta) + 90.0) * np.pi / 180.0
            
            # calculate phi
            tmp_phi = float(tmp_phi)
            if float(tmp_phi) < 0.0:
                phi = 2 * np.pi - np.abs(tmp_phi * np.pi / 180.0)
            else:
                phi = tmp_phi * np.pi / 180.0

            # create data_dict - radius to be assigned later
            data_dict[int(unique_id.strip())] = [city_name, int(population), float(theta), float(phi), region, None]

    print("City data loaded.")
    return data_dict


def write_coordinates_to_file(data_dict, cities_added):
    coordinates_file = "coordinates_embedding.txt"
    with open(coordinates_file, "w") as f:
        for city in cities_added:
            r, theta, phi = data_dict[city][5], data_dict[city][2], data_dict[city][3]
            f.write(str(city) + " " + str(r) + " " + str(theta) + " " + str(phi) + "\n")
    print("Coordinates saved to "+coordinates_file)

    
def write_edgelist_to_file(data_dict, edgelist):
    # go over added edges and measure their delay
    # using (distance + 1164.87669839)/49.4306893399
    with open("weighted_enumerated_edgelist.net", "w") as f:
        #we need only weighted edgelist with a new navigation script
        for node1, node2 in edgelist:
            theta1, phi1 = float(data_dict[node1][2]), float(data_dict[node1][3])
            theta2, phi2 = float(data_dict[node2][2]), float(data_dict[node2][3])
            geo_distance_12 = get_geographic_distance(theta1, phi1, theta2, phi2)
            delay = (geo_distance_12 + 1164.87669839) / 49.4306893399
            f.write(str(node1) + " " + str(node2) + " " + str(delay) + "\n")
    print("Weighted edgelist saved to weighted_enumerated_edgelist.net")


# function to generate network according to one of three models
def generate_network(N, m, t_start, model, data_dict, randomized, n_seeds, subset_cities):

    # variables
    dim = 2
    edgelist_dir = "."

    # sort cities by population
    sorted_cities = sorted(data_dict.items(), key = lambda x: x[1][1], reverse = True)

    # we call this function to get all radii if using RGH
    if model.lower() == "rgh":
        rgh_radius = compute_RGH_radii(data_dict, sorted_cities, n_seeds)
    
    t = 0
    for city, _ in sorted_cities:

        # calculate radii for each model
        r = 0.0
        if model.lower() == "h3":
            zeta = 1.0
            r = (2. / (zeta * dim)) * np.log(t_start + t) #derived for exactly d = 2
        elif model.lower() == "rgh":
            r = rgh_radius[city]
            
        p = data_dict[city][1]            
        #r = np.log(t_start * pop_max / p)
        theta, phi = data_dict[city][2], data_dict[city][3]
        data_dict[city][5] = r
        t += 1

    # now decide whether model should go over top-N cities
    # or only use some subset provided in separate file
    if subset_cities:

        # create city -> unique_id mapping to use for subset cities
        city_id_map = defaultdict(int)
        for city in data_dict:
            city_id_map[data_dict[city][0]] = city

        cities_to_consider = set()
        with open(subset_cities, "r") as f:
            for line in f:
                city_to_consider = line.strip().split()[0]
                cities_to_consider.add(city_id_map[city_to_consider])

        #sort only subset cities
        new_data_dict = defaultdict(list)
        for key, value in data_dict.items():
            if key in cities_to_consider:
                new_data_dict[key] = data_dict[key]

        # XXX Here we rewrite sorted_cities with only the subset cities sorted
        # and from now on, we use sorted_cities for everything
        sorted_cities = sorted(new_data_dict.items(), key=lambda x: x[1][1], reverse=True)

        # only for randomizd RGH model
        if model.lower() == "rgh" and randomized:
            print("Randomized arrivals mode...")
            ordered_nodes = 205 # iterate over all cities in population order to get all 5 hubs
                                # in all 14 regions (parameters for the paper)

            # order first ordered_nodes and take others in random order
            cities_to_shuffle = sorted_cities[ordered_nodes:]
            random.shuffle(cities_to_shuffle)
            sorted_cities[ordered_nodes:] = cities_to_shuffle

    current_time = 0
    cities_added = set()
    edgelist = set()
    for city, _ in sorted_cities:
        if len(cities_added) < N:
            print(str(current_time)+":"+data_dict[city][0])
            r1 = data_dict[city][5]
            theta1 = data_dict[city][2]
            phi1 = data_dict[city][3]
            if current_time > m:
                #connect to only m closest nodes
                all_possible_connections = defaultdict(float)
                for node in cities_added:
                    r2 = data_dict[node][5]
                    theta2 = data_dict[node][2]
                    phi2 = data_dict[node][3]
                    x_12 = get_distance(model, [r1, theta1, phi1, r2, theta2, phi2])
                    all_possible_connections[node] = x_12
                sorted_connections = sorted(all_possible_connections.items(), key = lambda x: x[1], reverse = False)
                for neighbor,_ in sorted_connections[:m]:
                        edgelist.add((city, neighbor))
            else:
                #connect to all existing nodes 
                for node in cities_added:
                    edgelist.add((city, node))
            cities_added.add(city)
            current_time += 1
        else:
            break

    return data_dict, cities_added, edgelist


if __name__ == '__main__':

    ## parse args
    args = parse_input(sys.argv)

    # required args
    N = args.n_nodes
    m = args.m_links
    model = args.network_model
    formatted_city_data = args.city_data
    
    # optional args
    t_start = 5
    if args.t0: t_start = args.t0
    randomized = args.randomized # default = False
    subset_cities = args.cities_subset # default = False
        
    # only for RHG
    n_seeds = 5
    if model.lower() == "rhg" and args.rgh_nseeds:
        n_seeds = args.rgh_n_seeds

        
    ## read city data
    data_dict = read_city_data_from_file(formatted_city_data)

    ## call generating functions
    data_dict, cities_added, edgelist = generate_network(N, m, t_start, model, data_dict, randomized, n_seeds, subset_cities)

    # save coordinates for cities added
    write_coordinates_to_file(data_dict, cities_added)

    # save edgelist 
    write_edgelist_to_file(data_dict, edgelist)
    
