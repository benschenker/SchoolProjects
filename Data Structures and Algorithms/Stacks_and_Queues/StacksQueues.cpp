/*
BENJAMIN SCHENKER
DUE 4/4/12

This program written in c++ reads in a file with specific formatting of commmands and creates an output file with the results.
It implements the data structures stack and queue (with support for lists of integers, strings, and doubles) using a 
custom templated abstract list class called SimpleList.  
It also keeps track of these various SimpleLists using the built in list class in c++.  
It has minor error checking such that a pop call to an empty list or a push to a nonexistent list, 
or a creation of a duplicate list will return a specified error.  
There is also a small amount of syntax checking which will only output to the command line if a problem occured.

an example of valid syntax would be as follows:

create il queue
push il 17
pop il 

this would create a queue of integers called il (note that the type is corresponding to the first letter of the name of the list) 
then push the value "17" onto it
then pop "17" and return it

*/
#include<iostream>
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<list>

using namespace std;

template <typename Object>
class SimpleList;

template <typename Object>
class Stack;

template <typename Object>
class Queue;

template <typename Object>
SimpleList<Object>* nameexists (list<SimpleList<Object> *> *listSL, string Listname);

// SimpleLists is a templated abstract list class from which most of the properties of the list classes "Queue" and "Stack" derive
template <typename Object>
class SimpleList
{
    private:
        //Node is the subclass of Simplelist which makes up the building block of each entry in a list.  It contains a value and a pointer to the next node
        class Node
        {    
            friend class SimpleList<Object>;

            private:
                Object data;
                Node* next;
            
            public:
                Node( Object d = NULL, Node* n = NULL ) :data(d), next(n) {} // Node constructor    
        };

        int size; // keeps track of # of nodes in SimpleList
        Node* tail, *head; // tail and head are pointers to the last and first nodes in the SimpleList respectively
        string name; // name of SimpleList

    protected:
        //insert node at beginning of SimpleList
        void Insert_Begin (Object& x) 
        {
            Node* firstnode = new Node (x, head);
            head = firstnode;
            if (this->isempty())
                tail = firstnode;
            size++;
        }

        //insert node at end of SimpleList
        void Insert_End (Object& x) 
        {
            Node* lastnode = new Node (x, NULL); 
            if (this->isempty())
                head = lastnode;
            else
                tail->next = lastnode;
            tail = lastnode;
            size++;
        }

        //remove node at the beginning of SimpleList and returns data from that node
        const Object Remove_Begin () 
        {
            Object Data = head->data;
            Node* newfirst = head->next;
            delete head;
            if (this->Size() == 1 )
            {
                head = NULL;
                tail = NULL;
            }
            else
                head = newfirst;
            size--;
            return Data;
        }

    public:
        // SimpleList constructor
        SimpleList(string N) 
        : size(0), tail(NULL), head(tail), name(N) {} 

        int Size (void)
        {
            return size;
        }
        
        // returns TRUE if SimpleList is empty
        bool isempty(void)
        {
            return size == 0;
        }

        // since the "pop" command is the same for  both "Queue" and "Stack", it is specified in the abstract class
        // pop removes the node at the beginning of the list and returns its data
        Object pop (void)
        {
            Object d;
            if (isempty() == 0)
                d = Remove_Begin();
            
            return d;
        }

        // since "push" is different for both derived classes, it is only declared virtually here
        virtual void push (Object x) {}

        // returns the name of the SimpleList
        string Nameis(void)
        {
            return name;
        }
};

// Stack is a class derived from SimpleList
template <typename Object>
class Stack : public SimpleList<Object>
{
    public:
    Stack<Object>(string N) : SimpleList<Object>(N) {} // calls the SimpleList constructor
            
            // declares its specific implementation of the push function of the abstract class
            void push (Object x)
            {
                Insert_Begin(x);
            }
};

// Queue is a class derived from SimpleList
template <typename Object>
class Queue : public SimpleList<Object>
{
    public:
            Queue<Object>(string N) : SimpleList<Object>(N) {} // calls the SimpleList constructor
            
            // declares its specific implementation of the push function of the abstract class
            void push (Object x)
            {
                Insert_End(x);
            }
};

// nameexists is a templated function that looks for a SimpleList with a specified name and returns that value if it finds it otherwise it returns NULL
template <typename Object>
SimpleList<Object>* nameexists (list<SimpleList<Object> *> *listSL, string Listname) 
{   
    // creates iterator that iterates through pointers to lists and does so checking if they have the same name as the Listname argument that was passed in
    for (typename list<SimpleList<Object> *>::iterator itr = listSL->begin(); itr != listSL->end(); itr++) 
    {
        if ( (*itr)->Nameis() == Listname )
            return *itr;
    }
    return NULL;
}

