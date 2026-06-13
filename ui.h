#pragma once
#include "CTDL.h"
#include <string>
#include <vector>
using namespace std;

// Ve man hinh va ve khung
void uiClear();
void uiDrawBoxTop(const string& title, int width = 64);
void uiDrawBoxBottom(int width = 64);
void uiDrawBoxLine(const string& text, int width = 64);
void uiDrawBoxSeparator(int width = 64);

// Menu tuong tac: tra ve chi so da chon, hoac -1 neu thoat/back
int uiMenu(const string& title, const vector<string>& items, const string& footer = "");

// Cho nguoi dung nhan Enter
void uiPause(const string& msg = "Nhan Enter de tiep tuc...");

// Huy nhap lieu (chi quay ve menu hien tai, khong ve menu chinh)
void uiResetCancel();
bool uiConsumeInputCancel();

// Nhap lieu (Esc hoac q khi chua goi -> huy). Tra ve false neu huy.
bool uiReadLine(const string& prompt, string& out);
bool uiReadWord(const string& prompt, string& out);
bool uiReadInt(const string& prompt, int& out);

// Chay giao dien chinh (goi cac handler CLI da co)
void uiRunApp(PTRCB &dscb, TreeHK &dshk);
