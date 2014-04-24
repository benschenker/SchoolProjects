/* Ben Schenker
 * 9/19/2012
 * Project 1
 * spellcheck.cpp
 */
#include "hash.h"

using namespace std;

int main(int argc, char** argv) {
    string outfilename, infilename, dictname;
    cout<<"enter name of dictionary: ";
    cin>>dictname;
    ifstream dict (dictname.c_str());
    clock_t t1 = clock();
    string line;
    hashTable dictionary;
    if (dict.is_open())
    {
        while(getline (dict, line)) // reads each line until the null character
        {
           transform(line.begin(), line.end(), line.begin(),::tolower); //converts to lowercase
           dictionary.insert(line);
        }
        dict.close();
    }

    else cout<< "Unable to open file"<<endl;


    clock_t t2 = clock();
    double ldtime = ((double) (t2 - t1)) / CLOCKS_PER_SEC;
    cout<<"total time(in seconds) to load dictionary: ";
    cout<< ldtime<<endl;
    
    cout<<"enter filename for input: ";
    cin>>infilename;
    ifstream document (infilename.c_str());
    
    cout<<"enter filename for output: ";
    cin>>outfilename;
    ofstream outfile (outfilename.c_str());
    clock_t t3 = clock();

    string validchars ="abcdefghijklmnopqrstuvwxyz0123456789-'";
    if (document.is_open())
    {
        int linecount=1;
        size_t found;
        string word;
        while(getline (document, line)) // reads each line until the null character
        {
            transform(line.begin(), line.end(), line.begin(),::tolower); //converts to lowercase
            while (line.size()!=0)
            {
                //parses into words seperated by delimiters by finding the first non-valid character
                found=line.find_first_not_of(validchars);
                if (found==0)//multiple delimiters in a row
                {
                    line.erase(0,1);
                    continue; 
                }
                else if (found!=string::npos) //not last word on line
                {
                    word = line.substr(0,found); //just take "word"
                    line.erase(0,found+1); //cut out word and delimiter
                }

                else //last word in line
                {
                    word = line;
                    line.clear(); //cut out word and delimiter
                }
                if (word.length()>20)
                    outfile<<"Long word at line "<<linecount<<", starts: "<<word.substr(0,20)<<endl;
                else if (dictionary.contains(word)==0)
                    if(word.find_first_of("0123456789")==string::npos)
                        outfile<<"Unknown word at line "<<linecount<<": "<<word<<endl;            
            }
            linecount++;
        }
        document.close();
    }

    else cout<< "Unable to open file"<<endl;
    clock_t t4 = clock();
    double chktime = ((double) (t4 - t3)) / CLOCKS_PER_SEC;
    cout<<"total time(in seconds) to check document: ";
    cout<< chktime<<endl;
    
    return 0;
}

