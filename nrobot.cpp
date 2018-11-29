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
    bool clean;
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
vector<Point> order, final_order;

void open_data(ifstream &floor, char* id){
    char floor_data[30];
    strcpy(floor_data, id);
    strcat(floor_data, "/floor.data");
    floor.open(floor_data);
}

void bfs(Data **d, int r_r, int r_c);
void bfs(bfs_Data **d, Data **a, int r_r, int r_c);
void dfs(Data **d, int r_r, int r_c);
inline bool isAdj(Point a, Point b);
void gotobattery(Point p, Data** d);
void s_gotobattery(Point p, Data** d);
int getback(Point p, Data** d);
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
                toclean[i][j] = mat[i][j].clean = mat[i][j].init_clean = mat[i][j].final_clean = true;
            }
            else{
                mat[i][j].available = true;
                toclean[i][j] = mat[i][j].clean = mat[i][j].init_clean = mat[i][j].final_clean = false;
                if(c=='R'){
                    r_row = i, r_col = j;
                }
            }
        }

    //bfs to find shortest path
    bfs(mat, r_row, r_col);


    //determine the order to clean
    for(int i=0; i<4; i++){
        int init_row = r_row, init_col = r_col;
        int dirsize = order.size();
        switch(i){
            case UP:
                if(total_row>0 && mat[init_row-1][init_col].available){
                    init_row--;
                    dfs(mat, init_row, init_col);
                }
                break;
            case RIGHT:
                if(init_col<total_col-1 && mat[init_row][init_col+1].available){
                    init_col++;
                    dfs(mat, init_row, init_col);
                }
                break;
            case DOWN:
                if(init_row<total_row-1 && mat[init_row+1][init_col].available){
                    init_row++;
                    dfs(mat, init_row, init_col);
                }
                break;
            case LEFT:
                if(total_col>0 && mat[init_row][init_col-1].available){
                    init_col--;
                    dfs(mat, init_row, init_col);
                }
                break;            
        }
        size_dir[i] = order.size();
        cout<<size_dir[i]<<' ';
        if(i==3)    cout<<'\n';
    }
    
    
    int j=0;
    for(int i=0;i<order.size();i++){
        if(i==size_dir[j]){
            switch(j){
                case 0:
                    cout<<"up to right\n";
                    break;
                case 1:
                    cout<<"right to down\n";
                    break;
                case 2:
                    cout<<"down to left\n";
                    break;
            }
            j++;
        }
        cout<<order[i].r<<' '<<order[i].c<<'\n';
    }



    //find whole path
    final_order.push_back(order[0]);
    int fix=1, s=0, bt=life;
    while(size_dir[s]==0)   s++;
    bt--;
    for(int i=1;i<order.size();i++){
        if(mat[order[i].r][order[i].c].final_clean){
            fix++;
            continue;
        }
        else    fix=1;

        if(i==size_dir[s]){
            int origin_dir = s;
            s++;
            while(size_dir[s]==size_dir[s-1])   s++;
            int dest_dir = s;

            //final_order.push_back(Point(origin_dir*10000,dest_dir*10000));

            s_gotobattery(order[i-fix], mat);
            from_a_to_b(origin_dir, dest_dir, mat);
            final_order.push_back(Point(r_row,r_col));
            bt = getback(order[i], mat);

            //final_order.push_back(Point(origin_dir*10000,dest_dir*10000));

            break;
        }

        if(isAdj(order[i-fix],order[i])){
            int distance = mat[order[i].r][order[i].c].min_dist;
            if(bt-1>=distance){
                bt--;
                final_order.push_back(order[i]);
                mat[order[i].r][order[i].c].final_clean = true;
            }
            else{
                gotobattery(order[i-fix], mat);
                if(i==order.size()) break;
                bt = getback(order[i],mat);
            }
        }
        else{
            gotobattery(order[i-fix], mat);
            if(i==order.size()) break;
            bt = getback(order[i],mat);
        }        
    }
    gotobattery(order[order.size()-1], mat);

    //output
    char outftmp[15] = "/final.path";
    char outf[30];
    strcpy(outf,argv[1]);
    strcat(outf,outftmp);
    ofstream outFile;
    outFile.open(outf);
    
    outFile<<final_order.size()<<'\n';
    for(int i=0; i<final_order.size(); i++){
        outFile<<final_order[i].r<<' '<<final_order[i].c<<'\n';
    }
}


