 #include<iostream>
using namespace std;

struct p{
        int id;
        int x,y;
        int deadx,deady;
        int * direct;
        int arr[60];
        int moved;
};

class board{

    public:

    struct p wk,wq,wr1,wr2,wh1,wh2,wb1,wb2,w1,w2,w3,w4,w5,w6,w7,w8;
    struct p bk,bq,br1,br2,bh1,bh2,bb1,bb2,b1,b2,b3,b4,b5,b6,b7,b8;

    struct p * p1[16] = {&w1,&w2,&w3,&w4,&w5,&w6,&w7,&w8,&wk,&wq,&wr1,&wr2,&wb1,&wb2,&wh1,&wh2};
    struct p * p2[16] = {&b1,&b2,&b3,&b4,&b5,&b6,&b7,&b8,&bk,&bq,&br1,&br2,&bb1,&bb2,&bh1,&bh2};

    int box[10][16] = {
        //1 2  3  4  5   6  7  8  9  10 11 12 13 14
        {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2},
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2}, //a
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2}, //b
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2}, //c
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2}, //d
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2}, //e
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2}, //f
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2}, //g
        {-2,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-2},  //h
        {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2}
    };

    struct p * check[5] = {NULL,NULL,NULL,NULL,NULL};
    int checkto=0;
    int availpiece[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int available[30]={-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2};
    int check1=0, check2=0;

    int castle(struct p * h){
        int c ,i,j,initialize;
        string t;
        struct p * r= NULL;
        struct p * a[4] = {&wr1,&wr2,&br1,&br2};
        if(h->id/10 == 1) c=check1 , initialize=0 ;
        else c=check2 , initialize=2;

        cout<<"enter rook position : ";
        cin>>t;
        i = (int)t[0] - 96;  //abcdefgh
        j = (int)t[1] - 48;  //12345678

        if(a[initialize]->x == i && a[initialize]->y == j) r = a[initialize];
        else if (a[initialize + 1]->x == i && a[initialize +1]->y == j) r = a[initialize+1] ;
        
        if( r == NULL){
            cout<<"INVALID INPUT"<<endl;
            return 1;
        }
        if(c == 0 && h->moved == 0 && r->moved == 0 ){
            int diff = h->y - r->y;
            if(diff>0 && safe(h->id/10 ,h->x,h->y - 2)){
                for(int i= r->y + 1 ; i < h->y ; i++){
                    if(box[h->x][h->y + i] != 0){
                        return 1;
                    }
                }
                box[h->x][h->y] = 0;
                box[r->x][r->y] = 0;
                box[h->x][h->y - 2] = h->id;
                h->y -= 2;
                box[h->x][h->y +1] = r->id;
                r->y +=1;
                h->moved = 1;
                r->moved=1;
                return 0;
            }
            else if(diff<0 && safe(h->id/10 ,h->x,h->y + 2)){
                for(int i= h->y + 1 ; i < r->y ; i++){
                    if(box[h->x][h->y + i] != 0){
                        return 1;
                    }
                }
                box[h->x][h->y] = 0;
                box[r->x][r->y] = 0;
                box[h->x][h->y + 2] = h->id;
                h->y += 2;
                box[h->x][h->y -1] = r->id;
                r->y -=1 ;
                h->moved = 1;
                r->moved=1;
                return 0;
            }          
        }
        return 1;     
    }

    void clear(){
        for(int i=0; i<30; i++ ){
            available[i] = -2;
        }
        for(int i=0; i<5; i++){
            check[i] = NULL;
        }
        for(int i=0; i<16; i++){
            availpiece[i] = 1;
        }
        checkto=0;
    }
    void put(int a, int b){
        int i =0;
        while(available[i] != -2){
            if(available[i] == a && available[i+1] == b){
                return;
            }
            i+=2;
        }
        available[i] = a;
        available[++i] = b;
    }
    void aftercheck(struct p * n){
        int enx,eny,chkx,chky,diffx,diffy;
        for(int i=0; i<5; i++){
            if(check[i] == NULL) break;
            enx = check[i]->x , eny = check[i]->y , chkx =n->x , chky = n->y , diffx = n->x - check[i]->x , diffy = n->y - check[i]->y ;
            if(check[i]->id%10 == 1 || check[i]->id%10 == 3) {
                put(enx , eny);
            }
            else if(check[i]->id%10 == 2 || check[i]->id%10 == 4 || check[i]->id%10 == 5 ){
                int dx=0 , dy=0;
                for(int i=1; i<8; i++){
                    if(diffx > 0) dx--;
                    else if(diffx < 0) dx++;

                    if(diffy > 0) dy--;
                    else if(diffy < 0) dy++;
                    put(chkx + dx , chky + dy);

                    if(((chkx + dx) == enx) && ((chky + dy) == eny)) break;
                }
            }
        }

    }
    void render(){
        if(checkto == 1) check1 = 1;
        else if(checkto ==2) check2 = 1;
        queen(p1[9]);
        queen(p2[9]);
        for(int i=0; i<16; i++){
            if(i==9) continue;
            switch(p1[i]->id%10){
                case 1:
                    pawn(p1[i]);
                    break;
                case 2:
                    bishop(p1[i]);
                    break;
                case 3:
                    horse(p1[i]);
                    break;
                case 4:
                    rook(p1[i]);
                    break;
                case 5:
                    king(p1[i]);
                    break;
            }
            switch(p2[i]->id%10){
                case 1:
                    pawn(p2[i]);
                    break;
                case 2:
                    bishop(p2[i]);
                    break;
                case 3:
                    horse(p2[i]);
                    break;
                case 4:
                    rook(p2[i]);
                    break;
                case 5:
                    king(p2[i]);
                    break;
            }
        }

        struct p * h;
        if(checkto !=0 ){
            (checkto == 1) ? h = p1[9] : h = p2[9];
            aftercheck(h);
        }
    }

    void pawn(struct p * n){
        if(n->id/10 == 1){
            if(box[n->x+1][n->y] == 0) {
                n->arr[0] = n->x+1;
                n->arr[1] = n->y;
            }
            else{
                n->arr[0] = -1;
                n->arr[1] = -1;
            }
            if(box[n->x+2][n->y] == 0 && n->x == 2){
                n->arr[2] = n->x+2;
                n->arr[3] = n->y;
            }
            else{
                n->arr[2] = -1;
                n->arr[3] = -1;
            }
            if(box[n->x+1][n->y-1]/10 > 0 ) {           //enemy
                n->arr[4] = n->x+1;
                n->arr[5] = n->y-1;

                if(box[n->x+1][n->y-1]/10 == 2 && box[n->x+1][n->y-1]%10 == 6){         //checking king
                    append(n);
                    checkto = 2;
                }
            }
            else{
                n->arr[4] = -1;
                n->arr[5] = -1;
            }
            if(box[n->x+1][n->y+1]/10 != 0 && box[n->x+1][n->y+1]/10 != -2 ) {
                n->arr[6] = n->x+1;
                n->arr[7] = n->y+1;

                if(box[n->x+1][n->y+1]/10 == 2 && box[n->x+1][n->y+1]%10 == 6){         //checking king
                    append(n);
                    checkto = 2;
                }
            }
            else{
                n->arr[6] = -1;
                n->arr[7] = -1;
            }
        }
        if(n->id/10 == 2){
            if(box[n->x-1][n->y] == 0){
                n->arr[0] = n->x-1;
                n->arr[1] = n->y;
            }
            else{
                n->arr[0] = -1;
                n->arr[1] = -1;
            }
            if(box[n->x-2][n->y] == 0 && n->x == 7){
                n->arr[2] = n->x-2;
                n->arr[3] = n->y;
            }
            else{
                n->arr[2] = -1;
                n->arr[3] = -1;
            }
            if(box[n->x-1][n->y+1]/10 != 0 && box[n->x-1][n->y+1]/10 != -2 ) {          //enemy
                n->arr[4] = n->x-1;
                n->arr[5] = n->y+1;

                if(box[n->x-1][n->y+1]/10 == 1 && box[n->x-1][n->y+1]%10 == 6){         //checking king
                    append(n);
                    checkto = 1;
                }
            }
            else{
                n->arr[4] = -1;
                n->arr[5] = -1;
            }
            if(box[n->x-1][n->y-1] != 0 && box[n->x-1][n->y-1] != -2) {
                n->arr[6] = n->x-1;
                n->arr[7] = n->y-1;

                if(box[n->x-1][n->y-1]/10 == 1 && box[n->x-1][n->y-1]%10 == 6){         //checking king
                    append(n);
                    checkto = 1;
                }
            }
            else{
                n->arr[6] = -1;
                n->arr[7] = -1;
            }
        }

    }

    void horse(struct p * n){
        int a=n->x , b=n->y;
        if( (a+1>=1 && a+1<=8) && (b+2>=1 && b+2<=8) && box[n->x+1][n->y+2] >=0 && box[n->x+1][n->y+2]/10 != n->id/10){
            n->arr[0] =  n->x+1;
            n->arr[1] = n->y+2;
            if(box[n->x+1][n->y+2]%10 == 6){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[0] = -1;
            n->arr[1] = -1;
        }
        if((a+2>=1 && a+2<=8) && (b+1>=1 && b+1<=8) && box[n->x+2][n->y+1] >=0 && box[n->x+2][n->y+1]/10 != n->id/10){
            n->arr[2] =  n->x+2;
            n->arr[3] = n->y+1;
            if(box[n->x+2][n->y+1]%10 == 6){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[2] = -1;
            n->arr[3] = -1;
        }
        if((a-1>=1 && a-1<=8) && (b+2>=1 && b+2<=8) && box[n->x-1][n->y+2] >=0 && box[n->x-1][n->y+2]/10 != n->id/10){
            n->arr[4] =  n->x-1;
            n->arr[5] = n->y+2;
            if(box[n->x-1][n->y+2]%10 == 6){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[4] = -1;
            n->arr[5] = -1;
        }
        if((a-2>=1 && a-2<=8) && (b+1>=1 && b+1<=8) && box[n->x-2][n->y+1] >=0 && box[n->x-2][n->y+1]/10 != n->id/10){
            n->arr[6] =  n->x-2;
            n->arr[7] = n->y+1;
            if(box[n->x-2][n->y+1]%10 == 6){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[6] = -1;
            n->arr[7] = -1;
        }
        if((a+1>=1 && a+1<=8) && (b-2>=1 && b-2<=8) && box[n->x+1][n->y-2] >=0 && box[n->x+1][n->y-2]/10 != n->id/10){
            n->arr[8] =  n->x+1;
            n->arr[9] = n->y-2;
            if(box[n->x+1][n->y-2]%10 == 6 ){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[8] = -1;
            n->arr[9] = -1;
        }
        if((a+2>=1 && a+2<=8) && (b-1>=1 && b-1<=8) && box[n->x+2][n->y-1] >=0 && box[n->x+2][n->y-1]/10 != n->id/10){
            n->arr[10] =  n->x+2;
            n->arr[11] = n->y-1;
            if(box[n->x+2][n->y-1]%10 == 6 ){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[10] = -1;
            n->arr[11] = -1;
        }
        if((a-1>=1 && a-1<=8) && (b-2>=1 && b-2<=8) && box[n->x-1][n->y-2] >=0 && box[n->x-1][n->y-2]/10 != n->id/10){
            n->arr[12] =  n->x-1;
            n->arr[13] = n->y-2;
            if(box[n->x-1][n->y-2]%10 == 6){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[12] = -1;
            n->arr[13] = -1;
        }
        if((a-2>=1 && a-2<=8) && (b-1>=1 && b-1<=8) && box[n->x-2][n->y-1] >=0 && box[n->x-2][n->y-1]/10 != n->id/10){
            n->arr[14] =  n->x-2;
            n->arr[15] = n->y-1;
            if(box[n->x-2][n->y-1]%10 == 6){
                append(n);
                (n->id/10 == 1) ? checkto=2 : checkto=1 ;
            }
        }
        else{
            n->arr[14] = -1;
            n->arr[15] = -1;
        }
    }

    void bishop(struct p * n){
        int a = n->x;
        int b = n->y;
        int k=0;
        for(int i=0 ; i<4 ; i++){
            a=n->x;
            b=n->y;
            for(int j=0; j<8 ; j++){
                if(i==0)a++,b++;
                else if(i==1)a++,b--;
                else if(i==2)a--,b++;
                else a--,b--;

                if(box[a][b] < 0) break;
                else if(box[a][b] >0){
                    if(box[a][b]/10 == n->id/10) break;
                    else{
                        n->arr[k] = a;      //kill
                        n->arr[++k] = b;
                        if(box[a][b]%10 == 6){   //check
                            (n->id/10 == 2)?checkto = 1 : checkto = 2;
                            append(n);
                        }
                        k++;
                        break;
                    }
                }
                else{
                    n->arr[k] = a;
                    n->arr[++k] = b;
                    k++;
                }
            }
        }
    }

    void rook(struct p * n){
        int a = n->x;
        int b = n->y;
        int k =0;
        for(int i=0 ; i<4 ; i++){
            a=n->x;
            b=n->y;
            for(int j=0; j<8 ; j++){
                if(i==0)b++;
                else if(i==1)b--;
                else if(i==2)a++;
                else a--;

                if(box[a][b] < 0) break;
                else if(box[a][b] >0){
                    if(box[a][b]/10 == n->id/10) break;
                    else{
                        n->arr[k] = a;          //kill
                        n->arr[++k] = b;
                        if(box[a][b]%10 == 6){   //check
                            (n->id/10 == 2)?checkto = 1 : checkto = 2;
                            append(n);
                        }
                        k++;
                        break;
                    }
                }
                else{
                    n->arr[k] = a;
                    n->arr[++k] = b;
                    k++;
                }
            }
        }
    }

    void king(struct p * n){
        int a = n->x;
        int b = n->y;
        int k =0;
        for(int i=1 ; i<=8 ; i++){
            a=n->x;
            b=n->y;
            for(int j=0; j<8 ; j++){
                switch(i){
                    case 1:
                        a++;
                        break;
                    case 2:
                        b++;
                        break;
                    case 3:
                        a--;
                        break;
                    case 4:
                        b--;
                        break;
                    case 5:
                        a++,b++;
                        break;
                    case 6:
                        a++,b--;
                        break;
                    case 7:
                        a--,b++;
                        break;
                    case 8:
                        a--,b--;
                        break;
                }

                if(box[a][b] < 0) break;
                else if(box[a][b] >0){
                    if(box[a][b]/10 == n->id/10) break;
                    else{
                        n->arr[k] = a;      //kill
                        n->arr[++k] = b;
                        if(box[a][b]%10 == 6){   //check
                            (n->id/10 == 2)?checkto = 1 : checkto = 2;
                            append(n);
                        }
                        k++;
                        break;
                    }
                }
                else{
                    n->arr[k] = a;
                    n->arr[k+1] = b;
                    k+=2;
                }
            }
        }
    }

    void append(struct p * n){
        int i=0;
        while(check[i] != NULL){
            i++;
        }
        check[i]=n;
    }

    int safe(int player ,int a,int b){

        for(int i=1;i<8;i++){           //king and rook
            if((box[a][b+i] == -2 || box[a][b+i]/10 == player) && box[a][b+i]%10 !=6) break;
            else if((box[a][b+i]%10 == 5)|| (box[a][b+i]%10 == 4)) {return 0;}
        }
        for(int i=1;i<8;i++){
            if(box[a][b-i] == -2 || box[a][b-i]/10 == player && box[a][b-i]%10 !=6) break;
            else if(box[a][b-i]%10 == 5 || box[a][b-i]%10 == 4) return 0;
        }
        for(int i=1;i<8;i++){
            if(box[a+i][b] == -2 || box[a+i][b]/10 == player && box[a+i][b]%10 !=6) break;
            else if(box[a+i][b]%10 == 5 || box[a+i][b]%10 == 4) return 0;
        }
        for(int i=1;i<8;i++){
            if(box[a-i][b] == -2 || box[a-i][b]/10 == player && box[a-i][b]%10 !=6) break;
            else if(box[a-i][b]%10 == 5 || box[a-i][b]%10 == 4) return 0;
        }

        for(int i=1;i<8;i++){               //king and bishop
            if(box[a+i][b+i] == -2 || box[a+i][b+i]/10 == player && box[a+i][b+i]%10 !=6) break;
            else if(box[a+i][b+i]%10 == 5 || box[a+i][b+i]%10 == 2) return 0;
        }
        for(int i=1;i<8;i++){
            if(box[a-i][b-i] == -2 || box[a-i][b-i]/10 == player && box[a-i][b-i]%10 !=6) break;
            else if(box[a-i][b-i]%10 == 5 || box[a-i][b-i]%10 == 2) return 0;
        }
        for(int i=1;i<8;i++){
            if(box[a+i][b-i] == -2 || box[a+i][b-i]/10 == player && box[a+i][b-i]%10 !=6) break;
            else if(box[a+i][b-i]%10 == 5 || box[a+i][b-i]%10 == 2) return 0;
        }
        for(int i=1;i<8;i++){
            if(box[a-i][b+i] == -2 || box[a-i][b+i]/10 == player && box[a-i][b+i]%10 !=6) break;
            else if(box[a-i][b+i]%10 == 5 || box[a-i][b+i]%10 == 2) return 0;
        }

        int bit[16] = {-2,-2,-1,1,2,2,1,-1,-2,-2,-1,1,2,2,1,-1};       //horse alogrithm
        for(int i=0;i<8; i++){
            if((a+bit[i+1]>=1 && a+bit[i+1]<=8) && (b+bit[i+3]>=1 && b+bit[i+3]<=8) && box[a+bit[i+1]][b+bit[i+3]]%10 == 3 && box[a+bit[i+1]][b+bit[i+3]]/10 != player) return 0;
        }

        int p,q;
        for(int i =0; i<9 ; i++){
            if(i%3 == 0)  p=-1;
            else if(i%3 == 1) p=0;
            else p=1;

            if(i/3 == 0) q=-1;
            else if(i/3 == 1) q=0;
            else q=1;

            if(box[a+q][b+q]%10 == 6 && box[a+q][b+q]/10 != player && (a+q>=1 && a+q<=8) && (b+p>=1 && b+p<=8)) return 0;
        }

        //pawn
        if( player == 2){
            if(((box[a-1][b+1] != -2) && (box[a-1][b+1]%10 == 1 && box[a-1][b+1]/10 == 1)) || ((box[a-1][b-1] != -2) && (box[a-1][b-1]%10 == 1 && box[a-1][b-1]/10 == 1))) return 0;
        }
        else{
            if(((box[a+1][b+1] != -2) && (box[a+1][b+1]%10 == 1 && box[a+1][b+1]/10 == 2)) ||((box[a+1][b-1] != -2) && (box[a+1][b-1]%10 == 1 && box[a+1][b-1]/10 == 2))) return 0;
        }

        return 1;
    }

    void queen(struct p * n){
        int a = n->x;
        int b = n->y;
        int p,q,s=0;

        for(int i =0; i<9 ; i++){
            if(i%3 == 0)  p=-1;
            else if(i%3 == 1) p=0;
            else p=1;

            if(i/3 == 0) q=-1;
            else if(i/3 == 1) q=0;
            else q=1;

            if(box[a+q][b+p] >= 0 && box[a+q][b+p]/10 != n->id/10 && safe(n->id/10 ,a+q , b+p)){
                n->arr[s] = a+q;
                n->arr[s+1] = b+p;
            }
            else{
                n->arr[s] = -1;
                n->arr[s+1] = -1;
            }
            s+=2;
        }
    }

    int windraw(int turn){
        if(checkto != 0){
            struct p * a;
            struct p ** h;
            int availsize=0,piecesize=0,z=0,x=0,c=0;
            (checkto == 1) ? h = p1 : h = p2;
            for(int i=0; i<16; i++){                //checking if any other piece have steps
                availpiece[i] = 0;
                if(i==9) continue;
                x=0;
                while(h[i]->arr[x] != -2){
                    c=0;
                    while(available[c] != -2){
                        if(h[i]->arr[x] == available[c] && h[i]->arr[x+1] == available[c+1]) {
                            availpiece[i] = 1;              //toggling if any pieces could move or not move while checked
                            availsize++;
                        }
                        c+=2;
                    }
                    x+=2;
                }
            }
            (checkto == 1) ? a = p1[9] : a = p2[9];
            while(a->arr[z] != -2){                 ////checking if queen piece have steps
                if(a->arr[z] != -1) piecesize++,availpiece[9] = 1;
                z+=2;
            }
            if(availsize + piecesize == 0) return 1; //win  by piece after check , player have no moves
        }
        else{
            struct p ** h;                  
            int z,movs=0;
            (turn == 1) ? h = p1 : h = p2;
            for(int i=0; i<16; i++){                    //iterating pieces
                z=0;
                while(h[i]->arr[z] != -2){              //iterating that particular piece from iterating pieces have any steps
                    if(h[i]->arr[z] != -1){
                        movs = 1;                       //if i get any single steps for any piecees that no need further checking to know if it is draw position or not 
                        break;
                    }
                    z+=2;
                }
                if(movs == 1) break;
            }
            if(movs != 1) return 3;             //draw      if not checked and any pieces dont have any steps
        }
        return 0;
    }

    void display(){
        cout<<"    ";
        for(int i = 0 ; i<8; i++){
            cout<<"  "<<(char)('1'+ i)<<"  ";
        }
        cout<<endl<<"   ";
        for(int j=0 ; j<21 ; j++) cout<<"--";
        cout<<endl;
        for(int i = 0 ; i<9; i++){
            if(i== 0) continue;
            cout<< (char)('a'+ i - 1) <<"  ||";
            for(int j=0 ; j<16 ; j++){
                if(box[i][j] == -1) cout<<" ";  // yaha par hai -1 ke liye "  "
                else if( box[i][j] == -2 ) continue;
                else cout<<" "<<look(i,j)<<" ";
                (j!=8 && j <8) ? cout<<'|' : (j>8) ? cout<<"  " : cout<<"||";
            }
            cout<<endl<<"    ";
            for(int j=0 ; j<21 ; j++) cout<<"--";
            cout<<endl;
        }
    }

    string look(int i, int j){
        char player = '0' + box[i][j]/10;
        char pieces[7] = {' ','P','B','H','R','K','Q'};
        int piece = box[i][j]%10;
        string s = "";
        s += pieces[piece];
        (player == '0') ? s += ' ' : s += player;
        return s;
    }

    void promotion(struct p * n){
        if(n->id/10 == 2 && n->id%10 == 1 && n->x == 1){   //  pawn promotion
                int promote;
                do{
                    if(promote ==2 || promote ==3 || promote ==4 || promote == 5) break;
                    cout<<"enter number to promote to K=5,R=4,H=3,B=2 : ";
                    scanf("%d" , &promote);   
                }
                while(1);
                n->id = 20+promote;
            }
            else if(n->id/10 == 1 && n->id%10 == 1 && n->x == 8){
                int promote;
                do{
                    if(promote ==2 || promote ==3 || promote ==4 || promote == 5) break;
                    cout<<"enter number to promote to K=5,R=4,H=3,B=2 : ";
                    scanf("%d" , &promote);   
                }
                while(1);
                n->id = 10+promote;
            }
    }

    int mov(struct p * n ,string f , string t){

        int i , j , p , q, alright=0;

        i = (int)t[0] - 96;  //abcdefgh
        j = (int)t[1] - 48;  //12345678

        p = (int)f[0] - 96;  //abcdefgh
        q = (int)f[1] - 48;  //12345678

        if(checkto != 0){
            int k=0,l=0;
            if(n->id%10 != 6){
                while(n->arr[k] != -2){
                    if(n->arr[k] == i && n->arr[k+1] == j){
                        while(available[l] != -2){
                            if(n->arr[k] == available[l] && n->arr[k+1] == available[l+1]){
                                alright = 2;
                                break;
                            }
                            l+=2;
                        }  
                    }
                    
                    if(alright == 2) break;
                    k+=2;
                }
            }
            else{
                struct p * a;
                (checkto == 1) ? a = p1[9] : a = p2[9];
                while(a->arr[l] != -2){
                    if( a->arr[l] == i && a->arr[l+1] == j ) {
                        alright = 2;
                        break;
                    }
                    l+=2;
                }
            }
        }
        else{
            int k=0;
            while(n->arr[k] != -2){
                if(n->arr[k] == i && n->arr[k+1] == j) {
                    alright = 2;
                    break;
                }
                k+=2;
            }
        }

        if(alright == 2){
            box[p][q] = 0;
            n->x =  i , n->y = j;

            struct p ** h;
            (n->id/10 == 1) ? h = p2 : h = p1;

            if(box[i][j] >0 ){
                for(int z=0; z<16; z++){
                    if(h[z]->x == i && h[z]->y == j){
                        h[z]->x = h[z]->deadx;
                        h[z]->y = h[z]->deady;
                        box[h[z]->deadx][h[z]->deady] = h[z]->id;
                        break;
                    }
                }
            }

            promotion(n);

            box[i][j] = n->id;

            if(n->moved == 0) n->moved = 1;

            return 0;
        }
        cout<<"ENTER VALID INPUT"<<endl;
        return 1;
    }

    // public:

    int player(int turn){
        render();
        int wd = 0;
        wd = windraw(turn);
        display();
        if(wd == 1) return 1;
        else if(wd == 3) return 3;

        string from,to;

        int p , q;
        int ok=1, r,cast;
        struct p ** h; 

        while(ok){
            cast=1;
            cout<<"enter piece position that move : ";
            cin>>from;
            struct p * a;

            if(from == "castle"){
                int c;
                (turn == 0) ? a=p1[9] : a=p2[9];
                (turn == 0) ? c=check1 : c=check2;
                if(c == 0 && a->moved == 0) cast = castle(a);
                else cout<<"NOT AVAILABLE"<<endl;
                if(cast == 0) break;
                cout<<"NOT POSSIBLE"<<endl;
                continue;
            } 

            p = (int)from[0] - 96;  //abcdefgh
            q = (int)from[1] - 48;  //12345678

            (turn == 0) ? h = p1 : h = p2;
            int v=-1 , ok=1 , ymoves=0;

            for(r = 0; r<16 ; r++){
                if(h[r]->x == p && h[r]->y == q ) {
                    ok = 2;
                    for(int j=0; j<60 ; j++){
                        if(h[r]->arr[j] != -2 ){
                            ymoves = 1;
                            break;
                        }
                    }
                    break;
                }
            }
            if(ok != 2){
                cout<<"ENTER VALID POSITION"<<endl;
                continue;
            } 
            if(ymoves == 0 && checkto == 0){
                cout<<"HAVE NO PLACE TO MOVE"<<endl;
                continue;
            } 
            if(availpiece[r] == 1 ) ok=1;
            else{
                cout<<"CHECK!!!"<<endl;
                continue;
            }

            while(ok && cast){
                cout<<"enter piece position where it move : ";
                cin>>to;
                if(to == "cancel") break;
                ok = mov(h[r] , from , to);
            }
            if(ok == 0) break;
        }
        return 5;
    }

    void piecing(struct p * n,int s , int x, int y){
        n->id = s;
        n->y = y;
        n->x = x;
        n->deadx = x;
        n->deady = y+8;
        n->moved = 0;
        box[x][y] = n->id;
        for(int k=0 ; k<=59 ; k++) {
            n->arr[k] = -2;
        }
        
    }

};


void game(){
    board b;
    b.piecing(&b.w1,11,2,1);
    b.piecing(&b.w2,11,2,2);
    b.piecing(&b.w3,11,2,3);
    b.piecing(&b.w4,11,2,4);
    b.piecing(&b.w5,11,2,5);
    b.piecing(&b.w6,11,2,6);
    b.piecing(&b.w7,11,2,7);
    b.piecing(&b.w8,11,2,8);
    b.piecing(&b.wk,15,1,4);
    b.piecing(&b.wq,16,1,5);
    b.piecing(&b.wr1,14,1,1);
    b.piecing(&b.wr2,14,1,8);
    b.piecing(&b.wh1,13,1,2);
    b.piecing(&b.wh2,13,1,7);
    b.piecing(&b.wb1,12,1,3);
    b.piecing(&b.wb2,12,1,6);

    b.piecing(&b.b1,21,7,1);
    b.piecing(&b.b2,21,7,2);
    b.piecing(&b.b3,21,7,3);
    b.piecing(&b.b4,21,7,4);
    b.piecing(&b.b5,21,7,5);
    b.piecing(&b.b6,21,7,6);
    b.piecing(&b.b7,21,7,7);
    b.piecing(&b.b8,21,7,8);
    b.piecing(&b.bk,25,8,4);
    b.piecing(&b.bq,26,8,5);
    b.piecing(&b.br1,24,8,1);
    b.piecing(&b.br2,24,8,8);
    b.piecing(&b.bh1,23,8,2);
    b.piecing(&b.bh2,23,8,7);
    b.piecing(&b.bb1,22,8,3);
    b.piecing(&b.bb2,22,8,6);

    string p1,p2;
    cout<<"enter Player 1 Name : ";
    cin>>p1;
    cout<<"enter Player 2 Name : ";
    cin>>p2;
    int result=5 , turn = 0 ;

    while(result){
        if(turn == 0){
            cout<<"\t\t\t\t\t\t\t"<<p1<<" Turns - 1st player"<<endl;
            result = b.player(turn);
            turn++;
        }
        else if (turn == 1){
            cout<<"\t\t\t\t\t\t\t"<<p2<<" Turns - 2nd player"<<endl;
            result = b.player(turn);
            turn--;
        }

        if(result == 1 && turn == 1) cout<<"\t\t\t\t\t\t\t"<<p1<<" wins"<<endl , result =0;
        else if(result == 1 && turn == 0) cout<<"\t\t\t\t\t\t\t"<<p2<<" wins"<<endl , result =0;
        else if(result == 3) cout<<"\t\t\t\t\t\t\t"<<"DRAW"<<endl, result =0;
        else continue;
    }
}

int main(){

    game();

    return 0;
}
