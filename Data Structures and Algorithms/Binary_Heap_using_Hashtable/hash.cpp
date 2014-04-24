/* Ben Schenker
 * 9/19/2012
 * Project 1
 * hash.cpp
 */
#include "hash.h"

//constructor for hashItem
hashTable::hashItem::hashItem() 
{
    key.clear();
    isOccupied = NULL;
    isDeleted = NULL;
    pv = NULL;
};

//constructor for hashTable
hashTable::hashTable(int size)
{
    capacity = getPrime(size);
	data = std::vector<hashItem> (capacity, hashItem());
	filled = 0;
}

//member functions for hashTable

//hash function from book
int hashTable::hash(const std::string &key)
{
	int hashVal=0;
	for(unsigned int i=0;i<key.length();i++)
	{
		hashVal =37*hashVal +key[i];
	}
	hashVal = hashVal%capacity;
	if(hashVal<0) 
	{
		hashVal+=capacity;
	}
	return hashVal;
}


int hashTable::findPos(const std::string &key)
{
	int index=hash(key);
	while (data[index].isOccupied)
	{
		if (key==data[index].key && data[index].isDeleted==0)
			return index;
		
		if (index==capacity-1) //case where index is out of bounds, wraps around
			index =0;
		else
			index++;
	}		
	return -1;
}
bool hashTable::rehash()
{
	int newcapacity = getPrime(capacity); //get next tablesize value
	std::vector<hashItem> Olddata = data; //copy over the old data
	//resize and overwrite the hashtable
	try
	{
		data.resize(newcapacity);
	}
	catch(std::bad_alloc)
	{
		return 0;
	}
	//doesn't actually delete each old entry but this is faster, 
	//can uncomment the other statements to completely delete
	for (int i=0;i<newcapacity;i++)
	{	
		data[i].isOccupied = 0;
		data[i].isDeleted = 0;
		//data[i].key.clear();
		//data[i].pv=NULL;
	}
	//copy over old entries into the resized table
	filled=0;
	for (int j=0;j<capacity;j++)
		if(Olddata[j].isOccupied==1 && Olddata[j].isDeleted==0)
			insert(Olddata[j].key, Olddata[j].pv);

	Olddata.clear(); //frees up the memory
	capacity=newcapacity;
	return 1;
}

int hashTable::getPrime(int size) 
{
	int primes[] = {98317, 196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};
	for (int i=0;i<15;i++)
	{
		if (primes[i]>size)
		{
			return primes[i];
		}
	}
	return 0;
}

int hashTable::insert(const std::string &key, void *pv)
{
	if (filled>=(capacity/2))
	{
		bool rehashing=rehash();
		if (rehashing==0)
			return 2;
	}
	int index=hash(key);
	//linearly probes until it finds an open entry, when done, index is the index of the open entry
	while(data[index].isOccupied)
	{
		if (data[index].key==key && data[index].isDeleted==0)
			return 1;
		if (index==capacity-1) //case where index is out of bounds, wraps around
			index =0;
		else
			index++;
	}
	data[index].key=key;
	data[index].pv=pv;
	data[index].isOccupied=1;
	filled++;
	return 0;
}
bool hashTable::contains(const std::string &key)
{
	int test=findPos(key);
	if (test==-1)
		return 0;
	else
		return 1;
}
void *hashTable::getPointer(const std::string &key, bool *b)
{
	int index = findPos(key);
	if (index==-1)
	{
		if(b!=0)
			*b=0;
		return 0;
	}
	if(b!=0)
		*b=1;
	return data[index].pv;
}
int hashTable::setPointer(const std::string &key, void *pv)
{
  	int index=findPos(key);
  	if (index==-1)
		return 1;
	else
		data[index].pv=pv;
	return 0;
}
bool hashTable::remove(const std::string &key)
{
	int index=findPos(key);
	if (index==-1)
		return 0;
	else
	{	
		data[index].isDeleted=1;
	}
	return 1;
}
