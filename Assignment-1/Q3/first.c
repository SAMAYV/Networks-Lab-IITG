#include<bits/stdc++.h>
using namespace std;

int main(){
 fstream file; string word,filename;
 filename="first.txt";
 file.open(filename.c_str());
 int c=0;
 vector<string> v;
 
 while(file >> word){
   if(word[0]=='t'&&word[1]=='i'&&word[2]=='m' &&word[3]=='e' &&word[4]=='=' ){
   int n=word.length();
   string s="";
  
   for(int i=5;i<n;i++)s+=word[i];
   v.push_back(s);
  }
   }
   sort(v.begin(),v.end());
   for(int i=0;i<v.size();i++)cout << v[i] <<'\n';
   cout << v[511] << '\n'<<  v[512] <<'\n'; 
   return 0;
   }
