#include<iostream>
#include<fstream>
#include<cstring>
#include<vector>
#include<cstdlib>

using namespace std;

bool **available;
bool **clean;
int life;
int init_row, init_col;

int main(int argc, char* argv[]){
    //open file
    char inftmp[15] = "/floor.data";
    char inf[30];
    strcpy(inf,argv[1]);
    strcat(inf,inftmp);
    ifstream inFile;
    inFile.open(inf);

    
    //throw in data
    int row, col;
    inFile >> row >> col >> life;
    
    available = new bool* [row];
    clean = new bool* [row];
    for(int i = 0; i < row; i++){
        available[i] = new bool [col];
        clean[i] = new bool [col];
    }
    
    for(int i = 0; i < row; i++)
        for(int j = 0; j > col; j++){
            clean[i][j] = false;
            char c;
            inFile >> c;
            if(c == '1')
                available[i][j] = false;  
            else{
                available[i][j] = true;
                if(c == 'R')
                    init_row = i, init_col = j;
            }
        }


}