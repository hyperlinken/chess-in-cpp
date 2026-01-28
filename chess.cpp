#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cassert>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define byte win_byte_override
#include <windows.h>
#undef byte
#endif

using namespace std;

struct p
{
    int id;
    int x, y;
    int deadx, deady;
    int *direct;
    int arr[60];
    int moved;
};

class board
{

private:
    static constexpr int EMPTY = 0;
    static constexpr int BORDER = -2;
    static constexpr int GRAVE = -1;

    static constexpr int PAWN = 1;
    static constexpr int BISHOP = 2;
    static constexpr int KNIGHT = 3;
    static constexpr int ROOK = 4;
    static constexpr int KING = 5;
    static constexpr int QUEEN = 6;

    bool inBoard(int x, int y)
    {
        return (x >= 1 && x <= 8 && y >= 1 && y <= 8);
    }

    void resetMoves(struct p *n)
    {
        for (int i = 0; i < 60; i++)
            n->arr[i] = -2; // sentinel everywhere
    }

    bool hasAnyMove(struct p *n)
    {
        for (int i = 0; i < 60 && n->arr[i] != -2; i += 2)
        {
            return true;
        }
        return false;
    }

    bool moveInArr(struct p *n, int x, int y)
    {
        for (int i = 0; i < 60 && n->arr[i] != -2; i += 2)
        {
            if (n->arr[i] == x && n->arr[i + 1] == y)
                return true;
        }
        return false;
    }

    bool moveInAvailable(int x, int y)
    {
        for (int i = 0; i < 30 && available[i] != -2; i += 2)
        {
            if (available[i] == x && available[i + 1] == y)
                return true;
        }
        return false;
    }

    // generic ray for rook/bishop/queen
    void ray(struct p *n, int dx, int dy, int &k)
    {
        int x = n->x + dx;
        int y = n->y + dy;
        int me = n->id / 10;
        int enemy = (me == 1 ? 2 : 1);

        while (box[x][y] >= 0)
        { // stops on -1 or -2
            if (box[x][y] == 0)
            {
                n->arr[k++] = x;
                n->arr[k++] = y;
            }
            else
            {
                if (box[x][y] / 10 != me)
                {
                    n->arr[k++] = x;
                    n->arr[k++] = y;

                    // ✅ correct king detection: KING = 5, not 6
                    if (box[x][y] / 10 == enemy && box[x][y] % 10 == KING)
                    {
                        checkto = enemy;
                        append(n);
                    }
                }
                break;
            }
            x += dx;
            y += dy;
        }
    }

public:
    struct p wk, wq, wr1, wr2, wh1, wh2, wb1, wb2, w1, w2, w3, w4, w5, w6, w7, w8;
    struct p bk, bq, br1, br2, bh1, bh2, bb1, bb2, b1, b2, b3, b4, b5, b6, b7, b8;

    struct p *p1[16] = {&w1, &w2, &w3, &w4, &w5, &w6, &w7, &w8, &wk, &wq, &wr1, &wr2, &wb1, &wb2, &wh1, &wh2};
    struct p *p2[16] = {&b1, &b2, &b3, &b4, &b5, &b6, &b7, &b8, &bk, &bq, &br1, &br2, &bb1, &bb2, &bh1, &bh2};

