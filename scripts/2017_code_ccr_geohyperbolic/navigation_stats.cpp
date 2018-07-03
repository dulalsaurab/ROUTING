//compile with g++ navigation_stats.cpp -fopenmp -O3 -std=c++11


#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <string>
#include <map>
#include <math.h>
#include <omp.h>
#include <unistd.h> 
#include <limits>

//structure to contain all node's information provided by edge list and embedding coordinates
struct node_information{
    std::string node_label; //label read from edge list
    double radial_coordinate; //popularity measure ~ degree
    double theta_coordiante; //from 0 to PI
    double phi_coordinate; //from 0 to 2PI
};


//define double-precision edge weight type
typedef boost::property<boost::edge_weight_t, double> EdgeWeightProperty;

//define Undirected Weighted Graph type with custom node attributes
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, node_information, EdgeWeightProperty> UndirectedWeightedGraph;


//descriptors for vertices and edges for weighted graph
typedef boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor vertex_t_weighted;
typedef boost::graph_traits<UndirectedWeightedGraph>::edge_descriptor edge_t_weighted;


//function to calculate hyperbolic distance in H3 between two nodes
inline double get_hyperbolic_distance(double r1, double theta1, double phi1, double r2, double theta2, double phi2, std::string dist_type){
    double zeta = 1.0;
    double x12;
    double delta_theta = acos(cos(theta1) * cos(theta2) + sin(theta1) * sin(theta2) *cos(phi1 - phi2));
    
    if (dist_type.compare("geo") == 0)
        x12 = 6371.0*delta_theta; //geo-distance
    else if (dist_type.compare("hyp") == 0)
        x12 = (1.0 / zeta) * acosh(cosh(zeta * r1) * cosh(zeta * r2) - sinh(zeta * r1) * sinh(zeta * r2) * cos(delta_theta)); //hyperbolic distance
    else{
        std::cout << "Such distance function is not implemented. Should be either geo or hyp. -1 returned" << std::endl;
        x12 = -1.;
    }
    return x12;
}






