#include <windows.h>
#include<bits/stdc++.h>
using namespace std;

const int CELL = 10;
const int COLS = 80;
const int ROWS = 60;
const int W    = COLS * CELL;
const int H    = ROWS * CELL;
const int TICK = 80; 

struct Hash {
    size_t operator()(const pair<int,int>& p) const {
        return p.first * 1000 + p.second;
    }
};

/*
vector<vector<int>> grid(ROWS, vector<int>(COLS, 0));
vector<vector<int>> nxt(ROWS, vector<int>(COLS, 0));
*/

unordered_set<pair<int,int>, Hash> alive;
bool paused = false;

/*
void randomize(){
    for(int i = 0 ; i<ROWS;i++){
        for(int j = 0 ; j<COLS; j++){
            grid[i][j] = rand()%2;
        }
    }
}*/

void insert(int r,int c){
    alive.insert({r,c});
}

void clear(){
    alive.clear();
}

// This is not required, this swaps individual cells but std lib swap function does swap only the pointers which takes O(1);
/*
void swap(vector<vector<int>> &a,vector<vector<int>> &b){
    for(int i = 0 ; i<ROWS;i++){
        for(int j = 0 ; j< COLS; j++){
            int temp = a[i][j];
            a[i][j] = b[i][j];
            b[i][j] = temp;
        }
    }
}*/

void step(){

    unordered_map<pair<int,int>, int , Hash> cnt;

    for(auto cell : alive){

        int r = cell.first;
        int c = cell.second;

        cnt[{r,c}] += 0;

        for(int dr = -1; dr <= 1; dr++){
            for(int dc = -1; dc <= 1; dc++){

                if(dr == 0 && dc == 0) continue;

                int nr = r + dr;
                int nc = c + dc;

                if(nr < 0){
                    nr = ROWS - 1;
                }
                else if(nr >= ROWS){
                    nr = 0;
                }
                if(nc < 0) {
                    nc = COLS - 1;
                }
                else if(nc >= COLS) {
                    nc = 0;
                }

                cnt[{nr,nc}]++;
            }
        }
    }

    unordered_set<pair<int,int>, Hash> nextAlive;

    for(auto x : cnt){

        auto cell = x.first;
        int n = x.second;

        bool live = alive.count(cell);

        if(live){
            if(n == 2 || n == 3)
                nextAlive.insert(cell);
        }
        else{
            if(n == 3)
                nextAlive.insert(cell);
        }
    }

    alive.swap(nextAlive);
}
// Rendering (GDI — don't touch)
void render(HDC hdc) {

    HDC memDC = CreateCompatibleDC(hdc);

    HBITMAP bmp = CreateCompatibleBitmap(hdc, W, H);

    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);

    HBRUSH aliveBrush = CreateSolidBrush(RGB(0, 220, 80));
    HBRUSH deadBrush  = CreateSolidBrush(RGB(20, 20, 20));

    // Fill entire screen once with dead color
    RECT bg = {0, 0, W, H};
    FillRect(memDC, &bg, deadBrush);

    // Draw only living cells
    for(auto cell : alive){

        int r = cell.first;
        int c = cell.second;

        RECT rc = {
            c * CELL,
            r * CELL,
            c * CELL + CELL - 1,
            r * CELL + CELL - 1
        };

        FillRect(memDC, &rc, aliveBrush);
    }

    BitBlt(hdc, 0, 0, W, H, memDC, 0, 0, SRCCOPY);

    DeleteObject(aliveBrush);
    DeleteObject(deadBrush);

    SelectObject(memDC, oldBmp);
    DeleteObject(bmp);
    DeleteDC(memDC);
}

// Window Procedure (GDI — don't touch)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        render(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:

        if (wp == VK_SPACE)
            paused = !paused;

        if (wp == 'C')
            clear();

        if (wp == VK_ESCAPE)
            DestroyWindow(hwnd);

        return 0;

    case WM_LBUTTONDOWN: {

        int c = LOWORD(lp) / CELL;
        int r = HIWORD(lp) / CELL;

        if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {

            pair<int,int> p = {r,c};

            // CHANGED: toggle set membership
            if(alive.count(p))
                alive.erase(p);
            else
                alive.insert(p);
        }

        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

// WinMain (GDI — don't touch)
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {

    WNDCLASS wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "GoL";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    RegisterClass(&wc);

    RECT r = {0,0,W,H};

    AdjustWindowRect(
        &r,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        FALSE
    );

    HWND hwnd = CreateWindow(
        "GoL",
        "Game of Life | Space=Pause C=Clear Esc=Quit",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        r.right - r.left,
        r.bottom - r.top,
        nullptr,
        nullptr,
        hInst,
        nullptr
    );

    ShowWindow(hwnd, nCmdShow);

    SetTimer(hwnd, 1, TICK, nullptr);

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0)) {

        if (msg.message == WM_TIMER && !paused) {
            step();
            InvalidateRect(hwnd, nullptr, FALSE);
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
