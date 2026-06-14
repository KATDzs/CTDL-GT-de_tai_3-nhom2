#pragma once
#include "CTDL.h"
#include <string>
using namespace std;

const int UI_DEFAULT_BOX_WIDTH = 78;
const int UI_FORM_MAX_FIELDS = 16;
const int UI_REF_MAX_LINES = 64;
const int UI_REF_DISPLAY_MAX = 8;

// Ve man hinh va ve khung
void uiClear();
void uiDrawBoxTop(const string& title, int width = UI_DEFAULT_BOX_WIDTH);
void uiDrawBoxBottom(int width = UI_DEFAULT_BOX_WIDTH);
void uiDrawBoxLine(const string& text, int width = UI_DEFAULT_BOX_WIDTH);
void uiDrawBoxSeparator(int width = UI_DEFAULT_BOX_WIDTH);
void uiShowTaskScreen(const string& title, const string& subtitle = "");
void uiShowDataBox(const string& title, const string lines[], int lineCount);

// Menu tuong tac: tra ve chi so da chon, hoac -1 neu thoat/back
int uiMenu(const string& title, const string items[], int itemCount, const string& footer = "");

// Cho nguoi dung nhan Enter
void uiPause(const string& msg = "Nhan Enter de tiep tuc...");

// Huy nhap lieu (chi quay ve menu hien tai, khong ve menu chinh)
void uiResetCancel();
bool uiConsumeInputCancel();

// Nhap lieu (Esc hoac q khi chua goi -> huy). Tra ve false neu huy.
bool uiReadLine(const string& prompt, string& out);
bool uiReadWord(const string& prompt, string& out);
bool uiReadInt(const string& prompt, int& out);

// Form nhap lieu: hien san nhan + du lieu tham chieu (tu file) trong khung
void uiShowFormScreen(const string& title, const string refLines[], int refCount, const string labels[], const string values[], int fieldCount, int activeIndex = -1, const string& footer = "Esc/q: huy thao tac");
bool uiFormReadWord(const string& title, const string refLines[], int refCount, const string labels[], string values[], int fieldCount, int fieldIndex);
bool uiFormReadLine(const string& title, const string refLines[], int refCount, const string labels[], string values[], int fieldCount, int fieldIndex);
bool uiFormReadInt(const string& title, const string refLines[], int refCount, const string labels[], string values[], int fieldCount, int fieldIndex, int& out);

// Chay giao dien chinh (goi cac handler CLI da co)
void uiRunApp(PTRCB &dscb, TreeHK &dshk);
