#include<iostream>
#include<fstream>
#include<cstring>
#include<vector>
#include<stack>
#include<queue>
#include<cstdlib>

using namespace std;

struct point{
    int x,y;
    point(int a,int b):x(a),y(b){}
    point():x(0),y(0){}
};

bool **available;
bool **init_clean;
bool **clean;
bool **final_clean;
int  **mindistfrominit;
int row, col;
int life;
int r_row, r_col;
int init_row, init_col;
vector<point> order;
vector<point> last_order;


void dfs(int r, int c);
void bfs(int r, int c);
void clean_data();
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
    init_clean  = new bool* [row];
    clean = new bool* [row];
    mindistfrominit = new int* [row];
    final_clean = new bool* [row];
    for(int i = 0; i < row; i++){
        available[i]  = new bool [col];
        init_clean[i]  = new bool [col];
        clean[i] = new bool [col];
        mindistfrominit[i] = new int [col];
        final_clean[i] = new bool [col];
    }

    //setup 2d-arrays
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++){
            init_clean[i][j]  = false;
            final_clean[i][j] = false;
            mindistfrominit[i][j] = 1000000;
            char c;
            inFile >> c;
            if(c == '1'){
                available[i][j] = false;  
                init_clean[i][j]  = true;
            }
            else{
                available[i][j] = true;
                if(c == 'R'){
                    r_row = i, r_col = j;
                    init_clean[i][j]  = true;
                    final_clean[i][j] = false;
                    available[i][j] = false;
                }
            }
        }
    
    //find out the order to go through these point
    int dir=0;
    while(1){
        bool cool = true;
        init_row=r_row, init_col=r_col;
        //determine which point to go out through starting point
        if(     init_col<col-1  &&  available[init_row][init_col+1] && dir<1)      init_col++,dir=1;
        else if(init_row<row-1  &&  available[init_row+1][init_col] && dir<2)      init_row++,dir=2;
        else if(init_col>0      &&  available[init_row][init_col-1] && dir<3)      init_col--,dir=3;
        else if(init_row>0      &&  available[init_row-1][init_col] && dir<4)      init_row--,dir=4;
        clean_data();
        bfs(init_row, init_col);
        

        for(int i=0; i<row; i++)
            for(int j=0; j<col; j++)
                if((mindistfrominit[i][j]+1)*2>life&&available[i][j])
                    cool = false;
        
        if(cool==true) break;

        for(int i=0; i<row; i++)
            for(int j=0; j<col; j++)
                mindistfrominit[i][j]=1000000;
    }
    //cout<<"1\n";
    clean_data();
    dfs(init_row, init_col);
    

    for(int i=0;i<row;i++){           //function for seeing if minimum distance is working
        for(int j=0;j<col;j++){
            cout<<mindistfrominit[i][j]<<' ';
        }
        cout<<'\n';
    }


    //run through order
    //cout<< order[0].x<<' '<<order[0].y<<'\n';
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
                //cout<<order[i].x<<' '<<order[i].y<<'\n';
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

void dfs(int r, int c){
    clean[r][c]=true;
    if(!available[r][c]) return;
    //cout<<r<<' '<<c<<'\n';
    point p(r,c);
    order.push_back(p);
    if(r>0&&!clean[r-1][c])           dfs(r-1,c);
    if(c>0&&!clean[r][c-1])           dfs(r,c-1);
    if(r<row-1&&!clean[r+1][c])       dfs(r+1,c);
    if(c<col-1&&!clean[r][c+1])       dfs(r,c+1);
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
        //cout<<x<<' '<<y<<'\n';
        last_order.push_back(point(x,y));
    }
    //cout<<r_row<<' '<<r_col<<'\n';
    last_order.push_back(point(r_row,r_col));

    
    return;
}

int getback(int x, int y){
    int tox=x,toy=y;
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
        //cout<<cur.x<<' '<<cur.y<<'\n';
        last_order.push_back(cur);
        s.pop();
    }

    return life-mindistfrominit[tox][toy]-1;
    
}

void bfs(int s_r,int s_c){ 
    queue <point> q;
    point u,upos,in;
    u.x=s_r;
    u.y=s_c;
    mindistfrominit[u.x][u.y]=0;
    clean[u.x][u.y]=true;
 
    q.push(u);
    while(!q.empty()){ 
        upos=q.front();
        //cout<<upos.x<<" "<<upos.y<<endl;
        q.pop();
        if(upos.x>0&&available[upos.x-1][upos.y]&&!clean[upos.x-1][upos.y]){
            in.x=upos.x-1;
            in.y=upos.y;
            mindistfrominit[upos.x-1][upos.y]=mindistfrominit[upos.x][upos.y]+1;
            q.push(in);
            clean[in.x][in.y]=true;
            
        }
 
        if(upos.x+1<row&&available[upos.x+1][upos.y]&&!clean[upos.x+1][upos.y]){
            in.x=upos.x+1;
            in.y=upos.y;
            mindistfrominit[upos.x+1][upos.y]=mindistfrominit[upos.x][upos.y]+1;
            q.push(in);
            clean[in.x][in.y]=true;
        }
 
        if(upos.y>0&&available[upos.x][upos.y-1]&&!clean[upos.x][upos.y-1]){
            in.x=upos.x;
            in.y=upos.y-1;
            mindistfrominit[upos.x][upos.y-1]=mindistfrominit[upos.x][upos.y]+1;
            q.push(in);
            clean[in.x][in.y]=true; 
        }
 
        if(upos.y+1<col&&available[upos.x][upos.y+1]&&!clean[upos.x][upos.y+1]){
            in.x=upos.x;
            in.y=upos.y+1;
            mindistfrominit[upos.x][upos.y+1]=mindistfrominit[upos.x][upos.y]+1;
            q.push(in); 
            clean[in.x][in.y]=true;
        }
 
    }
 
    //printf("%d\n",mindistfrominit[d_r][d_c]);
 
}

void clean_data(){
    for(int i=0; i<row; i++)
        for(int j=0; j<col; j++)
            clean[i][j] = init_clean[i][j];
}