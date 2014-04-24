/* Ben Schenker
 * 11/19/2012
 * Project 3
 * dijkstra.cpp
 */

#include "graph.h"
using namespace std;


int main()
{

    string outfilename, infilename, line, startID;
    cout<<"enter filename for input: ";
    cin>>infilename;
    ifstream myfile (infilename.c_str());
    graph mygraph;


    if (myfile.is_open())
    {
        while(getline (myfile, line)) // reads each line until the null character
        {
            stringstream iss(line); 
            // turns the line which was read in into a stream so that each word 
            // can be doled out to the proper variable
            
            string source, sink;
            int cost;
            iss >> source >> sink >>cost;
            mygraph.insert(source, sink, cost);
        }
        myfile.close();
    }
    else cout<< "Unable to open file"<<endl;
    
    // prompts user to enter id of valid start vertex
    // this will reprompt the user until the vertex is found to be in the graph
    bool b = 0;;
    clock_t t1 = clock();
    while (!b)
    {
    	cout<<"enter id of starting vertex: ";
    	cin>>startID;
    	
   		b = mygraph.dijkstra(startID);
   	}
    
    clock_t t2 = clock();
    double ldtime = ((double) (t2 - t1)) / CLOCKS_PER_SEC;
    cout<<"total time(in seconds) to execute Dijkstra's algorithm: ";
    cout<< ldtime<<endl;

    cout<<"enter filename for output: ";
    cin>>outfilename;
    ofstream outfile (outfilename.c_str());
    string outtext = mygraph.out();
    outfile<<outtext;
    
    return 0;
}
