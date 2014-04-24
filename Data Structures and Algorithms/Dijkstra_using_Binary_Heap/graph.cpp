/* Ben Schenker
 * 11/19/2012
 * Project 3
 * graph.cpp
 */

#include "graph.h"
#define MAX 1000000
//constructor for adjNode, the subclass of node
graph::node::adjNode::adjNode(node* node, int cost)
{
	dest=node;
	Cost=cost;
}

//constructor for node subclass of graph
graph::node::node(const std::string &vertexID)
{
	id=vertexID;
	edges.clear();
	dist = MAX;
	known = 0;
	prev = 0;

}

//constructor for graph
graph::graph()
{
	mapping = new hashTable();
	Nodes.clear();
}


graph::node* graph::findNode(std::string vertexID)
{
	bool b;
	bool *pb=&b;
	node *pn=static_cast<node*>(mapping->getPointer(vertexID, pb));
	if (!b)
		return 0;
	else
		return pn;
}

void graph::insert(const std::string &sourceID, const std::string &destID, int cost)
{
	//first see if source vertex exists, then add edge to it
	node* b=findNode(sourceID);
	node* d = findNode(destID);
	if(!b)// source does not exist, must create node
	{
		node* source = new node(sourceID);
		mapping->insert(sourceID, source);
		Nodes.push_back(source);
		b=source;
	}

	//then see if second vertex exists, if not then create it
	
	if(!d)// dest does not exist, must create node
	{
		node* dest = new node(destID);
		mapping->insert(destID, dest);
		Nodes.push_back(dest);
		d=dest;
	}

	b->edges.push_back(node::adjNode(d, cost)); //add edge to adjacency list of source

}

bool graph::dijkstra(std::string sourceID)
{
	node* b=findNode(sourceID);
	if (!b)
		return 0;//if node does not exist in graph
	myheap = new heap(Nodes.size());
	for (list<graph::node*>::iterator it = Nodes.begin(); it != Nodes.end(); it++)//add pointer to every node onto heap
	{
		node* current_node=*it;
		myheap->insert(current_node->id, MAX, current_node);//initialize their value to MAX
	}	
	//set first node as first node
	b->dist = 0;
	myheap->setKey(sourceID, 0);
	b->prev = NULL;

	int work = 0;
	node* current; //pointer to the current node
	int pkey;
	while(!work)//loop through all nodes on the heap
	{
		work = myheap->deleteMin(NULL, &pkey, &current);//current is a pointer to the node with the smallest distance that had been on the heap
		current->known = 1;
		int dv = current->dist;
		for (list<graph::node::adjNode>::iterator it = current->edges.begin(); it != current->edges.end(); it++)
		{
			node* w = it->dest;
			int Cvw = it->Cost;
			int total_cost = dv + Cvw;
			if ( !w->known && total_cost < (w->dist) )
			{
				w->dist = total_cost;
				myheap->setKey(w->id, total_cost );
				w->prev = current;
			}
		}
	}
	return 1;
}

string graph::out()
{
	stack<string> vertices;
	stringstream oss;
	for (list<graph::node*>::iterator it = Nodes.begin(); it != Nodes.end(); it++)//iterates through list of nodes in the order that we encountered them from the input file
    {
    	node* current_node=*it;
    	oss<<current_node->id<<": ";
    	if (current_node->dist==MAX)//nodes that werent visited with dijkstra have no path from source
    			oss<<"NO PATH"<<endl;
    	else//uses a string stack to have outup in correct order
    	{
    		oss<<current_node->dist<<" [";

			while(current_node->prev || current_node->dist)//pushes every nodeid onto the string stack except the sourceid
			{
				vertices.push(current_node->id);
				current_node=current_node->prev;
			}
			vertices.push(current_node->id);//pushes the sourceid onto the string stack
			
			oss<<vertices.top();
			vertices.pop();
			while(!vertices.empty())//pops all strings off the stack
			{
				oss<<", "<<vertices.top();
				vertices.pop();
			}

    		oss<<"]"<<endl;
    	}
    }
	return oss.str();
}
