#include <windows.h>
#include<bits/stdc++.h>
using namespace std;

const int CELL = 10;
const int COLS = 80;
const int ROWS = 60;
const int W    = COLS * CELL;
const int H    = ROWS * CELL;
const int TICK = 80; 

vector<vector<int>> grid(ROWS, vector<int>(COLS, 0));
vector<vector<int>> nxt(ROWS, vector<int>(COLS, 0));
bool paused = false;

void randomize(){
    for(int i = 0 ; i<ROWS;i++){
        for(int j = 0 ; j<COLS; j++){
            grid[i][j] = rand()%2;
        }
    }
}

void clear(){
    for(int i = 0 ; i<ROWS;i++){
        for(int j = 0 ; j<COLS; j++){
            grid[i][j] = 0;
        }
    }
}

int neighbours(int r, int c){
    int count = 0;
    for(int i = r-1 ; i<=r+1; i++){
        for(int j = c-1 ; j<=c+1;j++){
            if(i==r && j == c){
                continue;
            }
            int ni = (i + ROWS) % ROWS;
            int nj = (j + COLS) % COLS;
            if(grid[ni][nj] == 1) count++;
        }
    }
    return count;
}

void swap(vector<vector<int>> &a,vector<vector<int>> &b){
    for(int i = 0 ; i<ROWS;i++){
        for(int j = 0 ; j< COLS; j++){
            int temp = a[i][j];
            a[i][j] = b[i][j];
            b[i][j] = temp;
        }
    }
}

void step(){
    for(int i = 0 ; i<ROWS;i++){
        for(int j = 0 ; j<COLS; j++){
            int n = neighbours(i,j);
            if(grid[i][j]==1){
                if(n<2 || n>3){
                    nxt[i][j] = 0;
                }
                else{
                    nxt[i][j] = 1;
                }
            }
            else{
                if(n==3){
                    nxt[i][j]=1;
                }
                else{
                    nxt[i][j]=0;
                }
            }
        }
    }
    swap(grid,nxt); 
}
// Rendering (GDI — don't touch)
void render(HDC hdc) {
    HDC     memDC  = CreateCompatibleDC(hdc);
    HBITMAP bmp    = CreateCompatibleBitmap(hdc, W, H);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);

    HBRUSH alive = CreateSolidBrush(RGB(0, 220, 80));
    HBRUSH dead  = CreateSolidBrush(RGB(20, 20, 20));

    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) {
            RECT rc = { c*CELL, r*CELL, c*CELL+CELL-1, r*CELL+CELL-1 };
            FillRect(memDC, &rc, grid[r][c] ? alive : dead);
        }

    BitBlt(hdc, 0, 0, W, H, memDC, 0, 0, SRCCOPY);

    DeleteObject(alive);
    DeleteObject(dead);
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
        if (wp == VK_SPACE)  paused = !paused;
        if (wp == 'R')       randomize();
        if (wp == 'C')       clear();
        if (wp == VK_ESCAPE) DestroyWindow(hwnd);
        return 0;

    case WM_LBUTTONDOWN: {
        int c = LOWORD(lp) / CELL;
        int r = HIWORD(lp) / CELL;
        if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
            grid[r][c] ^= 1;
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
    WNDCLASS wc      = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "GoL";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);

    RECT r = {0, 0, W, H};
    AdjustWindowRect(&r, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE);
    HWND hwnd = CreateWindow("GoL", "Game of Life  |  Space=Pause  R=Random  C=Clear  Esc=Quit",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        r.right - r.left, r.bottom - r.top,
        nullptr, nullptr, hInst, nullptr);

    ShowWindow(hwnd, nCmdShow);
    randomize();
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