int main()
{
    //creates a list of pointers to each type of SimpleList
    list<SimpleList<int> *> listSLi;
    list<SimpleList<double> *> listSLd;
    list<SimpleList<string> *> listSLs;

    string outfilename, infilename, line;
    cout<<"enter filename for input: ";
    cin>>infilename;
    ifstream myfile (infilename.c_str());
    cout<<"enter filename for output: ";
    cin>>outfilename;
    ofstream outfile (outfilename.c_str());


    if (myfile.is_open())
    {
        while(getline (myfile, line)) // reads each line until the null character
        {
            outfile<<"PROCESSING COMMAND: "<<line<<endl;
            stringstream iss(line); //turns the line which was read in into a stream so that each word can be doled out to the proper variable
            /* command will contain the first word on the line; listname will contain the second;  
            listtype, values, valuei, or valued will contain the third word when applicable*/
            string command, listname, listtype, values; 
            int valuei;
            double valued;
            iss >> command >> listname;

            // main parsing loop
            if (command == "create")
            {	
    			iss>>listtype;
				if (listname[0] =='i')
				{   
					SimpleList<int>* NameExistsi = nameexists<int>(&listSLi,listname); // calls the nameexists function for this specific case
					if (NameExistsi == NULL)
					{
						SimpleList<int> *pSLi;
						if (listtype == "stack")                 
						{
							pSLi = new Stack<int> (listname); // creates the SimpleList with the specified listname
							listSLi.push_front(pSLi); // adds the pointer to the SimpleList onto the list of pointers
                            // these two steps are done for all of the cases with slightly different calls to the templated functions and classes
						}
						else if (listtype == "queue")
						{
							pSLi = new Queue<int> (listname);
							listSLi.push_front(pSLi);
						}
						else
							cout<<"INVALID LISTTYPE: "<< line<<endl; // not required but cant hurt to make sure syntax is correct.  outputs to command line, not textfile
					}
					else
						outfile<<"ERROR: This name already exists!"<<endl;
				}
				else if (listname[0] == 's')
				{
					SimpleList<string>* NameExistss = nameexists<string>(&listSLs,listname);
					if (NameExistss == NULL)
					{
						SimpleList<string> *pSLs;
						if (listtype == "stack")
						{ 
							pSLs = new Stack<string> (listname);
							listSLs.push_front(pSLs);
						}
						else if (listtype == "queue")
						{
							pSLs = new Queue<string> (listname);
							listSLs.push_front(pSLs);
						}
						else
							cout<<"INVALID LISTTYPE: "<< line<<endl;
					}
					else
						outfile<<"ERROR: This name already exists!"<<endl;
				}
				else if (listname[0] == 'd')
				{
					SimpleList<double>* NameExistsd = nameexists<double>(&listSLd,listname);
					if (NameExistsd == NULL)
					{
						SimpleList<double> *pSLd;
						if (listtype == "stack")
						{   
							pSLd = new Stack<double> (listname);
							listSLd.push_front(pSLd);
						}
						else if (listtype == "queue")  
						{   
							pSLd = new Queue<double> (listname);
							listSLd.push_front(pSLd);
						}
						else
							cout<<"INVALID LISTTYPE: "<< line<<endl;
					}
					else
						outfile<<"ERROR: This name already exists!"<<endl;
				}
				else
					cout<<"INVALID LISTNAME: "<< line<<endl;	
    		}

            if (command == "push")
            {
    			
				if (listname[0] == 'i')
				{   
					SimpleList<int>* NameExistsi = nameexists<int>(&listSLi,listname);
					if (NameExistsi !=NULL)
					{
						iss>>valuei;
						NameExistsi->push(valuei);
					}
					else
						outfile<<"ERROR: This name does not exist!"<<endl;
				}
				else if (listname[0] == 's')
				{     
					SimpleList<string>* NameExistss = nameexists<string>(&listSLs,listname);
					if (NameExistss !=NULL)
					{
						iss>>values;
						NameExistss->push(values);
					}
					else
						outfile<<"ERROR: This name does not exist!"<<endl;
				}
				else if (listname[0] == 'd')
				{	
					SimpleList<double>* NameExistsd = nameexists<double>(&listSLd,listname);
					if (NameExistsd !=NULL)
					{
						iss>>valued;
						NameExistsd->push(valued);
					}
					else
						outfile<<"ERROR: This name does not exist!"<<endl;
				}
				else
				    cout<<"INVALID LISTNAME: "<< line<<endl;
                
            }

            if (command == "pop")
            {
				if (listname[0] == 'i')
				{     
					SimpleList<int>* NameExistsi = nameexists<int>(&listSLi,listname);
					if (NameExistsi !=NULL)
					{
						if(! NameExistsi->isempty())
						{
							int popi = NameExistsi->pop();
							outfile<<"Value popped: "<< popi<<endl;
						}
						else
							outfile<<"ERROR: This list is empty!"<<endl;
					}
					else
						outfile<<"ERROR: This name does not exist!"<<endl;
				}
				else if (listname[0] == 's')
				{   
					SimpleList<string>* NameExistss = nameexists<string>(&listSLs,listname);
					if (NameExistss !=NULL)
					{
						if(! NameExistss->isempty())
						{
							string pops = NameExistss->pop();
							outfile<<"Value popped: "<< pops<<endl;
						}
						else
							outfile<<"ERROR: This list is empty!"<<endl;
					}
					else
						outfile<<"ERROR: This name does not exist!"<<endl;
				}
				else if (listname[0] == 'd')
				{	
					SimpleList<double>* NameExistsd = nameexists<double>(&listSLd,listname);
					if (NameExistsd !=NULL)
					{
						if(! NameExistsd->isempty())
						{
							double popd = NameExistsd->pop();
							outfile<<"Value popped: "<< popd<<endl;
						}
						else
							outfile<<"ERROR: This list is empty!"<<endl;
					}
					else
						outfile<<"ERROR: This name does not exist!"<<endl;
				}
				else
					cout<<"INVALID LISTNAME: "<< line<<endl;   
            }

        }
        myfile.close();
    }

    else cout<< "Unable to open file"<<endl;

    return 0;
}
