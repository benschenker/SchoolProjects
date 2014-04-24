/* Ben Schenker
 * 10/9/2012
 * Project 2
 * heap.cpp
 */
#include "heap.h"

//constructor for nested class node
heap::node::node()
{
	id.clear();
	key=0;
	pData=NULL;
}
//overloaded version for insert
heap::node::node(const std::string &ID, int KEY, void *pv)
{
	id=ID;
	key=KEY;
	pData=pv;
}

//constructor for heap
heap::heap(int heapsize)
{
	capacity=heapsize;
	size=0;
	data = std::vector<node> (capacity+1, node());
	//capacity+1 allows for the unused node at entry 0 in the vector

	mapping = new hashTable(capacity*2);
}

void heap::percolateUp(int posCur)
{
	//find the parent
	int parent = posCur/2;
	//set temp
	node temp=node();

	
	if (parent>=1)//if the node is not the root
	{
		if (data[posCur].key<data[parent].key)//checks for ordering property
		{
			//swap
			temp=data[parent];
			data[parent]=data[posCur];
			data[posCur]=temp;
			//update hashtable
			mapping->setPointer(data[parent].id, &data[parent]);
			mapping->setPointer(data[posCur].id, &data[posCur]);

			percolateUp(parent);
		}
	}
}

void heap::percolateDown(int posCur)
{
	//find the child
	int Lchild = posCur*2;
	int Rchild = Lchild+1;
	int min;
	//set temp
	node temp=node();
	
	if (Rchild>size) //no right child
	{
		if (Lchild>size) //no children
			return;		
		else //only left child
			min=Lchild;
	}
	else //has 2 children
	{
		if (data[Lchild].key<=data[Rchild].key) //right is bigger
			min=Lchild;
		else //right is smaller
			min=Rchild;
	}
	//now that min contains index of the smaller child, 
	//check for ordering property and potentially swap
		if (data[min].key<data[posCur].key) 
		{
			//swap
			temp=data[min];
			data[min]=data[posCur];
			data[posCur]=temp;
			//update hashtable
			mapping->setPointer(data[posCur].id, &data[posCur]);
			mapping->setPointer(data[min].id, &data[min]);

			percolateDown(min);
		}
	
}

int heap::getPos(node *pn)
{
	int pos = pn - &data[0];
	return pos;
}

int heap::insert(const std::string &id, int key, void *pv)
{
	if (size==capacity) //if full
		return 1;
	if (mapping->contains(id))
		return 2;
	//add the data to the end of the heap 
	size++;
	data[size].id=id;
	data[size].key=key;
	data[size].pData=pv;
	mapping->insert(id, &data[size]);
	percolateUp(size);
	return 0;
}
int heap::setKey(const std::string &id, int key)
{
	bool b;
	bool *pb=&b;
	node *pn=static_cast<node*>(mapping->getPointer(id, pb));
	if (!b)
		return 1;
	else
	{
		int index=getPos(pn);
		if (data[index].key<key) //key is going to grow
		{
			data[index].key=key;
			percolateDown(index);
		}
		else //key going to get smaller or stay the same
		{
			data[index].key=key;
			percolateUp(index);
		}
		return 0;
	}
}
int heap::deleteMin(std::string *pId, int *pKey, void *ppData)
{
	if (size==0)
		return 1;
	if (pId)
		*pId=data[1].id;
	if (pKey)
		*pKey=data[1].key;
	if (ppData)
		*(static_cast<void **> (ppData)) = data[1].pData;
	mapping->remove(data[1].id);
	data[1]=data[size];
	mapping->setPointer(data[1].id, &data[1]);
	size--;
	percolateDown(1);
	return 0;
}
int heap::remove(const std::string &id, int *pKey, void *ppData)
{
	bool b;
	bool *pb=&b;
	node *pn=static_cast<node*>(mapping->getPointer(id, pb));
	int key=pn->key; //for use later to determine which way to percolate
	if (!b)
		return 1;
	if (pKey)
		*pKey=key;
	if (ppData)
		*(static_cast<void **> (ppData)) = pn->pData;

	mapping->remove(id);
	int index=getPos(pn);
	data[index]=data[size];
	mapping->setPointer(data[index].id, &data[index]);
	size--;

	if (key<data[index].key) //key grew
		percolateDown(index);
	
	else //key got smaller or stayed the same
		percolateUp(index);
	
	return 0;
}