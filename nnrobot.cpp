#include<fstream>
#include<iostream>
#include<cstring>
#include<string>
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
    short dir_clean = -1;
    bool final_clean;
    int min_dist = 1000000;
};

struct bfs_Data{
    bool clean;
    int min_dist = 1000000;
};

int sum = 0;
int back_r,back_c;
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
ofstream tmp;

void open_data(ifstream &floor, char* id){
    char floor_data[30];
    strcpy(floor_data, id);
    strcat(floor_data, "/floor.data");
    floor.open(floor_data);
}

void bfs(bfs_Data **&dir, Data **mat, int r, int c);
void dfs(bfs_Data **&dir, Data **mat, int r, int c, int d);
inline bool isAdj(Point a, Point b);
void gotobattery(Point p, Data **mat, int dir);
int getback(Point p, Data** mat, int dir);
void from_a_to_b(int a, int b, Data** mat);
int pathfinder(Point a, Point b, Data** mat, int s_dir);
int dist(Point a, int dir);

int findlowest(int a, int b, int c, int d)
{
    int of_a_b = a <= b ? a : b;
    int of_c_d = c <= d ? c : d;
    int min = of_a_b <= of_c_d ? of_a_b : of_c_d;
    if(min==a)  return 0;
    if(min==b)  return 1;
    if(min==c)  return 2;
    if(min==d)  return 3;
}

