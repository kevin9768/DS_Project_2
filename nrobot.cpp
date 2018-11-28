#include<fstream>
#include<iostream>
#include<cstring>
#include<stack>
#include<queue>
#include<vector>
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

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
};

struct bfs_Data{
    bool clean;
    int min_dist;
};

int total_row, total_col;
int life;
int r_row, r_col;
int size_dir[4] = {0};
vector<Point> order, final_order;

void open_data(ifstream &floor, char* id){
    char floor_data[30];
    strcpy(floor_data, id);
    strcat(floor_data, "/floor.data");
    floor.open(floor_data);
}

void bfs(bfs_Data **a, Data **d, int r_r, int r_c);
void dfs(bfs_Data **a, Data **d, int r_r, int r_c);

int main(int argc, char* argv[]){
    ifstream inFile;
    open_data(inFile, argv[1]);
    inFile >> total_row >> total_col >> life;
    

    //initialize 2d-array
    Data     **mat;
    bfs_Data **dist;
    mat  = new Data*     [total_row];
    dist = new bfs_Data* [total_row];
    for(int i=0; i<total_row; i++){
        mat[i]  = new Data     [total_col];
        dist[i] = new bfs_Data [total_col];
    }

    //input data
    for(int i=0; i<total_row; i++)
        for(int j=0; j<total_col; j++){
            char c;
            inFile >> c;
            if(c=='1'){
                mat[i][j].available = false;
                mat[i][j].init_clean = mat[i][j].final_clean = true;
            }
            else{
                mat[i][j].available = true;
                mat[i][j].init_clean = mat[i][j].final_clean = false;
                if(c=='R'){
                    r_row = i, r_col = j;
                }
            }
        }

    //bfs to find shortest path
    bfs(dist, mat, r_row, r_col);


    //determine the order to clean
    for(int i=0; i<4; i++){
        int init_row = r_row, init_col = r_col;
        int dirsize = order.size();
        switch(i){
            case UP:
                if(total_row>0 && mat[init_row-1][init_col].available)
                    init_row--;
                break;
            case DOWN:
                if(init_row<total_row-1 && mat[init_row+1][init_col].available)
                    init_row++;
                break;
            case LEFT:
                if(total_col>0 && mat[init_row][init_col-1].available)
                    init_col--;
                break;
            case RIGHT:
                if(init_col<total_col-1 && mat[init_row][init_col+1].available)
                    init_col++;
                break;
        }
        dfs(dist, mat, init_row, init_col);
        size_dir[i] = order.size() - dirsize;
    }
}


void bfs(bfs_Data **a, Data **d, int r_r, int r_c){
    queue<Point> q;
    Point u, upos, in;
    u.r = r_r, u.c = r_c;
    a[u.r][u.c].min_dist = 0;
    a[u.r][u.c].clean = true;

    q.push(u);
    while(!q.empty()){
        upos = q.front();
        q.pop();
        if(upos.r>0 && d[upos.r-1][upos.c].available && !a[upos.r-1][upos.c].clean){
            in.r = upos.r - 1;
            in.c = upos.c;
            a[upos.r-1][upos.c].min_dist = a[upos.r][upos.c].min_dist + 1;
            q.push(in);
            a[upos.r-1][upos.c].clean = true;
        }
        if(upos.r+1<total_row && d[upos.r+1][upos.c].available && !a[upos.r+1][upos.c].clean){
            in.r = upos.r + 1;
            in.c = upos.c;
            a[upos.r+1][upos.c].min_dist = a[upos.r][upos.c].min_dist + 1;
            q.push(in);
            a[upos.r+1][upos.c].clean = true;
        }
        if(upos.c>0 && d[upos.r][upos.c-1].available && !a[upos.r][upos.c-1].clean){
            in.r = upos.r;
            in.c = upos.c - 1;
            a[upos.r][upos.c-1].min_dist = a[upos.r][upos.c].min_dist + 1;
            q.push(in);
            a[upos.r][upos.c-1].clean = true;
        }
        if(upos.c+1<total_col && d[upos.r][upos.c+1].available && !a[upos.r][upos.c+1].clean){
            in.r = upos.r;
            in.c = upos.c + 1;
            a[upos.r][upos.c+1].min_dist = a[upos.r][upos.c].min_dist + 1;
            q.push(in);
            a[upos.r][upos.c+1].clean = true;
        }
    }
}

void dfs(bfs_Data **a, Data **d, int r, int c){
    if(a[r][c].min_dist*2>life) return;
    d[r][c].init_clean = true;
    if(!d[r][c].available)  return;
    Point p(r,c);
    order.push_back(p);
    if(r>0 && !d[r-1][c].init_clean && a[r-1][c].min_dist==a[r][c].min_dist+1)
        dfs(a,d,r-1,c);
    if(c>0 && !d[r][c-1].init_clean && a[r][c-1].min_dist==a[r][c].min_dist+1)
        dfs(a,d,r,c-1);
    if(r+1<total_row && !d[r+1][c].init_clean && a[r+1][c].min_dist==a[r][c].min_dist+1)
        dfs(a,d,r+1,c);
    if(c+1<total_col && !d[r][c+1].init_clean && a[r][c+1].min_dist==a[r][c].min_dist+1)
        dfs(a,d,r,c+1);
}