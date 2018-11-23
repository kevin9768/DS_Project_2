#include<iostream>
#include<fstream>
#include<cstring>
#include<vector>
#include<cstdlib>

using namespace std;

struct point{
    int x,y;
    point(int a,int b):x(a),y(b){}
};

bool **available;
bool **dfs_clean;
bool **trav_clean;
int row, col;
int life;
int r_row, r_col;
vector<point> order;


void dfs(int r, int c);
void mindist_from_init(int r, int c);

int main(int argc, char* argv[]){
    //open file
    char inftmp[15] = "/floor.data";
    char inf[30];
    strcpy(inf,argv[1]);
    strcat(inf,inftmp);
    ifstream inFile;
    inFile.open(inf);

    //throw in data
    inFile >> row >> col >> life;

    available  = new bool* [row];
    dfs_clean  = new bool* [row];
    trav_clean = new bool* [row];
    for(int i = 0; i < row; i++){
        available[i]  = new bool [col];
        dfs_clean[i]  = new bool [col];
        trav_clean[i] = new bool [col];
    }

    //setup 2d-arrays
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++){
            dfs_clean[i][j]  = false;
            trav_clean[i][j] = false;
            char c;
            inFile >> c;
            if(c == '1')
                available[i][j] = false;  
            else{
                available[i][j] = true;
                if(c == 'R'){
                    r_row = i, r_col = j;
                    dfs_clean[i][j]  = true;
                    trav_clean[i][j] = true;
                }
            }
        }
    
    //find out the order to go through these point

    int init_row=r_row, init_col=r_col;
    //determine which point to go out through starting point
    if(     init_col<col-1  &&  available[init_row][init_col+1])      init_col++;
    else if(init_row<row-1  &&  available[init_row+1][init_col])      init_row++;
    else if(init_col>0      &&  available[init_row][init_col-1])      init_col--;
    else if(init_row>0      &&  available[init_row-1][init_col])      init_row--;

    dfs(init_row,init_col);

    mindist_from_init(init_row,init_col);
}

void dfs(int r, int c){
    dfs_clean[r][c]=true;
    cout<<r<<' '<<c<<'\n';
    point p(r,c);
    order.push_back(p);
    if(r>0&&available[r-1][c]&&!(dfs_clean[r-1][c]))           dfs(r-1,c);
    if(c>0&&available[r][c-1]&&!(dfs_clean[r][c-1]))           dfs(r,c-1);
    if(r<row-1&&available[r+1][c]&&!(dfs_clean[r+1][c]))       dfs(r+1,c);
    if(c<col-1&&available[r][c+1]&&!(dfs_clean[r][c+1]))       dfs(r,c+1);
}