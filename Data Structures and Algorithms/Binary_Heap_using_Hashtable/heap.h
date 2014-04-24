/* Ben Schenker
 * 2012
 * Project 2
 * heap.h
 */
using namespace std;
#include "hash.h"

class heap {
    public:
        //
        // heap - The constructor allocates space for the nodes of the heap
        // and the mapping (hash table) based on the specified capacity
        //
        heap(int heapsize=0);

        //
        // insert - Inserts a new node into the binary heap
        //
        // Inserts a node with the specified id string, key,
        // and optionally a pointer.                They key is used to
        // determine the final position of the new node.
        //
        // Returns:
        //                 0 on success
        //                 1 if the heap is already filled to capacity
        //                 2 if a node with the given id already exists (but the heap
        //                                 is not filled to capacity)
        //
        int insert(const std::string &id, int key, void *pv = NULL);

        //
        // setKey - set the key of the specified node to the specified value
        //
        // I have decided that the class should provide this member function
        // instead of two separate increaseKey and decreaseKey functions.
        //
        // Returns:
        //         0 on success
        //         1 if a node with the given id does not exist
        //
        int setKey(const std::string &id, int key);

        //
        // deleteMin - return the data associated with the smallest key
        //                                                 and delete that node from the binary heap
        //
        // If pId is supplied (i.e., it is not NULL), write to that address
        // the id of the node being deleted. If pKey is supplied, write to
        // that address the key of the node being deleted. If ppData is
        // supplied, write to that address the associated void pointer.
        //
        // Returns:
        //         0 on success
        //         1 if the heap is empty
        //
        int deleteMin(std::string *pId = NULL, int *pKey = NULL, void *ppData = NULL);

        //
        // remove - delete the node with the specified id from the binary heap
        //
        // If pKey is supplied, write to that address the key of the node
        // being deleted. If ppData is supplied, write to that address the
        // associated void pointer.
        //
        // Returns:
        //         0 on success
        //         1 if a node with the given id does not exist
        //
        int remove(const std::string &id, int *pKey = NULL, void *ppData = NULL);

    private:

        class node { // A nested class within heap
        public:
                node();
                node(const std::string &ID, int KEY, void *pv = NULL);
                std::string id; // The id of this node
                void* pData; // A pointer to the actual data
                int key; // The key of this node
        };

        int capacity; //total possible number of items that can be stored on the heap
        int size; //current amount of items on the heap
        std::vector<node> data; // The actual binary heap
        hashTable *mapping; // maps ids to node pointers
        //percolateUp - takes the index of a potentially out of place node
        //and swaps the node with its parent if they are out of order
        //it then recursively calls percolateUp. It keeps going
        //until it either reaches the root or the ordering property is not violated 
        void percolateUp(int posCur);
        //percolateDown - takes the index of a potentially out of place node
        //and swaps the node with its smallest child if they are out of order
        //it then recursively calls percolateDown. It keeps going
        //until it either reaches a leaf or the ordering property is not violated 
        void percolateDown(int posCur);
        //getPos - takes a pointer to a node in the heap and returns an index in 
        //the heap for that particular node
        int getPos(node *pn);

};