    int box[10][16] = {
        // 1 2  3  4  5   6  7  8  9  10 11 12 13 14
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2},
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // a
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // b
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // c
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // d
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // e
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // f
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // g
        {-2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -2}, // h
        {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2}};

    struct p *check[5] = {NULL, NULL, NULL, NULL, NULL};
    int checkto = 0;
    int availpiece[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int available[30] = {-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2};
    int check1 = 0, check2 = 0;

    // int castle(struct p * h){
    //     int c ,i,j,initialize;
    //     string t;
    //     struct p * r= NULL;
    //     struct p * a[4] = {&wr1,&wr2,&br1,&br2};
    //     if(h->id/10 == 1) c=check1 , initialize=0 ;
    //     else c=check2 , initialize=2;

    //     cout<<"enter rook position : ";
    //     cin>>t;
    //     i = (int)t[0] - 96;  //abcdefgh
    //     j = (int)t[1] - 48;  //12345678

    //     if(a[initialize]->x == i && a[initialize]->y == j) r = a[initialize];
    //     else if (a[initialize + 1]->x == i && a[initialize +1]->y == j) r = a[initialize+1] ;

    //     if( r == NULL){
    //         cout<<"INVALID INPUT"<<endl;
    //         return 1;
    //     }
    //     if(c == 0 && h->moved == 0 && r->moved == 0 ){
    //         int diff = h->y - r->y;
    //         if(diff>0 && safe(h->id/10 ,h->x,h->y - 2)){
    //             for(int i= r->y + 1 ; i < h->y ; i++){
    //                 if(box[h->x][h->y + i] != 0){
    //                     return 1;
    //                 }
    //             }
    //             box[h->x][h->y] = 0;
    //             box[r->x][r->y] = 0;
    //             box[h->x][h->y - 2] = h->id;
    //             h->y -= 2;
    //             box[h->x][h->y +1] = r->id;
    //             r->y +=1;
    //             h->moved = 1;
    //             r->moved=1;
    //             return 0;
    //         }
    //         else if(diff<0 && safe(h->id/10 ,h->x,h->y + 2)){
    //             for(int i= h->y + 1 ; i < r->y ; i++){
    //                 if(box[h->x][h->y + i] != 0){
    //                     return 1;
    //                 }
    //             }
    //             box[h->x][h->y] = 0;
    //             box[r->x][r->y] = 0;
    //             box[h->x][h->y + 2] = h->id;
    //             h->y += 2;
    //             box[h->x][h->y -1] = r->id;
    //             r->y -=1 ;
    //             h->moved = 1;
    //             r->moved=1;
    //             return 0;
    //         }
    //     }
    //     return 1;
    // }
    int castle(struct p *h)
    {
        int player = h->id / 10;
        if (h->moved)
            return 1;
        if (!safe(player, h->x, h->y))
            return 1; // can't castle out of check

        string t;
        cout << "enter rook position : ";
        cin >> t;
        if (t.size() != 2)
            return 1;

        // int rx = t[0] - 96;
        // int ry = t[1] - 48;

        int rx, ry;
        if (!parseSquare(t, rx, ry)) return 1;


        if (!inBoard(rx, ry))
            return 1;

        struct p *r = NULL;
        // pick rook of same color from the 4 known rooks
        struct p *rooks[4] = {&wr1, &wr2, &br1, &br2};
        for (auto rr : rooks)
        {
            if (rr->x == rx && rr->y == ry && rr->id / 10 == player)
            {
                r = rr;
                break;
            }
        }
        if (!r || r->moved)
            return 1;
        if (r->x != h->x)
            return 1;

        // determine side
        if (r->y < h->y)
        {
            // queenside: squares between rook and king empty
            for (int col = r->y + 1; col < h->y; col++)
            {
                if (box[h->x][col] != 0)
                    return 1;
            }
            // king passes through y-1 and lands y-2
            if (!safe(player, h->x, h->y - 1) || !safe(player, h->x, h->y - 2))
                return 1;

            box[h->x][h->y] = 0;
            box[r->x][r->y] = 0;

            h->y -= 2;
            r->y = h->y + 1;

            box[h->x][h->y] = h->id;
            box[r->x][r->y] = r->id;
        }
        else
        {
            // kingside
            for (int col = h->y + 1; col < r->y; col++)
            {
                if (box[h->x][col] != 0)
                    return 1;
            }
            if (!safe(player, h->x, h->y + 1) || !safe(player, h->x, h->y + 2))
                return 1;

            box[h->x][h->y] = 0;
            box[r->x][r->y] = 0;

            h->y += 2;
            r->y = h->y - 1;

            box[h->x][h->y] = h->id;
            box[r->x][r->y] = r->id;
        }

        h->moved = 1;
        r->moved = 1;
        return 0;
    }

    void clear()
    {
        for (int i = 0; i < 30; i++)
        {
            available[i] = -2;
        }
        for (int i = 0; i < 5; i++)
        {
            check[i] = NULL;
        }
        for (int i = 0; i < 16; i++)
        {
            availpiece[i] = 1;
        }
        checkto = 0;
    }
    void put(int a, int b)
    {
        int i = 0;
        while (available[i] != -2)
        {
            if (available[i] == a && available[i + 1] == b)
            {
                return;
            }
            i += 2;
        }
        available[i] = a;
        available[++i] = b;
    }
    void aftercheck(struct p *n)
    {
        int enx, eny, chkx, chky, diffx, diffy;
        for (int i = 0; i < 5; i++)
        {
            if (check[i] == NULL)
                break;
            enx = check[i]->x, eny = check[i]->y, chkx = n->x, chky = n->y, diffx = n->x - check[i]->x, diffy = n->y - check[i]->y;
            if (check[i]->id % 10 == 1 || check[i]->id % 10 == 3)
            {
                put(enx, eny);
            }
            else if(check[i]->id%10 == BISHOP || check[i]->id%10 == ROOK || check[i]->id%10 == QUEEN)
            {
                int dx = 0, dy = 0;
                for (int i = 1; i < 8; i++)
                {
                    if (diffx > 0)
                        dx--;
                    else if (diffx < 0)
                        dx++;

                    if (diffy > 0)
                        dy--;
                    else if (diffy < 0)
                        dy++;
                    put(chkx + dx, chky + dy);

                    if (((chkx + dx) == enx) && ((chky + dy) == eny))
                        break;
                }
            }
        }
    }
    // void render(){
    //     if(checkto == 1) check1 = 1;
    //     else if(checkto ==2) check2 = 1;
    //     queen(p1[9]);
    //     queen(p2[9]);
    //     for(int i=0; i<16; i++){
    //         if(i==9) continue;
    //         switch(p1[i]->id%10){
    //             case 1:
    //                 pawn(p1[i]);
    //                 break;
    //             case 2:
    //                 bishop(p1[i]);
    //                 break;
    //             case 3:
    //                 horse(p1[i]);
    //                 break;
    //             case 4:
    //                 rook(p1[i]);
    //                 break;
    //             case 5:
    //                 king(p1[i]);
    //                 break;
    //         }
    //         switch(p2[i]->id%10){
    //             case 1:
    //                 pawn(p2[i]);
    //                 break;
    //             case 2:
    //                 bishop(p2[i]);
    //                 break;
    //             case 3:
    //                 horse(p2[i]);
    //                 break;
    //             case 4:
    //                 rook(p2[i]);
    //                 break;
    //             case 5:
    //                 king(p2[i]);
    //                 break;
    //         }
    //     }

    //     struct p * h;
    //     if(checkto !=0 ){
    //         (checkto == 1) ? h = p1[9] : h = p2[9];
    //         aftercheck(h);
    //     }
    // }
    void render()
    {
        clear();     // ✅ must reset available/check/availpiece
        checkto = 0; // ✅ reset check

        // generate moves for both sides
        for (int i = 0; i < 16; i++)
        {
            // WHITE
            if (p1[i]->id > 0)
            {
                switch (p1[i]->id % 10)
                {
                case PAWN:
                    pawn(p1[i]);
                    break;
                case BISHOP:
                    bishop(p1[i]);
                    break;
                case KNIGHT:
                    horse(p1[i]);
                    break;
                case ROOK:
                    rook(p1[i]);
                    break;
                case KING:
                    king(p1[i]);
                    break;
                case QUEEN:
                    queen(p1[i]);
                    break;
                }
            }

            // BLACK
            if (p2[i]->id > 0)
            {
                switch (p2[i]->id % 10)
                {
                case PAWN:
                    pawn(p2[i]);
                    break;
                case BISHOP:
                    bishop(p2[i]);
                    break;
                case KNIGHT:
                    horse(p2[i]);
                    break;
                case ROOK:
                    rook(p2[i]);
                    break;
                case KING:
                    king(p2[i]);
                    break;
                case QUEEN:
                    queen(p2[i]);
                    break;
                }
            }
        }

        // if someone is in check, compute "blocking/capturing squares" (single-check only)
        if (checkto != 0)
        {
            struct p *checkedKing = (checkto == 1 ? &wk : &bk); // ✅ king is wk/bk
            aftercheck(checkedKing);
        }
    }

    // void pawn(struct p * n){
    //     if(n->id/10 == 1){
    //         if(box[n->x+1][n->y] == 0) {
    //             n->arr[0] = n->x+1;
    //             n->arr[1] = n->y;
    //         }
    //         else{
    //             n->arr[0] = -1;
    //             n->arr[1] = -1;
    //         }
    //         if(box[n->x+2][n->y] == 0 && n->x == 2){
    //             n->arr[2] = n->x+2;
    //             n->arr[3] = n->y;
    //         }
    //         else{
    //             n->arr[2] = -1;
    //             n->arr[3] = -1;
    //         }
    //         if(box[n->x+1][n->y-1]/10 > 0 ) {           //enemy
    //             n->arr[4] = n->x+1;
    //             n->arr[5] = n->y-1;

    //             if(box[n->x+1][n->y-1]/10 == 2 && box[n->x+1][n->y-1]%10 == 6){         //checking king
    //                 append(n);
    //                 checkto = 2;
    //             }
    //         }
    //         else{
    //             n->arr[4] = -1;
    //             n->arr[5] = -1;
    //         }
    //         if(box[n->x+1][n->y+1]/10 != 0 && box[n->x+1][n->y+1]/10 != -2 ) {
    //             n->arr[6] = n->x+1;
    //             n->arr[7] = n->y+1;

    //             if(box[n->x+1][n->y+1]/10 == 2 && box[n->x+1][n->y+1]%10 == 6){         //checking king
    //                 append(n);
    //                 checkto = 2;
    //             }
    //         }
    //         else{
    //             n->arr[6] = -1;
    //             n->arr[7] = -1;
    //         }
    //     }
    //     if(n->id/10 == 2){
    //         if(box[n->x-1][n->y] == 0){
    //             n->arr[0] = n->x-1;
    //             n->arr[1] = n->y;
    //         }
    //         else{
    //             n->arr[0] = -1;
    //             n->arr[1] = -1;
    //         }
    //         if(box[n->x-2][n->y] == 0 && n->x == 7){
    //             n->arr[2] = n->x-2;
    //             n->arr[3] = n->y;
    //         }
    //         else{
    //             n->arr[2] = -1;
    //             n->arr[3] = -1;
    //         }
    //         if(box[n->x-1][n->y+1]/10 != 0 && box[n->x-1][n->y+1]/10 != -2 ) {          //enemy
    //             n->arr[4] = n->x-1;
    //             n->arr[5] = n->y+1;

    //             if(box[n->x-1][n->y+1]/10 == 1 && box[n->x-1][n->y+1]%10 == 6){         //checking king
    //                 append(n);
    //                 checkto = 1;
    //             }
    //         }
    //         else{
    //             n->arr[4] = -1;
    //             n->arr[5] = -1;
    //         }
    //         if(box[n->x-1][n->y-1] != 0 && box[n->x-1][n->y-1] != -2) {
    //             n->arr[6] = n->x-1;
    //             n->arr[7] = n->y-1;

    //             if(box[n->x-1][n->y-1]/10 == 1 && box[n->x-1][n->y-1]%10 == 6){         //checking king
    //                 append(n);
    //                 checkto = 1;
    //             }
    //         }
    //         else{
    //             n->arr[6] = -1;
    //             n->arr[7] = -1;
    //         }
    //     }

    // }
    void pawn(struct p *n)
    {
        resetMoves(n);
        if (!inBoard(n->x, n->y))
            return;

        int k = 0;
        int me = n->id / 10;
        int enemy = (me == 1 ? 2 : 1);

        if (me == 1)
        {
            // forward
            if (box[n->x + 1][n->y] == 0)
            {
                n->arr[k++] = n->x + 1;
                n->arr[k++] = n->y;
                // double move only if first is empty
                if (n->x == 2 && box[n->x + 2][n->y] == 0)
                {
                    n->arr[k++] = n->x + 2;
                    n->arr[k++] = n->y;
                }
            }

            // captures
            for (int dy : {-1, +1})
            {
                int nx = n->x + 1, ny = n->y + dy;
                if (!inBoard(nx, ny))
                    continue;
                if (box[nx][ny] > 0 && box[nx][ny] / 10 == enemy)
                {
                    n->arr[k++] = nx;
                    n->arr[k++] = ny;
                    if (box[nx][ny] % 10 == KING)
                    {
                        checkto = enemy;
                        append(n);
                    }
                }
            }
        }
        else
        {
            // forward
            if (box[n->x - 1][n->y] == 0)
            {
                n->arr[k++] = n->x - 1;
                n->arr[k++] = n->y;
                if (n->x == 7 && box[n->x - 2][n->y] == 0)
                {
                    n->arr[k++] = n->x - 2;
                    n->arr[k++] = n->y;
                }
            }

            // captures
            for (int dy : {-1, +1})
            {
                int nx = n->x - 1, ny = n->y + dy;
                if (!inBoard(nx, ny))
                    continue;
                if (box[nx][ny] > 0 && box[nx][ny] / 10 == enemy)
                {
                    n->arr[k++] = nx;
                    n->arr[k++] = ny;
                    if (box[nx][ny] % 10 == KING)
                    {
                        checkto = enemy;
                        append(n);
                    }
                }
            }
        }
    }

    // void horse(struct p * n){
    //     int a=n->x , b=n->y;
    //     if( (a+1>=1 && a+1<=8) && (b+2>=1 && b+2<=8) && box[n->x+1][n->y+2] >=0 && box[n->x+1][n->y+2]/10 != n->id/10){
    //         n->arr[0] =  n->x+1;
    //         n->arr[1] = n->y+2;
    //         if(box[n->x+1][n->y+2]%10 == 6){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[0] = -1;
    //         n->arr[1] = -1;
    //     }
    //     if((a+2>=1 && a+2<=8) && (b+1>=1 && b+1<=8) && box[n->x+2][n->y+1] >=0 && box[n->x+2][n->y+1]/10 != n->id/10){
    //         n->arr[2] =  n->x+2;
    //         n->arr[3] = n->y+1;
    //         if(box[n->x+2][n->y+1]%10 == 6){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[2] = -1;
    //         n->arr[3] = -1;
    //     }
    //     if((a-1>=1 && a-1<=8) && (b+2>=1 && b+2<=8) && box[n->x-1][n->y+2] >=0 && box[n->x-1][n->y+2]/10 != n->id/10){
    //         n->arr[4] =  n->x-1;
    //         n->arr[5] = n->y+2;
    //         if(box[n->x-1][n->y+2]%10 == 6){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[4] = -1;
    //         n->arr[5] = -1;
    //     }
    //     if((a-2>=1 && a-2<=8) && (b+1>=1 && b+1<=8) && box[n->x-2][n->y+1] >=0 && box[n->x-2][n->y+1]/10 != n->id/10){
    //         n->arr[6] =  n->x-2;
    //         n->arr[7] = n->y+1;
    //         if(box[n->x-2][n->y+1]%10 == 6){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[6] = -1;
    //         n->arr[7] = -1;
    //     }
    //     if((a+1>=1 && a+1<=8) && (b-2>=1 && b-2<=8) && box[n->x+1][n->y-2] >=0 && box[n->x+1][n->y-2]/10 != n->id/10){
    //         n->arr[8] =  n->x+1;
    //         n->arr[9] = n->y-2;
    //         if(box[n->x+1][n->y-2]%10 == 6 ){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[8] = -1;
    //         n->arr[9] = -1;
    //     }
    //     if((a+2>=1 && a+2<=8) && (b-1>=1 && b-1<=8) && box[n->x+2][n->y-1] >=0 && box[n->x+2][n->y-1]/10 != n->id/10){
    //         n->arr[10] =  n->x+2;
    //         n->arr[11] = n->y-1;
    //         if(box[n->x+2][n->y-1]%10 == 6 ){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[10] = -1;
    //         n->arr[11] = -1;
    //     }
    //     if((a-1>=1 && a-1<=8) && (b-2>=1 && b-2<=8) && box[n->x-1][n->y-2] >=0 && box[n->x-1][n->y-2]/10 != n->id/10){
    //         n->arr[12] =  n->x-1;
    //         n->arr[13] = n->y-2;
    //         if(box[n->x-1][n->y-2]%10 == 6){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[12] = -1;
    //         n->arr[13] = -1;
    //     }
    //     if((a-2>=1 && a-2<=8) && (b-1>=1 && b-1<=8) && box[n->x-2][n->y-1] >=0 && box[n->x-2][n->y-1]/10 != n->id/10){
    //         n->arr[14] =  n->x-2;
    //         n->arr[15] = n->y-1;
    //         if(box[n->x-2][n->y-1]%10 == 6){
    //             append(n);
    //             (n->id/10 == 1) ? checkto=2 : checkto=1 ;
    //         }
    //     }
    //     else{
    //         n->arr[14] = -1;
    //         n->arr[15] = -1;
    //     }
    // }
    void horse(struct p *n)
    {
        resetMoves(n);
        if (!inBoard(n->x, n->y))
            return;

        int k = 0;
        int me = n->id / 10;
        int enemy = (me == 1 ? 2 : 1);

        const int dx[8] = {1, 2, 2, 1, -1, -2, -2, -1};
        const int dy[8] = {2, 1, -1, -2, -2, -1, 1, 2};

        for (int i = 0; i < 8; i++)
        {
            int nx = n->x + dx[i], ny = n->y + dy[i];
            if (!inBoard(nx, ny))
                continue;
            if (box[nx][ny] > 0 && box[nx][ny] / 10 == me)
                continue;

            n->arr[k++] = nx;
            n->arr[k++] = ny;

            if (box[nx][ny] > 0 && box[nx][ny] / 10 == enemy && box[nx][ny] % 10 == KING)
            {
                checkto = enemy;
                append(n);
            }
        }
    }

    // void bishop(struct p *n)
    // {
    //     int a = n->x;
    //     int b = n->y;
    //     int k = 0;
    //     for (int i = 0; i < 4; i++)
    //     {
    //         a = n->x;
    //         b = n->y;
    //         for (int j = 0; j < 8; j++)
    //         {
    //             if (i == 0)
    //                 a++, b++;
    //             else if (i == 1)
    //                 a++, b--;
    //             else if (i == 2)
    //                 a--, b++;
    //             else
    //                 a--, b--;

    //             if (box[a][b] < 0)
    //                 break;
    //             else if (box[a][b] > 0)
    //             {
    //                 if (box[a][b] / 10 == n->id / 10)
    //                     break;
    //                 else
    //                 {
    //                     n->arr[k] = a; // kill
    //                     n->arr[++k] = b;
    //                     if (box[a][b] % 10 == 6)
    //                     { // check
    //                         (n->id / 10 == 2) ? checkto = 1 : checkto = 2;
    //                         append(n);
    //                     }
    //                     k++;
    //                     break;
    //                 }
    //             }
    //             else
    //             {
    //                 n->arr[k] = a;
    //                 n->arr[++k] = b;
    //                 k++;
    //             }
    //         }
    //     }
    // }
    void bishop(struct p* n){
        resetMoves(n);
        if(!inBoard(n->x,n->y)) return;
        int k=0;
        ray(n, +1,+1,k);
        ray(n, +1,-1,k);
        ray(n, -1,+1,k);
        ray(n, -1,-1,k);
    }

    // void rook(struct p *n)
    // {
    //     int a = n->x;
    //     int b = n->y;
    //     int k = 0;
    //     for (int i = 0; i < 4; i++)
    //     {
    //         a = n->x;
    //         b = n->y;
    //         for (int j = 0; j < 8; j++)
    //         {
    //             if (i == 0)
    //                 b++;
    //             else if (i == 1)
    //                 b--;
    //             else if (i == 2)
    //                 a++;
    //             else
    //                 a--;

    //             if (box[a][b] < 0)
    //                 break;
    //             else if (box[a][b] > 0)
    //             {
    //                 if (box[a][b] / 10 == n->id / 10)
    //                     break;
    //                 else
    //                 {
    //                     n->arr[k] = a; // kill
    //                     n->arr[++k] = b;
    //                     if (box[a][b] % 10 == 6)
    //                     { // check
    //                         (n->id / 10 == 2) ? checkto = 1 : checkto = 2;
    //                         append(n);
    //                     }
    //                     k++;
    //                     break;
    //                 }
    //             }
    //             else
    //             {
    //                 n->arr[k] = a;
    //                 n->arr[++k] = b;
    //                 k++;
    //             }
    //         }
    //     }
    // }
    void rook(struct p* n){
        resetMoves(n);
        if(!inBoard(n->x,n->y)) return;
        int k=0;
        ray(n, +1,0,k);
        ray(n, -1,0,k);
        ray(n, 0,+1,k);
        ray(n, 0,-1,k);
    }

    // void king(struct p * n){
    //     int a = n->x;
    //     int b = n->y;
    //     int k =0;
    //     for(int i=1 ; i<=8 ; i++){
    //         a=n->x;
    //         b=n->y;
    //         for(int j=0; j<8 ; j++){
    //             switch(i){
    //                 case 1:
    //                     a++;
    //                     break;
    //                 case 2:
    //                     b++;
    //                     break;
    //                 case 3:
    //                     a--;
    //                     break;
    //                 case 4:
    //                     b--;
    //                     break;
    //                 case 5:
    //                     a++,b++;
    //                     break;
    //                 case 6:
    //                     a++,b--;
    //                     break;
    //                 case 7:
    //                     a--,b++;
    //                     break;
    //                 case 8:
    //                     a--,b--;
    //                     break;
    //             }

    //             if(box[a][b] < 0) break;
    //             else if(box[a][b] >0){
    //                 if(box[a][b]/10 == n->id/10) break;
    //                 else{
    //                     n->arr[k] = a;      //kill
    //                     n->arr[++k] = b;
    //                     if(box[a][b]%10 == 6){   //check
    //                         (n->id/10 == 2)?checkto = 1 : checkto = 2;
    //                         append(n);
    //                     }
    //                     k++;
    //                     break;
    //                 }
    //             }
    //             else{
    //                 n->arr[k] = a;
    //                 n->arr[k+1] = b;
    //                 k+=2;
    //             }
    //         }
    //     }
    // }

    void king(struct p *n)
    {
        resetMoves(n);
        if (!inBoard(n->x, n->y))
            return;

        int k = 0;
        int player = n->id / 10;

        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                if (dx == 0 && dy == 0)
                    continue;
                int nx = n->x + dx;
                int ny = n->y + dy;
                if (!inBoard(nx, ny))
                    continue;

                // can't land on own piece
                if (box[nx][ny] > 0 && box[nx][ny] / 10 == player)
                    continue;

                // if capturing, temporarily remove captured piece to evaluate safety
                int saved = box[nx][ny];
                box[nx][ny] = 0;
                bool ok = safe(player, nx, ny);
                box[nx][ny] = saved;

                if (ok)
                {
                    n->arr[k++] = nx;
                    n->arr[k++] = ny;
                }
            }
        }
    }

    void append(struct p *n)
    {
        int i = 0;
        while (check[i] != NULL)
        {
            i++;
        }
        check[i] = n;
    }

    // int safe(int player ,int a,int b){

    //     for(int i=1;i<8;i++){           //king and rook
    //         if((box[a][b+i] == -2 || box[a][b+i]/10 == player) && box[a][b+i]%10 !=6) break;
    //         else if((box[a][b+i]%10 == 5)|| (box[a][b+i]%10 == 4)) {return 0;}
    //     }
    //     for(int i=1;i<8;i++){
    //         if(box[a][b-i] == -2 || box[a][b-i]/10 == player && box[a][b-i]%10 !=6) break;
    //         else if(box[a][b-i]%10 == 5 || box[a][b-i]%10 == 4) return 0;
    //     }
    //     for(int i=1;i<8;i++){
    //         if(box[a+i][b] == -2 || box[a+i][b]/10 == player && box[a+i][b]%10 !=6) break;
    //         else if(box[a+i][b]%10 == 5 || box[a+i][b]%10 == 4) return 0;
    //     }
    //     for(int i=1;i<8;i++){
    //         if(box[a-i][b] == -2 || box[a-i][b]/10 == player && box[a-i][b]%10 !=6) break;
    //         else if(box[a-i][b]%10 == 5 || box[a-i][b]%10 == 4) return 0;
    //     }

    //     for(int i=1;i<8;i++){               //king and bishop
    //         if(box[a+i][b+i] == -2 || box[a+i][b+i]/10 == player && box[a+i][b+i]%10 !=6) break;
    //         else if(box[a+i][b+i]%10 == 5 || box[a+i][b+i]%10 == 2) return 0;
    //     }
    //     for(int i=1;i<8;i++){
    //         if(box[a-i][b-i] == -2 || box[a-i][b-i]/10 == player && box[a-i][b-i]%10 !=6) break;
    //         else if(box[a-i][b-i]%10 == 5 || box[a-i][b-i]%10 == 2) return 0;
    //     }
    //     for(int i=1;i<8;i++){
    //         if(box[a+i][b-i] == -2 || box[a+i][b-i]/10 == player && box[a+i][b-i]%10 !=6) break;
    //         else if(box[a+i][b-i]%10 == 5 || box[a+i][b-i]%10 == 2) return 0;
    //     }
    //     for(int i=1;i<8;i++){
    //         if(box[a-i][b+i] == -2 || box[a-i][b+i]/10 == player && box[a-i][b+i]%10 !=6) break;
    //         else if(box[a-i][b+i]%10 == 5 || box[a-i][b+i]%10 == 2) return 0;
    //     }

    //     int bit[16] = {-2,-2,-1,1,2,2,1,-1,-2,-2,-1,1,2,2,1,-1};       //horse alogrithm
    //     for(int i=0;i<8; i++){
    //         if((a+bit[i+1]>=1 && a+bit[i+1]<=8) && (b+bit[i+3]>=1 && b+bit[i+3]<=8) && box[a+bit[i+1]][b+bit[i+3]]%10 == 3 && box[a+bit[i+1]][b+bit[i+3]]/10 != player) return 0;
    //     }

    //     int p,q;
    //     for(int i =0; i<9 ; i++){
    //         if(i%3 == 0)  p=-1;
    //         else if(i%3 == 1) p=0;
    //         else p=1;

    //         if(i/3 == 0) q=-1;
    //         else if(i/3 == 1) q=0;
    //         else q=1;

    //         if(box[a+q][b+q]%10 == 6 && box[a+q][b+q]/10 != player && (a+q>=1 && a+q<=8) && (b+p>=1 && b+p<=8)) return 0;
    //     }

    //     //pawn
    //     if( player == 2){
    //         if(((box[a-1][b+1] != -2) && (box[a-1][b+1]%10 == 1 && box[a-1][b+1]/10 == 1)) || ((box[a-1][b-1] != -2) && (box[a-1][b-1]%10 == 1 && box[a-1][b-1]/10 == 1))) return 0;
    //     }
    //     else{
    //         if(((box[a+1][b+1] != -2) && (box[a+1][b+1]%10 == 1 && box[a+1][b+1]/10 == 2)) ||((box[a+1][b-1] != -2) && (box[a+1][b-1]%10 == 1 && box[a+1][b-1]/10 == 2))) return 0;
    //     }

    //     return 1;
    // }

    int safe(int player, int a, int b)
    {
        if (!inBoard(a, b))
            return 0;
        int enemy = (player == 1 ? 2 : 1);

        // 1) Knight attacks
        const int kdx[8] = {1, 2, 2, 1, -1, -2, -2, -1};
        const int kdy[8] = {2, 1, -1, -2, -2, -1, 1, 2};
        for (int i = 0; i < 8; i++)
        {
            int nx = a + kdx[i], ny = b + kdy[i];
            if (!inBoard(nx, ny))
                continue;
            if (box[nx][ny] > 0 && box[nx][ny] / 10 == enemy && box[nx][ny] % 10 == KNIGHT)
                return 0;
        }

        // 2) Pawn attacks
        if (enemy == 1)
        {
            // white pawn attacks (x+1,y±1) => pawn would be at (a-1, b±1)
            for (int dy : {-1, +1})
            {
                int px = a - 1, py = b + dy;
                if (inBoard(px, py) && box[px][py] > 0 && box[px][py] / 10 == enemy && box[px][py] % 10 == PAWN)
                    return 0;
            }
        }
        else
        {
            // black pawn attacks (x-1,y±1) => pawn would be at (a+1, b±1)
            for (int dy : {-1, +1})
            {
                int px = a + 1, py = b + dy;
                if (inBoard(px, py) && box[px][py] > 0 && box[px][py] / 10 == enemy && box[px][py] % 10 == PAWN)
                    return 0;
            }
        }

        // 3) Enemy king adjacency
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                if (dx == 0 && dy == 0)
                    continue;
                int nx = a + dx, ny = b + dy;
                if (!inBoard(nx, ny))
                    continue;
                if (box[nx][ny] > 0 && box[nx][ny] / 10 == enemy && box[nx][ny] % 10 == KING)
                    return 0;
            }
        }

        // 4) Sliding attacks: rook/queen
        const int rdx[4] = {1, -1, 0, 0};
        const int rdy[4] = {0, 0, 1, -1};
        for (int d = 0; d < 4; d++)
        {
            int x = a + rdx[d], y = b + rdy[d];
            while (box[x][y] >= 0)
            {
                if (box[x][y] == 0)
                {
                    x += rdx[d];
                    y += rdy[d];
                    continue;
                }
                if (box[x][y] / 10 == player)
                    break;
                int t = box[x][y] % 10;
                if (t == ROOK || t == QUEEN)
                    return 0;
                break;
            }
        }

        // 5) Sliding attacks: bishop/queen
        const int bdx[4] = {1, 1, -1, -1};
        const int bdy[4] = {1, -1, 1, -1};
        for (int d = 0; d < 4; d++)
        {
            int x = a + bdx[d], y = b + bdy[d];
            while (box[x][y] >= 0)
            {
                if (box[x][y] == 0)
                {
                    x += bdx[d];
                    y += bdy[d];
                    continue;
                }
                if (box[x][y] / 10 == player)
                    break;
                int t = box[x][y] % 10;
                if (t == BISHOP || t == QUEEN)
                    return 0;
                break;
            }
        }

        return 1;
    }

    // void queen(struct p * n){
    //     int a = n->x;
    //     int b = n->y;
    //     int p,q,s=0;

    //     for(int i =0; i<9 ; i++){
    //         if(i%3 == 0)  p=-1;
    //         else if(i%3 == 1) p=0;
    //         else p=1;

    //         if(i/3 == 0) q=-1;
    //         else if(i/3 == 1) q=0;
    //         else q=1;

    //         if(box[a+q][b+p] >= 0 && box[a+q][b+p]/10 != n->id/10 && safe(n->id/10 ,a+q , b+p)){
    //             n->arr[s] = a+q;
    //             n->arr[s+1] = b+p;
    //         }
    //         else{
    //             n->arr[s] = -1;
    //             n->arr[s+1] = -1;
    //         }
    //         s+=2;
    //     }
    // }
    void queen(struct p *n)
    {
        // Queen = rook + bishop rays
        resetMoves(n);
        if (!inBoard(n->x, n->y))
            return;

        int k = 0;
        ray(n, +1, 0, k);
        ray(n, -1, 0, k);
        ray(n, 0, +1, k);
        ray(n, 0, -1, k);

        ray(n, +1, +1, k);
        ray(n, +1, -1, k);
        ray(n, -1, +1, k);
        ray(n, -1, -1, k);
    }

    // int windraw(int turn)
    // {
    //     if (checkto != 0)
    //     {
    //         struct p *a;
    //         struct p **h;
    //         int availsize = 0, piecesize = 0, z = 0, x = 0, c = 0;
    //         (checkto == 1) ? h = p1 : h = p2;
    //         for (int i = 0; i < 16; i++)
    //         { // checking if any other piece have steps
    //             availpiece[i] = 0;
    //             if (i == 9)
    //                 continue;
    //             x = 0;
    //             while (h[i]->arr[x] != -2)
    //             {
    //                 c = 0;
    //                 while (available[c] != -2)
    //                 {
    //                     if (h[i]->arr[x] == available[c] && h[i]->arr[x + 1] == available[c + 1])
    //                     {
    //                         availpiece[i] = 1; // toggling if any pieces could move or not move while checked
    //                         availsize++;
    //                     }
    //                     c += 2;
    //                 }
    //                 x += 2;
    //             }
    //         }
    //         (checkto == 1) ? a = p1[9] : a = p2[9];
    //         while (a->arr[z] != -2)
    //         { ////checking if queen piece have steps
    //             if (a->arr[z] != -1)
    //                 piecesize++, availpiece[9] = 1;
    //             z += 2;
    //         }
    //         if (availsize + piecesize == 0)
    //             return 1; // win  by piece after check , player have no moves
    //     }
    //     else
    //     {
    //         struct p **h;
    //         int z, movs = 0;
    //         (turn == 0) ? h = p1 : h = p2;
    //         for (int i = 0; i < 16; i++)
    //         { // iterating pieces
    //             z = 0;
    //             while (h[i]->arr[z] != -2)
    //             { // iterating that particular piece from iterating pieces have any steps
    //                 if (h[i]->arr[z] != -1)
    //                 {
    //                     movs = 1; // if i get any single steps for any piecees that no need further checking to know if it is draw position or not
    //                     break;
    //                 }
    //                 z += 2;
    //             }
    //             if (movs == 1)
    //                 break;
    //         }
    //         if (movs != 1)
    //             return 3; // draw      if not checked and any pieces dont have any steps
    //     }
    //     return 0;
    // }

    int windraw(int turn)
    {
        if (checkto != 0)
        {
            struct p **h = (checkto == 1) ? p1 : p2;
            int availsize = 0;

            // pieces except king (king index = 8)
            for (int idx = 0; idx < 16; idx++)
            {
                availpiece[idx] = 0;
                if (idx == 8) continue; // ✅ skip king

                for (int x = 0; x < 60 && h[idx]->arr[x] != -2; x += 2)
                {
                    for (int c = 0; c < 30 && available[c] != -2; c += 2)
                    {
                        if (h[idx]->arr[x] == available[c] && h[idx]->arr[x + 1] == available[c + 1])
                        {
                            availpiece[idx] = 1;
                            availsize = 1;
                            break;
                        }
                    }
                    if (availpiece[idx]) break;
                }
            }

            // king moves
            struct p *king = (checkto == 1) ? &wk : &bk;
            availpiece[8] = (king->arr[0] != -2) ? 1 : 0; // ✅ king has any legal move?

            if (availsize == 0 && availpiece[8] == 0) return 1; // checkmate
        }
        else
        {
            struct p **h = (turn == 0) ? p1 : p2;
            for (int idx = 0; idx < 16; idx++)
            {
                if (h[idx]->arr[0] != -2) return 0; // any move exists
            }
            return 3; // stalemate/draw
        }
        return 0;
    }

    bool parseSquare(const string& s, int &x, int &y) {
        if (s.size() != 2) return false;

        char file = (char)tolower((unsigned char)s[0]); // a-h
        char rank = s[1];                               // 1-8

        if (file < 'a' || file > 'h') return false;
        if (rank < '1' || rank > '8') return false;

        y = file - 'a' + 1;   // file -> y (1..8)
        x = rank - '0';       // rank -> x (1..8)
        return true;
    }


    void printLine()
    {
        // tune this once if spacing changes
        const int LINE = 43;
        cout << "   ";
        for (int i = 0; i < LINE; i++)
            cout << "-";
        cout << "\n";
    }

    void display()
    {
        cout << "    ";
        for (int y = 1; y <= 8; y++)
            cout << "  " << (char)('a' + y - 1) << "  ";
        cout << "\n";

        printLine();

        for (int x = 8; x >= 1; x--) {
            cout << " " << x << " ||";
            for (int y = 1; y <= 8; y++) {
                cout << " " << looks(x, y) << " ";
                if (y != 8) cout << "|";
            }
            cout << "||\n";
            printLine();
        }
    }


    string look(int i, int j)
    {
        char player = '0' + box[i][j] / 10;
        char pieces[7] = {' ', 'P', 'B', 'H', 'R', 'K', 'Q'};
        int piece = box[i][j] % 10;
        string s = "";
        s += pieces[piece];
        (player == '0') ? s += ' ' : s += player;
        return s;
    }
    string looks(int i, int j)
    {
        int v = box[i][j];
        if (v <= 0)
            return "  "; // empty, -1, -2 => blank

        int player = v / 10; // 1 or 2
        int piece = v % 10;  // 1..6
        if (piece < 1 || piece > 6)
            return "  ";

        // index = piece id: 1=P,2=B,3=N,4=R,5=K,6=Q
        static const string W[7] = {
            "  ", u8"\u2659 ", u8"\u2657 ", u8"\u2658 ", u8"\u2656 ", u8"\u2654 ", u8"\u2655 "};
        static const string B[7] = {
            "  ", u8"\u265F ", u8"\u265D ", u8"\u265E ", u8"\u265C ", u8"\u265A ", u8"\u265B "};

        // your ids: 1x = white, 2x = black
        return (player == 1) ? B[piece] : W[piece];

    }

    // void promotion(struct p * n){
    //     if(n->id/10 == 2 && n->id%10 == 1 && n->x == 1){   //  pawn promotion
    //             int promote;
    //             do{
    //                 if(promote ==2 || promote ==3 || promote ==4 || promote == 5) break;
    //                 cout<<"enter number to promote to K=5,R=4,H=3,B=2 : ";
    //                 scanf("%d" , &promote);
    //             }
    //             while(1);
    //             n->id = 20+promote;
    //         }
    //         else if(n->id/10 == 1 && n->id%10 == 1 && n->x == 8){
    //             int promote;
    //             do{
    //                 if(promote ==2 || promote ==3 || promote ==4 || promote == 5) break;
    //                 cout<<"enter number to promote to K=5,R=4,H=3,B=2 : ";
    //                 scanf("%d" , &promote);
    //             }
    //             while(1);
    //             n->id = 10+promote;
    //         }
    // }
    void promotion(struct p *n)
    {
        if (!inBoard(n->x, n->y))
            return;

        if (n->id % 10 != PAWN)
            return;

        if (n->id / 10 == 1 && n->x == 8)
        {
            int promote = 0;
            while (true)
            {
                cout << "Promote to: Q=6, R=4, H=3, B=2 : ";
                cin >> promote;
                if (promote == QUEEN || promote == ROOK || promote == KNIGHT || promote == BISHOP)
                    break;
                else continue;
            }
            n->id = 10 + promote;
        }
        else if (n->id / 10 == 2 && n->x == 1)
        {
            int promote = 0;
            while (true)
            {
                cout << "Promote to: Q=6, R=4, H=3, B=2 : ";
                cin >> promote;
                if (promote == QUEEN || promote == ROOK || promote == KNIGHT || promote == BISHOP)
                    break;
            }
            n->id = 20 + promote;
        }
    }

    // int mov(struct p *n, string f, string t)
    // {

    //     int i, j, p, q, alright = 0;

    //     i = (int)t[0] - 96; // abcdefgh
    //     j = (int)t[1] - 48; // 12345678

    //     p = (int)f[0] - 96; // abcdefgh
    //     q = (int)f[1] - 48; // 12345678

    //     if (checkto != 0)
    //     {
    //         int k = 0, l = 0;
    //         if (n->id % 10 != 6)
    //         {
    //             while (n->arr[k] != -2)
    //             {
    //                 if (n->arr[k] == i && n->arr[k + 1] == j)
    //                 {
    //                     while (available[l] != -2)
    //                     {
    //                         if (n->arr[k] == available[l] && n->arr[k + 1] == available[l + 1])
    //                         {
    //                             alright = 2;
    //                             break;
    //                         }
    //                         l += 2;
    //                     }
    //                 }

    //                 if (alright == 2)
    //                     break;
    //                 k += 2;
    //             }
    //         }
    //         else
    //         {
    //             struct p *a;
    //             (checkto == 1) ? a = p1[9] : a = p2[9];
    //             while (a->arr[l] != -2)
    //             {
    //                 if (a->arr[l] == i && a->arr[l + 1] == j)
    //                 {
    //                     alright = 2;
    //                     break;
    //                 }
    //                 l += 2;
    //             }
    //         }
    //     }
    //     else
    //     {
    //         int k = 0;
    //         while (n->arr[k] != -2)
    //         {
    //             if (n->arr[k] == i && n->arr[k + 1] == j)
    //             {
    //                 alright = 2;
    //                 break;
    //             }
    //             k += 2;
    //         }
    //     }

    //     if (alright == 2)
    //     {
    //         box[p][q] = 0;
    //         n->x = i, n->y = j;

    //         struct p **h;
    //         (n->id / 10 == 1) ? h = p2 : h = p1;

    //         if (box[i][j] > 0)
    //         {
    //             for (int z = 0; z < 16; z++)
    //             {
    //                 if (h[z]->x == i && h[z]->y == j)
    //                 {
    //                     h[z]->x = h[z]->deadx;
    //                     h[z]->y = h[z]->deady;
    //                     box[h[z]->deadx][h[z]->deady] = h[z]->id;
    //                     break;
    //                 }
    //             }
    //         }

    //         promotion(n);

    //         box[i][j] = n->id;

    //         if (n->moved == 0)
    //             n->moved = 1;

    //         return 0;
    //     }
    //     cout << "ENTER VALID INPUT" << endl;
    //     return 1;
    // }
    int mov(struct p *n, string f, string t)
    {
        if (f.size() != 2 || t.size() != 2) {
            cout << "ENTER VALID INPUT" << endl;
            return 1;
        }

        int i = (int)t[0] - 96; // target x (a..h => 1..8)
        int j = (int)t[1] - 48; // target y (1..8)

        int p = (int)f[0] - 96; // from x
        int q = (int)f[1] - 48; // from y

        if (!parseSquare(t, i, j) || !parseSquare(f, p, q)) {
            cout << "ENTER VALID INPUT" << endl;
            return 1;
        }


        if (!inBoard(i, j) || !inBoard(p, q)) {
            cout << "ENTER VALID INPUT" << endl;
            return 1;
        }

        int alright = 0;

        if (checkto != 0)
        {
            // ✅ if KING is moving in check: only need to be in king's arr (already safe filtered)
            if (n->id % 10 == KING)
            {
                if (moveInArr(n, i, j)) alright = 2;
            }
            else
            {
                // other pieces: must be in their moves AND must be one of blocking/capturing squares
                if (moveInArr(n, i, j) && moveInAvailable(i, j)) alright = 2;
            }
        }
        else
        {
            if (moveInArr(n, i, j)) alright = 2;
        }

        if (alright == 2)
        {
            box[p][q] = 0;
            n->x = i; n->y = j;

            struct p **h = (n->id / 10 == 1) ? p2 : p1;

            if (box[i][j] > 0)
            {
                for (int z = 0; z < 16; z++)
                {
                    if (h[z]->x == i && h[z]->y == j)
                    {
                        h[z]->x = h[z]->deadx;
                        h[z]->y = h[z]->deady;
                        box[h[z]->deadx][h[z]->deady] = h[z]->id;
                        break;
                    }
                }
            }

            promotion(n);
            box[i][j] = n->id;

            if (n->moved == 0) n->moved = 1;
            return 0;
        }

        cout << "ENTER VALID INPUT" << endl;
        return 1;
    }


    // public:

    int player(int turn)
    {
        render();
        int wd = 0;
        wd = windraw(turn);
        display();
        if (wd == 1)
            return 1;
        else if (wd == 3)
            return 3;

        string from, to;

        int p, q;
        int ok = 1, r, cast;
        struct p **h;

        while (ok)
        {
            cast = 1;
            cout << "enter piece position that move : ";
            cin >> from;
            struct p *a = (turn == 0 ? &wk : &bk);

            // if (from == "castle")
            
            // {
            //     int c;
            //     // (turn == 0) ? a = p1[9] : a = p2[9];
            //     // (turn == 0) ? c = check1 : c = check2;
            //     struct p* a = (turn == 0 ? &wk : &bk);
            //     cast = castle(a);

            //     if (c == 0 && a->moved == 0)
            //         cast = castle(a);
            //     else
            //         cout << "NOT AVAILABLE" << endl;
            //     if (cast == 0)
            //         break;
            //     cout << "NOT POSSIBLE" << endl;
            //     continue;
            // }
            if (from == "castle") {
                struct p* k = (turn == 0 ? &wk : &bk);
                if (castle(k) == 0) break;
                cout << "NOT POSSIBLE" << endl;
                continue;
            }



            // p = (int)from[0] - 96; // abcdefgh
            // q = (int)from[1] - 48; // 12345678
            if (!parseSquare(from, p, q)) {
                cout << "ENTER VALID POSITION" << endl;
                continue;
            }


            (turn == 0) ? h = p1 : h = p2;
            int v = -1, ok = 1, ymoves = 0;

            for (r = 0; r < 16; r++)
            {
                if (h[r]->x == p && h[r]->y == q)
                {
                    ok = 2;
                    for (int j = 0; j < 60; j++)
                    {
                        if (h[r]->arr[j] != -2)
                        {
                            ymoves = 1;
                            break;
                        }
                    }
                    break;
                }
            }
            if (ok != 2)
            {
                cout << "ENTER VALID POSITION" << endl;
                continue;
            }
            if (ymoves == 0 && checkto == 0)
            {
                cout << "HAVE NO PLACE TO MOVE" << endl;
                continue;
            }
            if (availpiece[r] == 1)
                ok = 1;
            else
            {
                cout << "CHECK!!!" << endl;
                continue;
            }

            while (ok && cast)
            {
                cout << "enter piece position where it move : ";
                cin >> to;
                if (to == "cancel")
                    break;
                ok = mov(h[r], from, to);
            }
            if (ok == 0)
                break;
        }
        return 5;
    }

    void piecing(struct p *n, int s, int x, int y)
    {
        n->id = s;
        n->y = y;
        n->x = x;
        n->deadx = x;
        n->deady = y + 8;
        n->moved = 0;
        box[x][y] = n->id;
        for (int k = 0; k <= 59; k++)
        {
            n->arr[k] = -2;
        }
    }
};