void bfs(Data **d, int r_r, int r_c){
    queue<Point> q;
    Point u, upos, in;
    u.r = r_r, u.c = r_c;
    d[u.r][u.c].min_dist = 0;
    d[u.r][u.c].clean = true;

    q.push(u);
    while(!q.empty()){
        upos = q.front();
        q.pop();
        if(upos.r>0 && d[upos.r-1][upos.c].available && !d[upos.r-1][upos.c].clean){
            in.r = upos.r - 1;
            in.c = upos.c;
            d[upos.r-1][upos.c].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r-1][upos.c].clean = true;
        }
        if(upos.r+1<total_row && d[upos.r+1][upos.c].available && !d[upos.r+1][upos.c].clean){
            in.r = upos.r + 1;
            in.c = upos.c;
            d[upos.r+1][upos.c].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r+1][upos.c].clean = true;
        }
        if(upos.c>0 && d[upos.r][upos.c-1].available && !d[upos.r][upos.c-1].clean){
            in.r = upos.r;
            in.c = upos.c - 1;
            d[upos.r][upos.c-1].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r][upos.c-1].clean = true;
        }
        if(upos.c+1<total_col && d[upos.r][upos.c+1].available && !d[upos.r][upos.c+1].clean){
            in.r = upos.r;
            in.c = upos.c + 1;
            d[upos.r][upos.c+1].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r][upos.c+1].clean = true;
        }
    }
}
void bfs(bfs_Data **d, Data **a, int r_r, int r_c){
    queue<Point> q;
    Point u, upos, in;
    u.r = r_r, u.c = r_c;
    d[u.r][u.c].min_dist = 0;
    d[u.r][u.c].clean = true;

    q.push(u);
    while(!q.empty()){
        upos = q.front();
        q.pop();
        if(upos.r>0 && a[upos.r-1][upos.c].available && !d[upos.r-1][upos.c].clean){
            in.r = upos.r - 1;
            in.c = upos.c;
            d[upos.r-1][upos.c].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r-1][upos.c].clean = true;
        }
        if(upos.r+1<total_row && a[upos.r+1][upos.c].available && !d[upos.r+1][upos.c].clean){
            in.r = upos.r + 1;
            in.c = upos.c;
            d[upos.r+1][upos.c].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r+1][upos.c].clean = true;
        }
        if(upos.c>0 && a[upos.r][upos.c-1].available && !d[upos.r][upos.c-1].clean){
            in.r = upos.r;
            in.c = upos.c - 1;
            d[upos.r][upos.c-1].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r][upos.c-1].clean = true;
        }
        if(upos.c+1<total_col && a[upos.r][upos.c+1].available && !d[upos.r][upos.c+1].clean){
            in.r = upos.r;
            in.c = upos.c + 1;
            d[upos.r][upos.c+1].min_dist = d[upos.r][upos.c].min_dist + 1;
            q.push(in);
            d[upos.r][upos.c+1].clean = true;
        }
    }
}

void dfs(Data **d, int r, int c){
    d[r][c].init_clean = true;
    if(!d[r][c].available)  return;
    Point p(r,c);
    order.push_back(p);
    if(r>0 && !d[r-1][c].init_clean && d[r-1][c].min_dist==d[r][c].min_dist+1)
        dfs(d,r-1,c);
    if(c>0 && !d[r][c-1].init_clean && d[r][c-1].min_dist==d[r][c].min_dist+1)
        dfs(d,r,c-1);
    if(r+1<total_row && !d[r+1][c].init_clean && d[r+1][c].min_dist==d[r][c].min_dist+1)
        dfs(d,r+1,c);
    if(c+1<total_col && !d[r][c+1].init_clean && d[r][c+1].min_dist==d[r][c].min_dist+1)
        dfs(d,r,c+1);
}

