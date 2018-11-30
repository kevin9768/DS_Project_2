#include<fstream>
#include<iostream>
#include<cstring>
#include<stack>
#include<queue>
#include<vector>
#include<cmath>
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3


using namespace std;

struct Point{
    int r,c;
    Point(int x, int y): r(x), c(y){}
    Point(): r(0), c(0){}
};

struct Data{
    bool available;
    bool init_clean;
    bool final_clean;
    int min_dist = 1000000;
};

struct bfs_Data{
    bool clean;
    int min_dist = 1000000;
};

int total_row, total_col;
int life;
int r_row, r_col;
int size_dir[4] = {0}; //0:上 1:右 2:下 3:左
bool** toclean;
bfs_Data** Up;
bfs_Data** Down;
bfs_Data** Right;
bfs_Data** Left;
vector<Point> order, final_order;

void open_data(ifstream &floor, char* id){
    char floor_data[30];
    strcpy(floor_data, id);
    strcat(floor_data, "/floor.data");
    floor.open(floor_data);
}

void bfs(bfs_Data **dir, Data **mat, int r, int c);
void dfs(bfs_Data **dir, Data **d, int r, int c);
inline bool isAdj(Point a, Point b);
void gotobattery(Point p, Data** mat);
void s_gotobattery(Point p, Data** mat);
int getback(Point p, Data** mat, int dir);
void from_a_to_b(int a, int b, Data** mat);
int pathfinder(Point a, Point b, Data** mat);

int main(int argc, char* argv[]){
    ifstream inFile;
    open_data(inFile, argv[1]);
    inFile >> total_row >> total_col >> life;
    

    //initialize 2d-array
    Data **mat;
    mat  = new Data* [total_row];
    toclean = new bool* [total_row];
    for(int i=0; i<total_row; i++){
        mat[i] = new Data [total_col];
        toclean[i] = new bool [total_col];
    }

    //input data
    for(int i=0; i<total_row; i++)
        for(int j=0; j<total_col; j++){
            char c;
            inFile >> c;
            if(c=='1'){
                mat[i][j].available = false;
                toclean[i][j] = mat[i][j].init_clean = mat[i][j].final_clean = true;
            }
            else{
                mat[i][j].available = true;
                toclean[i][j] = mat[i][j].init_clean = mat[i][j].final_clean = false;
                if(c=='R'){
                    r_row = i, r_col = j;
                    toclean[i][j] = mat[i][j].init_clean = mat[i][j].final_clean = true;
                }
            }
        }


    //determine the order to clean/
    for(int i=0; i<4; i++){
        int init_row = r_row, init_col = r_col;
        int dirsize = order.size();
        switch(i){
            case UP:
                if(total_row>0 && mat[init_row-1][init_col].available){
                    init_row--;
                    bfs(Up, mat, init_row, init_col);
                    dfs(Up, mat, init_row, init_col);
                    mat[r_row][r_col].available = true;
                }
                break;
            case RIGHT:
                if(init_col<total_col-1 && mat[init_row][init_col+1].available){
                    init_col++;
                    bfs(Right, mat, init_row, init_col);
                    dfs(Right, mat, init_row, init_col);
                    mat[r_row][r_col].available = true;
                }
                break;
            case DOWN:
                if(init_row<total_row-1 && mat[init_row+1][init_col].available){
                    init_row++;
                    bfs(Down, mat, init_row, init_col);
                    dfs(Down, mat, init_row, init_col);
                    mat[r_row][r_col].available = true;
                }
                break;
            case LEFT:
                if(total_col>0 && mat[init_row][init_col-1].available){
                    init_col--;
                    bfs(Left, mat, init_row, init_col);
                    dfs(Left, mat, init_row, init_col);
                    mat[r_row][r_col].available = true;
                }
                break;            
        }
        size_dir[i] = order.size();
        cout<<size_dir[i]<<' ';
        if(i==3)    cout<<'\n';
    }
    vector<bfs_Data**> each_dir = {Up, Right, Down, Left};

    for(int i=0;i<4;i++)
        for(int j=0;j<total_row;j++){
            for(int k=0;k<total_col;k++){
                if(each_dir[i][j][k].min_dist==1000000) cout<<"x ";
                else cout<<each_dir[i][j][k].min_dist<<' ';
            }
            cout<<'\n';
        }
    
}

void bfs(bfs_Data **d, Data **mat, int r_r, int r_c){
    
    d = new bfs_Data* [total_row];
    for(int i=0; i<total_row; i++)
        d[i] = new bfs_Data [total_col];
    for(int i=0; i<total_row; i++)
        for(int j=0; j<total_col; j++)
            d[i][j].clean = toclean[i][j];
    
    d[r_row][r_col].clean=true;
    mat[r_row][r_col].available = false;
    queue<Point> q;
    Point u, upos, in;
    u.r = r_r, u.c = r_c;
    d[u.r][u.c].min_dist = 0;
    d[u.r][u.c].clean = true;
    q.push(u);
    while(!q.empty()){
        upos = q.front();
        q.pop();
        if(upos.r>0 && mat[upos.r-1][upos.c].available && !d[upos.r-1][upos.c].clean){
            in.r = upos.r - 1;
            in.c = upos.c;
            d[upos.r-1][upos.c].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r-1][upos.c].clean = true;
        }
        if(upos.r+1<total_row && mat[upos.r+1][upos.c].available && !d[upos.r+1][upos.c].clean){
            in.r = upos.r + 1;
            in.c = upos.c;
            d[upos.r+1][upos.c].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r+1][upos.c].clean = true;
        }
        if(upos.c>0 && mat[upos.r][upos.c-1].available && !d[upos.r][upos.c-1].clean){
            in.r = upos.r;
            in.c = upos.c - 1;
            d[upos.r][upos.c-1].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r][upos.c-1].clean = true;
        }
        if(upos.c+1<total_col && mat[upos.r][upos.c+1].available && !d[upos.r][upos.c+1].clean){
            in.r = upos.r;
            in.c = upos.c + 1;
            d[upos.r][upos.c+1].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r][upos.c+1].clean = true;
        }
    }
    for(int i=0; i<total_row; i++)
        for(int j=0; j<total_col; j++)
            d[i][j].clean = toclean[i][j];
}

void dfs(bfs_Data** dir, Data **mat, int r, int c){
    mat[r][c].init_clean = true;
    if(!mat[r][c].available)  return;
    Point p(r,c);
    order.push_back(p);
    if(r>0 && !dir[r-1][c].clean && dir[r-1][c].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r-1,c);
    if(c>0 && !dir[r][c-1].clean && dir[r][c-1].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r,c-1);
    if(r+1<total_row && !dir[r+1][c].clean && dir[r+1][c].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r+1,c);
    if(c+1<total_col && !dir[r][c+1].clean && dir[r][c+1].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r,c+1);
}