int main(int argc, char* argv[]){
    ifstream inFile;
    open_data(inFile, argv[1]);
    inFile >> total_row >> total_col >> life;
    
    //open tmp file for storing pos
    char oftmp[15] = "/tmp.path";
    char of[30];
    strcpy(of,argv[1]);
    strcat(of,oftmp);
    tmp.open(of);

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

    inFile.close();

    
    
    
    
    for(int i=0; i<4; i++){
        int init_row = r_row, init_col = r_col;
        switch(i){
            case UP:
                if(total_row>0 && mat[init_row-1][init_col].available){
                    init_row--;
                    bfs(Up, mat, init_row, init_col);
                }
                break;
            case RIGHT:
                if(init_col<total_col-1 && mat[init_row][init_col+1].available){
                    init_col++;
                    bfs(Right, mat, init_row, init_col);
                }
                break;
            case DOWN:
                if(init_row<total_row-1 && mat[init_row+1][init_col].available){
                    init_row++;
                    bfs(Down, mat, init_row, init_col);
                }
                break;
            case LEFT:
                if(total_col>0 && mat[init_row][init_col-1].available){
                    init_col--;
                    bfs(Left, mat, init_row, init_col);
                }
                break;            
        }
    }

    //delete toclean
    for(int i=0; i<total_row; i++)
        delete[] toclean[i];
    delete[] toclean;

    for(int i=0; i<total_row; i++){
        for(int j=0; j<total_col; j++){
            int up, right, down, left;

            up    = (Up    == NULL ? 1000000:Up[i][j].min_dist);
            right = (Right == NULL ? 1000000:Right[i][j].min_dist);
            down  = (Down  == NULL ? 1000000:Down[i][j].min_dist);
            left  = (Left  == NULL ? 1000000:Left[i][j].min_dist);
            if(up==1000000&&right==1000000&&down==1000000&&left==1000000){
                //cout<<"x ";
                continue;
            }
            mat[i][j].dir_clean = findlowest(up,right,down,left); 
            //cout<<mat[i][j].dir_clean<<' ';
        }
        //cout<<'\n';
    }
    
    //determine the order to clean/
    for(int i=0; i<4; i++){
        int init_row = r_row, init_col = r_col;
        int dirsize = order.size();
        mat[r_row][r_col].available = false;
        switch(i){
            case UP:
                if(total_row>0 && mat[init_row-1][init_col].available){
                    init_row--;
                    dfs(Up, mat, init_row, init_col, i);                    
                }
                break;
            case RIGHT:
                if(init_col<total_col-1 && mat[init_row][init_col+1].available){
                    init_col++;
                    dfs(Right, mat, init_row, init_col, i);                   
                }
                break;
            case DOWN:
                if(init_row<total_row-1 && mat[init_row+1][init_col].available){
                    init_row++;
                    dfs(Down, mat, init_row, init_col, i);                  
                }
                break;
            case LEFT:
                if(total_col>0 && mat[init_row][init_col-1].available){
                    init_col--;
                    dfs(Left, mat, init_row, init_col, i);
                }
                break;            
        }
        mat[r_row][r_col].available = true;
        size_dir[i] = order.size();
        //cout<<size_dir[i]<<' ';
        //if(i==3)    cout<<'\n';
    }
    
    
    /*vector<bfs_Data**> each_dir = {Up, Right, Down, Left};

    for(int i=0;i<4;i++){
        if(each_dir[i]==NULL)   break;
        for(int j=0;j<total_row;j++){
            for(int k=0;k<total_col;k++){
                if(each_dir[i][j][k].min_dist==1000000) cout<<"x ";
                else cout<<each_dir[i][j][k].min_dist<<' ';
            }
            cout<<'\n';
        }
        cout<<"-----------------------------------\n";
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
    }*/

    //find final order
    tmp<<order[0].r<<' '<<order[0].c<<'\n';
    back_r=order[0].r, back_c=order[0].c;
    sum++;
    int fix=1, s=0, bt=life;
    while(size_dir[s]==0)   s++;
    bt--;
    for(int i=1; i<order.size(); i++){
        /*if(mat[order[i].r][order[i].c].final_clean){
            fix++;
            continue;
        }
        else    fix=1;*/

        if(i==size_dir[s]){
            int origin_dir = s;
            s++;
            while(size_dir[s]==size_dir[s-1])   s++;
            int dest_dir = s;
            
            gotobattery(order[i-fix], mat, origin_dir);
            from_a_to_b(origin_dir, dest_dir, mat);
            //final_order.push_back(Point(r_row,r_col));
            tmp<<r_row<<' '<<r_col<<'\n';
            back_r=r_row, back_c=r_col;
            sum++;
            bt = getback(order[i], mat, dest_dir);

            continue;
        }

        if(isAdj(order[i-fix],order[i])){
            if(bt-1>=dist(order[i],s)){
                bt--;
                //final_order.push_back(order[i]);
                tmp<<order[i].r<<' '<<order[i].c<<'\n';
                back_r=order[i].r, back_c=order[i].c;
                sum++;
                mat[order[i].r][order[i].c].final_clean = true;
            }
            else{
                gotobattery(order[i-fix], mat, s);
                if(i==order.size()) break;
                bt = getback(order[i],mat, s);
            }
        }
        else{
            gotobattery(order[i-fix], mat, s);
            if(i==order.size()) break;
            bt = getback(order[i],mat, s);
        }
    }
    gotobattery(order[order.size()-1], mat, s);
    
    
    
    
    
    
    tmp.close();

    //delete 4 matrix;
    for(int i=0; i<total_row; i++){
        delete[] Up[i];
        delete[] Down[i];
        delete[] Right[i];
        delete[] Left[i];
    }
    delete[] Up;
    delete[] Down;
    delete[] Right;
    delete[] Left;

    //
    ifstream intmp;
    intmp.open(of);

    //output
    char outftmp[15] = "/final.path";
    char outf[30];
    strcpy(outf,argv[1]);
    strcat(outf,outftmp);
    ofstream outFile;
    outFile.open(outf);
    
    outFile<<sum<<'\n';
    
    string line;
    while(getline(intmp, line)){
        outFile<<line<<'\n';
    }
    intmp.close();
    outFile.close();
    remove(of);
    
    
}

void bfs(bfs_Data **&d, Data **mat, int r_r, int r_c){
    
    d = new bfs_Data* [total_row];
    for(int i=0; i<total_row; i++)
        d[i] = new bfs_Data [total_col];
    for(int i=0; i<total_row; i++)
        for(int j=0; j<total_col; j++)
            d[i][j].clean = toclean[i][j];
    
    d[r_row][r_col].clean = true;
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
    
    mat[r_row][r_col].available = true;
}