inline bool isAdj(Point a, Point b){
    if(abs(a.r-b.r)+abs(a.c-b.c)==1)    return true;
    return false;
}

void gotobattery(Point p, Data** d){
   if(final_order.back().r==r_row&&final_order.back().c==r_col) return;
    while(d[p.r][p.c].min_dist>0){
        if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r-1][p.c].final_clean)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r][p.c-1].final_clean)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r+1][p.c].final_clean)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r][p.c+1].final_clean)
            p.c++;
        else if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist)
            p.c++;
        d[p.r][p.c].final_clean = true;
        final_order.push_back(p);
    }
    return;
}
void s_gotobattery(Point p, Data** d){
    if(final_order.back().r==r_row&&final_order.back().c==r_col) return;
    p = final_order[final_order.size()-2];
    final_order.push_back(p);
    while(d[p.r][p.c].min_dist>0){
        if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r-1][p.c].final_clean)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r][p.c-1].final_clean)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r+1][p.c].final_clean)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r][p.c+1].final_clean)
            p.c++;
        else if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist)
            p.c++;
        d[p.r][p.c].final_clean = true;
        final_order.push_back(p);
    }
    return;
}

int getback(Point p, Data** d){
    int r = p.r, c = p.c;
    stack<Point> s;
    s.push(p);
    while(d[p.r][p.c].min_dist>1){
        if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r-1][p.c].final_clean)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r][p.c-1].final_clean)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r+1][p.c].final_clean)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist&&!d[p.r][p.c+1].final_clean)
            p.c++;
        else if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist)
            p.c++;
        s.push(p);
    }

    while(!s.empty()){
        Point cur = s.top();
        d[cur.r][cur.c].final_clean = true;
        final_order.push_back(cur);
        s.pop();
    }

    return life - d[r][c].min_dist;
}

void from_a_to_b(int s_d, int d_d, Data** mat){
    Point from(r_row,r_col), to(r_row,r_col);
    if(s_d==UP) from.r--;
    else if(s_d==RIGHT) from.c++;
    else if(s_d==DOWN)  from.r++;
    else from.c--;

    if(d_d==RIGHT)  to.c++;
    else if(d_d==DOWN)  to.r++;
    else to.c--;

    pathfinder(from,to,mat);
}

int pathfinder(Point s, Point d, Data** mat){
    bfs_Data **pmat;
    pmat = new bfs_Data* [total_row];
    for(int i=0; i<total_row; i++)
        pmat[i] = new bfs_Data [total_col];
    for(int i=0; i<total_row; i++)
        for(int j=0; j<total_col; j++)
            pmat[i][j].clean = toclean[i][j];
    mat[r_row][r_col].available = false;
    bfs(pmat,mat,s.r,s.c);
    mat[r_row][r_col].available = true;

    stack<Point> stack;
    stack.push(d);
    while(pmat[d.r][d.c].min_dist>0){
        if(d.r>0&&pmat[d.r-1][d.c].min_dist+1==pmat[d.r][d.c].min_dist)
            d.r--;
        else if(d.c>0&&pmat[d.r][d.c-1].min_dist+1==pmat[d.r][d.c].min_dist)
            d.c--;
        else if(d.r+1<total_row&&pmat[d.r+1][d.c].min_dist+1==pmat[d.r][d.c].min_dist)
            d.r++;
        else if(d.c+1<total_col&&pmat[d.r][d.c+1].min_dist+1==pmat[d.r][d.c].min_dist)
            d.c++;
        stack.push(d);
    }
    int bt=0;
    while(!stack.empty()){
        Point cur = stack.top();
        final_order.push_back(cur);
        stack.pop();
        bt++;
    }
    return bt;
}