void game()
{
    board b;
    b.piecing(&b.w1, 11, 2, 1);
    b.piecing(&b.w2, 11, 2, 2);
    b.piecing(&b.w3, 11, 2, 3);
    b.piecing(&b.w4, 11, 2, 4);
    b.piecing(&b.w5, 11, 2, 5);
    b.piecing(&b.w6, 11, 2, 6);
    b.piecing(&b.w7, 11, 2, 7);
    b.piecing(&b.w8, 11, 2, 8);
    b.piecing(&b.wk, 15, 1, 4);
    b.piecing(&b.wq, 16, 1, 5);
    b.piecing(&b.wr1, 14, 1, 1);
    b.piecing(&b.wr2, 14, 1, 8);
    b.piecing(&b.wh1, 13, 1, 2);
    b.piecing(&b.wh2, 13, 1, 7);
    b.piecing(&b.wb1, 12, 1, 3);
    b.piecing(&b.wb2, 12, 1, 6);

    b.piecing(&b.b1, 21, 7, 1);
    b.piecing(&b.b2, 21, 7, 2);
    b.piecing(&b.b3, 21, 7, 3);
    b.piecing(&b.b4, 21, 7, 4);
    b.piecing(&b.b5, 21, 7, 5);
    b.piecing(&b.b6, 21, 7, 6);
    b.piecing(&b.b7, 21, 7, 7);
    b.piecing(&b.b8, 21, 7, 8);
    b.piecing(&b.bk, 25, 8, 4);
    b.piecing(&b.bq, 26, 8, 5);
    b.piecing(&b.br1, 24, 8, 1);
    b.piecing(&b.br2, 24, 8, 8);
    b.piecing(&b.bh1, 23, 8, 2);
    b.piecing(&b.bh2, 23, 8, 7);
    b.piecing(&b.bb1, 22, 8, 3);
    b.piecing(&b.bb2, 22, 8, 6);

    string p1, p2;
    cout << "enter Player 1 Name : ";
    cin >> p1;
    cout << "enter Player 2 Name : ";
    cin >> p2;
    int result = 5, turn = 0;
    while (true)
    {
        cout << "\t\t\t\t\t\t\t" << (turn == 0 ? p1 : p2)
             << " Turn - " << (turn == 0 ? "WHITE" : "BLACK") << endl;


        result = b.player(turn);

        if (result == 1)
        {
            // current player is checkmated => opponent wins
            cout << "\t\t\t\t\t\t\t" << (turn == 0 ? p2 : p1) << " wins" << endl;
            break;
        }
        if (result == 3)
        {
            cout << "\t\t\t\t\t\t\tDRAW" << endl;
            break;
        }

        turn ^= 1; // switch turn only if game continues
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    game();
    return 0;
}
