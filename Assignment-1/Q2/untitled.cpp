#include<bits/stdc++.h>
using namespace std;

int main()
{
	fstream fin,fout;
	fin.open("file2.txt",ios::in);
	fout.open("out2.txt",ios::out);
	
	vector<string> row; 
    string line, word, temp; 

	while(fin >> temp){   
	    row.clear(); 
	    getline(fin, line);  
	    stringstream s(line);  
	    while(getline(s, word, ' ')) { 
	  		row.push_back(word); 
	    } 
	    if(row[6].substr(0,5) == "time="){
	    	fout << row[6].substr(5,3) << ' ';
	    }  
	} 
	return 0;
}