#include <iostream>

using namespace std;

int main() {
    int r=1,s=1,start=1,end=49152;
    for(r=0;r<4;r++){
        for(s=0;s<4;s++){
            
            cout<<"region "<<r+1<<"\t and"<<" segment " << s+1<<" --- starts at \t"<<start<< "\t and ends at \t"<< start+49151 <<endl;
        start+=49152;
        }
    }
    //cout<<"region "<<r <<"and"<<" segment " << s <<" --- starts at"<<start<< "and ends at "<< end <<endl;

    //out<<"Sum of x+y = " << z;
}