int main(int argc, char** argv){

    std::string distance_type;
    //program options passed in command line
    char tmp;
    if(argc == 1){
        std::cout << "Distance type is not provided. Available distances: geo, hyp. Aborting..." << std::endl;
        exit(1);
    }
    while((tmp=getopt(argc, argv, "t:"))!=-1){
        switch(tmp){
            case 't':
                std::cout << "Distance type chosen: " << optarg << std::endl;
                distance_type = std::string(optarg);
                break;
            default:
                std::cout << "Wrong option provided, aborting..." << std::endl;
                exit(1);
            break;
        }
    }

    UndirectedWeightedGraph G_weighted;
    UndirectedWeightedGraph G_unweighted; //this is just for unweighted shortest path search (all weights are = 1)

    std::string coordinates_embedding_path = "./coordinates_embedding.txt";
    std::ifstream file(coordinates_embedding_path.c_str());
    std::string line;
    int line_count = 0;

    //mapping between node labels and their ids to create edges then
    std::map<std::string, int> label_map;

    //add nodes to the graph
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string label;
        double r, theta, phi;
        if (!(iss >> label >> r >> theta >> phi)) {                 // error
            std::cout << "Line " << line_count + 1 << " is broken, breaking the program..." << std::endl;
        } 

        //add current node to the graph
        vertex_t_weighted node_i = boost::add_vertex(G_weighted);
        G_weighted[node_i].node_label = label;
        G_weighted[node_i].radial_coordinate = r;
        G_weighted[node_i].theta_coordiante = theta;
        G_weighted[node_i].phi_coordinate = phi;
        vertex_t_weighted node_i_2 = boost::add_vertex(G_unweighted);
        G_unweighted[node_i_2].node_label = label;
        G_unweighted[node_i_2].radial_coordinate = r;
        G_unweighted[node_i_2].theta_coordiante = theta;
        G_unweighted[node_i_2].phi_coordinate = phi;

        label_map.insert(std::pair<std::string, int>(label, line_count));
        line_count++;
    }

    std::string edgelist_path = "./weighted_enumerated_edgelist.net";
    std::ifstream file2(edgelist_path.c_str());
    line_count = 0;
    while (std::getline(file2, line))
    {
        std::istringstream iss(line);
        std::string label_i, label_j;
        double weight;
        if (!(iss >> label_i >> label_j >> weight)) {                 // error
            std::cout << "Line " << line_count + 1 << " is broken, breaking the program..." << std::endl;
        } 

        //add current edge to the graph
        int node_i = label_map.at(label_i);
        int node_j = label_map.at(label_j);
        boost::add_edge(node_i, node_j, weight, G_weighted);
        boost::add_edge(node_i, node_j, 1.0, G_unweighted); //add edge with w = 1.0
    }

    std::cout << "Number of nodes in the network: " << boost::num_vertices(G_weighted) << std::endl;

    //Routing part starts here
    //=========================================================================================

    //declare an output file which will contain all the data
    std::ofstream output_file;
    output_file.open("./paths_output.dat");

    #pragma omp parallel for schedule(dynamic, 1)
    for(int i = 0; i < boost::num_vertices(G_weighted); i++){
        for(int j = 0; j < boost::num_vertices(G_weighted); j++){
            if(i != j){ //better way of doing this?


            //Dijkstra between a pair of nodes
            int node_source_id = i;
            int node_target_id = j;


            //descriptor for source node
            vertex_t_weighted source = boost::vertex(node_source_id, G_weighted);

            //descriptor for target node
            vertex_t_weighted target = boost::vertex(node_target_id, G_weighted);

            //vector for passed nodes for Dijkstra algorithm
            std::vector<vertex_t_weighted> passed_nodes_dijkstra(boost::num_vertices(G_weighted));

            //vector to contain weight distances from source node for Dijkstra
            std::vector<double> distances_dijkstra(boost::num_vertices(G_weighted));

            //evaluate Dijkstra distances on G_weighted from source node
            boost::dijkstra_shortest_paths(G_weighted, source, boost::predecessor_map(&passed_nodes_dijkstra[0]).distance_map(&distances_dijkstra[0]));

            //
            std::vector<boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor > path;
            boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor current = target;
            
            double cumulative_weight_dijkstra_path = distances_dijkstra[current];
            while(current != source) 
            {
                path.push_back(current);
                current = passed_nodes_dijkstra[current];
            }
            path.push_back(source);

            


            //unweighted shortest path
            //descriptor for source node
            vertex_t_weighted source_unweighted = boost::vertex(node_source_id, G_unweighted);

            //descriptor for target node
            vertex_t_weighted target_unweighted = boost::vertex(node_target_id, G_unweighted);

            //vector for passed nodes for Dijkstra algorithm
            std::vector<vertex_t_weighted> passed_nodes_shortest(boost::num_vertices(G_unweighted));

            //vector to contain weight distances from source node for Dijkstra
            std::vector<double> distances_shortest(boost::num_vertices(G_unweighted));

            //evaluate Dijkstra distances on G_weighted from source node
            boost::dijkstra_shortest_paths(G_unweighted, source_unweighted, boost::predecessor_map(&passed_nodes_shortest[0]).distance_map(&distances_shortest[0]));

            //
            std::vector<boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor > path2;
            current = target;
            
            double cumulative_weight_shortest = distances_shortest[current];
            while(current != source) 
            {
                path2.push_back(current);
                current = passed_nodes_shortest[current];
            }
            path2.push_back(source);



            //hyperbolic routing between a pair of nodes
            
            //pick a source node and get its neighbors
            vertex_t_weighted current_node = source;
            vertex_t_weighted next_node;
            vertex_t_weighted prev_node;
            std::vector<boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor > path3; //this path contains all nodes visited from source to target
            UndirectedWeightedGraph::adjacency_iterator neighbor_it, neighbor_end;

            double cumulative_weight_greedy = 0.0;
            bool routing_status = false; //routing either fails or succeeds

            double r_t = G_weighted[target].radial_coordinate;
            double theta_t = G_weighted[target].theta_coordiante;
            double phi_t = G_weighted[target].phi_coordinate;


            //greedy walk performed here

            while(current_node != target){
                path3.push_back(current_node);
                //get all neighbors of a current node
                boost::tie(neighbor_it, neighbor_end) = boost::adjacent_vertices(current_node, G_weighted);
                
                //get hyperbolic coordinates of current node
                double r1 = G_weighted[current_node].radial_coordinate;
                double theta1 = G_weighted[current_node].theta_coordiante;
                double phi1 = G_weighted[current_node].phi_coordinate;

                double minimal_distance_initial = std::numeric_limits<float>::infinity();

                double minimal_distance_observed = std::numeric_limits<float>::infinity();  //set it to the distance
                                                                                            // between the current node
                                                                                            // and the target if OGF
                                                                                            // and to inf if MGF

                //iterate over neighbors to find closer node

                for(; neighbor_it != neighbor_end; ++neighbor_it){

                    if(*neighbor_it == target){
                        //navigation reached the target

                        //get weight of last edge
                        std::pair<UndirectedWeightedGraph::edge_descriptor, bool> retrievedEdge = boost::edge(current_node, target, G_weighted);
                        //std::cout << get(boost::edge_weight_t(), G_weighted, retrievedEdge.first) << std::endl;
                        cumulative_weight_greedy += get(boost::edge_weight_t(), G_weighted, retrievedEdge.first);

                        //append target to the path
                        path3.push_back(*neighbor_it);

                        //routing is successful
                        routing_status = true;
                        
                        //break for and while loop
                        current_node = target;
                        neighbor_it = neighbor_end;

                        
                    }
                    

                    if(neighbor_it == neighbor_end)
                        break;

                    //coordinate of current neighbor
                    double r2 = G_weighted[*neighbor_it].radial_coordinate;
                    double theta2 = G_weighted[*neighbor_it].theta_coordiante;
                    double phi2 = G_weighted[*neighbor_it].phi_coordinate;

                    double x12 = get_hyperbolic_distance(r2, theta2, phi2, r_t, theta_t, phi_t, distance_type);
                    if(x12 <= minimal_distance_observed){
                        minimal_distance_observed = x12;
                        next_node = *neighbor_it;
                    }
                           
                }


                if(current_node == target)
                    break;

                //check if next node has been visited already
                bool passing_flag = true;
                std::vector<boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor >::iterator it_prev;
                for (it_prev = path3.begin(); it_prev != path3.end(); ++it_prev) {
                    if(next_node == *it_prev){
                        passing_flag = false;
                        break;
                    }
                        
                }


                //now all neighbors are checked, decide where to move next

                //check if minimal observed distance is larger than infinity
                if(minimal_distance_observed == minimal_distance_initial || !passing_flag){
                    routing_status = false;
                    break;
                }

                else{
                    
                    //accumulate weight from this edge
                    std::pair<UndirectedWeightedGraph::edge_descriptor, bool> retrievedEdge = boost::edge(current_node, next_node, G_weighted);
                    cumulative_weight_greedy += get(boost::edge_weight_t(), G_weighted, retrievedEdge.first);
                    current_node = next_node;
                }
                    
            }



            


            //now save all the results into the output file
            //print pair of nodes to navigate into the file


            #pragma omp critical
            {
            output_file << "(" << G_weighted[i].node_label << "," << G_weighted[j].node_label << ")" << std::endl;

            //print greedy path into the file
            std::vector<boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor >::iterator it3;

            output_file << "HG Path: ";
            for (it3 = path3.begin(); it3 != path3.end(); ++it3) {
                output_file << G_weighted[*it3].node_label << " ";
            }

            if(!routing_status)
                output_file << "FAIL";

            output_file << std::endl;

            //print shortest path into the file

            output_file << "Unweighted Shortest Path: ";
            std::vector<boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor >::reverse_iterator it2;

            for (it2 = path2.rbegin(); it2 != path2.rend(); ++it2) {
                output_file << G_unweighted[*it2].node_label << " ";
            }
            output_file << std::endl;


            //print Dijkstra shortest file to the file


            output_file << "Dijkstra Shortest Path: ";
            std::vector<boost::graph_traits<UndirectedWeightedGraph>::vertex_descriptor >::reverse_iterator it;


            for (it = path.rbegin(); it != path.rend(); ++it) {
                output_file << G_weighted[*it].node_label << " ";
            }
            output_file << std::endl;



            //now print accumulated weights along the paths (Dijkstra and greedy only)


            output_file << "Cumulative weight (greedy): " << cumulative_weight_greedy << std::endl;
            output_file << "Cumulative weight (Dijkstra): " << cumulative_weight_dijkstra_path << std::endl;
            output_file << "===" << std::endl;}
                }
        

    } 
    } 

    //close the output file
    output_file.close();



    return 0;
}
