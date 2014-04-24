/* Ben Schenker
 * 11/19/2012
 * Project 3
 * graph.h
 */
using namespace std;
#include "heap.h"

class graph {
    public:
        //
        // graph - The constructor creates a linked list of nodes for the graph
        // and the mapping (hash table) based on the specified capacity
        //
        graph();

        //
        // insert - Inserts a new edge into the graph
        //
        // Checks to see if the vertices exist (if they don't it adds them)
        // then adds the edge and cost into the adjacency list of the proper node
        // of the linked list 
        //
        // Returns:
        //                 true on success
        //                 false otherwise
        //
        void insert(const std::string &sourceID, const std::string &destID, int cost);

        //
        // dijkstra - performs dijkstra's algorithm on the graph 
	//
        // Returns:
        //          1 on success
        //	    0 on failure
        //
        bool dijkstra(std::string sourceID);
        // out - properly outputs the results of Dijkstra's algorithm
        string out();

    private:

        class node { // A nested class within graph
        	public:
	    	    node(const std::string &vertexID);
	            std::string id; // The id of this node
	            int dist; //distance from the sourceid
	            bool known; // whether the node has been visited yet via dijkstra
	            node* prev; // pointer to the previous node that connects this node with the source
        		class adjNode{ //a nested class within graph within node
        			public:
        				adjNode(node* node, int cost);
        				node* dest; //pointer to the destination node of the edge
        				int Cost; //cost of the edge
        			};
        		list<adjNode> edges; // adjacency list for each node
        };
        hashTable *mapping; // maps ids to node pointers
        heap *myheap;
        list<graph::node*> Nodes; //list of nodes in the order they were encountered in the input file

        //findNode - given a string corresponding to a vertex, 
        // returns pointer to the vertex node if it exist
        // returns 0 if the vertex does not exist on the hashtable
        //
        // 
        graph::node* findNode(std::string vertexID);        
};
