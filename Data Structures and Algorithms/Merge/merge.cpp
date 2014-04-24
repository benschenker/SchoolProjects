#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cctype>
#include <stack>

using namespace std;

main()
{
	string a, b, merge, outfilename, infilename, line;
	int i, j, n, m;

    //cout<<"enter filename for input: ";
    //cin>>infilename;
    ifstream myfile ("in.txt");//infilename.c_str());
    //cout<<"enter filename for output: ";
    //cin>>outfilename;
    ofstream outfile ("out.txt");//outfilename.c_str());

    if (myfile.is_open())
    {
        while(getline (myfile, line)) // reads each line until the null character
        {
            //line+="0";
            a=line;
            getline (myfile, line);//assumes lines in file is a multiple of 3
            //line+="0";
            b=line;
            getline (myfile, line);
            //line+="1";
            merge=line;
			//assume all have proper values
			bool Matrix [a.length()+1] [b.length()+1];// to make room for sides!
			stringstream oss;
			for (j=0;j<b.length()+1; j++)
			{
				for(i=0;i<a.length()+1;i++)
				{
					Matrix[i][j]=false;
				}
			}
			Matrix[0][0]=true;//initialize first value to one

			for (j=0;j<b.length()+1; j++)
			{
				for(i=0;i<a.length()+1;i++)
				{
					cout<<Matrix[i][j];
				}
				cout<< '\n';
			}

			for (j=0;j<b.length()+1; j++)
			{
				for(i=0;i<a.length()+1;i++)
				{
					if(!Matrix[i][j])// skips if the current entry is zero(unnecessary calculation)
						continue;
					if(i==a.length())//avoids going out of bounds on the rows
					{
						if(b[j]==merge[i+j])
							Matrix[i][j+1] = true;
						continue;
					}
					if (j==b.length())//avoids going out of bounds on the columns
					{
						if(a[i]==merge[i+j])
							Matrix[i+1][j] = true;
						continue;
					}

					if(a[i]==merge[i+j])
						Matrix[i+1][j] = true;
					if(b[j]==merge[i+j])
						Matrix[i][j+1] = true;
				}
			}

		//test!
			cout<<"after operations"<<endl;
			for (j=0;j<b.length()+1; j++)
			{
				for(i=0;i<a.length()+1;i++)
				{
					cout<<Matrix[i][j];
				}
				cout << '\n';
			}
			i=a.length();
			j=b.length();
			char adding;
		//assuming i= a.length()+1 and j=b.length()+1
			if(!Matrix[i][j])
				outfile<<"*** NOT A MERGE ***"<<endl;
			else
			{
				while(i>0||j>0)
				{
					if(j>0 && Matrix[i][j-1])//make sure dont go out of bounds
					{
						oss<<b[j-1];
						j--;
					}
					else if(i>0 && Matrix[i-1][j])//make sure dont go out of bounds
					{
						adding = a[i-1]-32;
						oss<<adding;
						i--;
					}
					else
						cout<<"please help me!"<<endl;
				}
				string reverse= oss.str();
				outfile<<string ( reverse.rbegin(), reverse.rend() )<<endl;
			}
				
        }
        myfile.close();
    }
    else cout<< "Unable to open file"<<endl;




}

