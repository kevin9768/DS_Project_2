#include<iostream>
#include<fstream>
#include<cstring>
#include<vector>
#include<stack>
#include<cstdlib>
#include<cmath>

using namespace std;

struct point{
    int x,y;
    point(int a,int b):x(a),y(b){}
};

bool **available;
bool **clean;
bool **final_clean;
int  **mindistfrominit;
int row, col;
int life;
int r_row, r_col;
int init_row, init_col;
vector<point> order;
vector<point> last_order;


void dfs(int r, int c, int cost);
void fixmin();
inline bool isAdj(point a, point b);
void gotobattery(int x, int y);
int getback(int x, int y);

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
    clean  = new bool* [row];
    mindistfrominit = new int* [row];
    final_clean = new bool* [row];
    for(int i = 0; i < row; i++){
        available[i]  = new bool [col];
        clean[i]  = new bool [col];
        mindistfrominit[i] = new int [col];
        final_clean[i] = new bool [col];
    }

    //setup 2d-arrays
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++){
            clean[i][j]  = false;
            final_clean[i][j] = false;
            mindistfrominit[i][j] = 3000;
            char c;
            inFile >> c;
            if(c == '1')
                available[i][j] = false;  
            else{
                available[i][j] = true;
                if(c == 'R'){
                    r_row = i, r_col = j;
                    clean[i][j]  = true;
                    final_clean[i][j] = false;
                    available[i][j] = false;
                }
            }
        }
    
    //find out the order to go through these point

    init_row=r_row, init_col=r_col;
    //determine which point to go out through starting point
    if(     init_col<col-1  &&  available[init_row][init_col+1])      init_col++;
    else if(init_row<row-1  &&  available[init_row+1][init_col])      init_row++;
    else if(init_col>0      &&  available[init_row][init_col-1])      init_col--;
    else if(init_row>0      &&  available[init_row-1][init_col])      init_row--;

    mindistfrominit[init_row][init_col]=0;
    dfs(init_row, init_col, 0);
    fixmin();

    for(int i=0;i<row;i++){           //function for seeing if minimum distance is working
        for(int j=0;j<col;j++){
            cout<<mindistfrominit[i][j]<<' ';
        }
        cout<<'\n';
    }


    //run through order
    cout<<order[0].x<<' '<<order[0].y<<'\n';
    last_order.push_back(order[0]);
    final_clean[order[0].x][order[0].y]=true;
    int bt=life;
    bt--;
    int fix=0;
    for(int i=1;i<order.size();i++){
        if(final_clean[order[i].x][order[i].y]){
            vector<point>::iterator iter = order.begin()+i;
            order.erase(iter);
            i--;
            continue;
        }

        if(isAdj(order[i-1-fix],order[i])){
            int dist = mindistfrominit[order[i].x][order[i].y];
            if(bt-1>=dist+1){
                bt--;
                cout<<order[i].x<<' '<<order[i].y<<'\n';
                last_order.push_back(order[i]);
                final_clean[order[i].x][order[i].y]=true;
            }
            else{
                gotobattery(order[i-1-fix].x, order[i-1-fix].y);
                bt = getback(order[i].x, order[i].y);
            }
            fix=0;
        }
        else{
            gotobattery(order[i-1-fix].x, order[i-1-fix].y);
            bt = getback(order[i].x, order[i].y);
            fix=0;
        }
    }
    gotobattery(order.back().x,order.back().y);



    //output
    char outftmp[15] = "/final.path";
    char outf[30];
    strcpy(outf,argv[1]);
    strcat(outf,outftmp);
    ofstream outFile;
    outFile.open(outf);
    
    outFile<<last_order.size()<<'\n';
    for(int i=0; i<last_order.size(); i++){
        outFile<<last_order[i].x<<' '<<last_order[i].y<<'\n';
    }
}

void dfs(int r, int c, int cost){
    clean[r][c]=true;
    if(!available[r][c]) return;
    if(cost<mindistfrominit[r][c]) mindistfrominit[r][c]=cost;
    cout<<r<<' '<<c<<'\n';
    point p(r,c);
    order.push_back(p);
    if(r>0&&!clean[r-1][c])           dfs(r-1,c,cost+1);
    if(c>0&&!clean[r][c-1])           dfs(r,c-1,cost+1);
    if(r<row-1&&!clean[r+1][c])       dfs(r+1,c,cost+1);
    if(c<col-1&&!clean[r][c+1])       dfs(r,c+1,cost+1);
}

void fixmin(){
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            int min = mindistfrominit[i][j];
            if(!available[i][j])    continue;
            if(i>0&&min>mindistfrominit[i-1][j]+1)    min = mindistfrominit[i-1][j]+1;
            if(j>0&&min>mindistfrominit[i][j-1]+1)    min = mindistfrominit[i][j-1]+1;
            if(i<row-1&&min>mindistfrominit[i+1][j]+1)  min = mindistfrominit[i+1][j]+1;
            if(j<col-1&&min>mindistfrominit[i][j+1]+1)  min = mindistfrominit[i][j+1]+1;
            mindistfrominit[i][j]=min;
        }
    }
    for(int i=row-1;i>=0;i--){
        for(int j=col-1;j>=0;j--){
            int min = mindistfrominit[i][j];
            if(!available[i][j])    continue;
            if(i>0&&min>mindistfrominit[i-1][j]+1)    min = mindistfrominit[i-1][j]+1;
            if(j>0&&min>mindistfrominit[i][j-1]+1)    min = mindistfrominit[i][j-1]+1;
            if(i<row-1&&min>mindistfrominit[i+1][j]+1)  min = mindistfrominit[i+1][j]+1;
            if(j<col-1&&min>mindistfrominit[i][j+1]+1)  min = mindistfrominit[i][j+1]+1;
            mindistfrominit[i][j]=min;
        }
    }
}

inline bool isAdj(point a, point b){
    if(abs(a.x-b.x)+abs(a.y-b.y)==1)    return true;
    return false;
}

void gotobattery(int x, int y){
    while(mindistfrominit[x][y]>0){
        if(x>0&&mindistfrominit[x-1][y]+1==mindistfrominit[x][y])              x--;
        else if(y>0&&mindistfrominit[x][y-1]+1==mindistfrominit[x][y])         y--;
        else if(x<row-1&&mindistfrominit[x+1][y]+1==mindistfrominit[x][y])     x++;
        else if(y<col-1&&mindistfrominit[x][y+1]+1==mindistfrominit[x][y])     y++;
        final_clean[x][y]=true;
        cout<<x<<' '<<y<<'\n';
        last_order.push_back(point(x,y));
    }
    cout<<r_row<<' '<<r_col<<'\n';
    last_order.push_back(point(r_row,r_col));

    
    return;
}

int getback(int x, int y){
    int tox=x,toy=x;
    stack<point> s;
    s.push(point(x,y));
    while(mindistfrominit[x][y]>0){
        if(x>0&&mindistfrominit[x-1][y]+1==mindistfrominit[x][y])              x--;
        else if(y>0&&mindistfrominit[x][y-1]+1==mindistfrominit[x][y])         y--;
        else if(x<row-1&&mindistfrominit[x+1][y]+1==mindistfrominit[x][y])     x++;
        else if(y<col-1&&mindistfrominit[x][y+1]+1==mindistfrominit[x][y])     y++;
        s.push(point(x,y));
    }
    
    while(!s.empty()){
        point cur = s.top();
        final_clean[x][y]=true;
        cout<<cur.x<<' '<<cur.y<<'\n';
        last_order.push_back(cur);
        s.pop();
    }

    return life-mindistfrominit[tox][toy]-1;
    
}