void dfs(bfs_Data **&dir, Data **mat, int r, int c, int d){
    if((dir[r][c].min_dist+1)*2>life||mat[r][c].dir_clean!=d)   return;
    dir[r][c].clean = true;
    if(!mat[r][c].available)  return;
    Point p(r,c);
    order.push_back(p);
    if(r>0 && !dir[r-1][c].clean && dir[r-1][c].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r-1,c,d);
    if(c>0 && !dir[r][c-1].clean && dir[r][c-1].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r,c-1,d);
    if(r+1<total_row && !dir[r+1][c].clean && dir[r+1][c].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r+1,c,d);
    if(c+1<total_col && !dir[r][c+1].clean && dir[r][c+1].min_dist==dir[r][c].min_dist+1)
        dfs(dir,mat,r,c+1,d);
}

inline bool isAdj(Point a, Point b){
    return abs(a.r-b.r)+abs(a.c-b.c)==1 ? true : false;
}

void gotobattery(Point p, Data **mat, int dir){
    if(back_r==r_row&&back_c==r_col) return;
    
    bfs_Data **d;
    switch(dir){
        case UP:
            d = Up;
            break;
        case RIGHT:
            d = Right;
            break;
        case DOWN:
            d = Down;
            break;
        case LEFT:
            d = Left;
            break;
    }

    while(d[p.r][p.c].min_dist>0){
        if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r-1][p.c].final_clean)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r][p.c-1].final_clean)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r+1][p.c].final_clean)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r][p.c+1].final_clean)
            p.c++;
        else if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist)
            p.c++;
        mat[p.r][p.c].final_clean = true;
        //final_order.push_back(p);
        tmp<<p.r<<' '<<p.c<<'\n';
        back_r=p.r, back_c=p.c;
        sum++;
    }
    //final_order.push_back(Point(r_row,r_col));
    tmp<<r_row<<' '<<r_col<<'\n';
    back_r=r_row, back_c=r_col;
    sum++;
    return;
}

int getback(Point p, Data** mat, int dir){
    
    bfs_Data **d;
    switch(dir){
        case UP:
            d = Up;
            break;
        case RIGHT:
            d = Right;
            break;
        case DOWN:
            d = Down;
            break;
        case LEFT:
            d = Left;
            break;
    }

    int r = p.r, c = p.c;
    stack<Point> s;
    s.push(p);
    while(d[p.r][p.c].min_dist>0){
        if(p.r>0&&d[p.r-1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r-1][p.c].final_clean)
            p.r--;
        else if(p.c>0&&d[p.r][p.c-1].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r][p.c-1].final_clean)
            p.c--;
        else if(p.r+1<total_row&&d[p.r+1][p.c].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r+1][p.c].final_clean)
            p.r++;
        else if(p.c+1<total_col&&d[p.r][p.c+1].min_dist+1==d[p.r][p.c].min_dist&&!mat[p.r][p.c+1].final_clean)
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
        mat[cur.r][cur.c].final_clean = true;
        //final_order.push_back(cur);
        tmp<<cur.r<<' '<<cur.c<<'\n';
        back_r=cur.r, back_c=cur.c;
        sum++;
        s.pop();
    }

    return life - d[r][c].min_dist - 1;
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

    pathfinder(from,to,mat,s_d);
}

int pathfinder(Point s, Point d, Data** mat, int s_dir){
    bfs_Data **pmat;
    
    switch(s_dir){
        case UP:
            pmat = Up;
            break;
        case RIGHT:
            pmat = Right;
            break;
        case DOWN:
            pmat = Down;
            break;
        case LEFT:
            pmat = Left;
            break;
    }

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
        //final_order.push_back(cur);
        tmp<<cur.r<<' '<<cur.c<<'\n';
        back_r=cur.r, back_c=cur.c;
        sum++;
        stack.pop();
        bt++;
    }
    return bt;
}

int dist(Point a, int dir){
    bfs_Data **d;
    switch(dir){
        case UP:
            d = Up;
            break;
        case RIGHT:
            d = Right;
            break;
        case DOWN:
            d = Down;
            break;
        case LEFT:
            d = Left;
            break;
    }
    return d[a.r][a.c].min_dist+1;
}