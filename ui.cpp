#define _HAS_STD_BYTE 0
#include "ui.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

static bool uiInputCancelled = false;

void uiResetCancel() {
    uiInputCancelled = false;
}

bool uiConsumeInputCancel() {
    if (!uiInputCancelled) return false;
    uiInputCancelled = false;
    return true;
}

// --- doc phim ---
#ifdef _WIN32
static int uiGetch() {
    int c = _getch();
    if (c == 0 || c == 224) {
        int c2 = _getch();
        if (c2 == 72) return 1000;
        if (c2 == 80) return 1001;
        if (c2 == 75) return 1002;
        if (c2 == 77) return 1003;
        return c2;
    }
    return c;
}
#else
static struct termios ui_orig_termios;
static bool ui_raw = false;

static void uiEnableRaw() {
    if (ui_raw) return;
    tcgetattr(STDIN_FILENO, &ui_orig_termios);
    struct termios raw = ui_orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    ui_raw = true;
}
static void uiDisableRaw() {
    if (!ui_raw) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ui_orig_termios);
    ui_raw = false;
}
static int uiGetch() {
    uiEnableRaw();
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;
    if (c == 27) {
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return 27;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return 27;
        if (seq[0] == '[') {
            if (seq[1] == 'A') return 1000;
            if (seq[1] == 'B') return 1001;
            if (seq[1] == 'C') return 1002;
            if (seq[1] == 'D') return 1003;
        }
        return 27;
    }
    return c;
}
#endif

static void uiCancelInput() {
    uiInputCancelled = true;
    cout << "\nDa huy thao tac.\n";
}

static bool uiReadChars(const string& prompt, string& out, bool allowSpaces) {
    cout << prompt << " (Esc/q: huy) ";
    out.clear();

    while (true) {
        int c = uiGetch();
        if (c == -1) continue;

        if (c == 27 || ((c == 'q' || c == 'Q') && out.empty())) {
            uiCancelInput();
            return false;
        }
        if (c == '\r' || c == '\n') {
            cout << "\n";
            return true;
        }
        if (c == 127 || c == 8) {
            if (!out.empty()) {
                out.pop_back();
                cout << "\b \b";
            }
            continue;
        }
        if (c >= 32 && c <= 126) {
            if (!allowSpaces && c == ' ') continue;
            out += (char)c;
            cout << (char)c;
        }
    }
}

bool uiReadLine(const string& prompt, string& out) {
    return uiReadChars(prompt, out, true);
}

bool uiReadWord(const string& prompt, string& out) {
    return uiReadChars(prompt, out, false);
}

bool uiReadInt(const string& prompt, int& out) {
    while (true) {
        string s;
        if (!uiReadWord(prompt, s)) return false;
        if (s.empty()) {
            cout << "Loi: Vui long nhap so!\n";
            continue;
        }
        bool ok = true;
        for (char c : s) {
            if (!isdigit((unsigned char)c)) { ok = false; break; }
        }
        if (!ok) {
            cout << "Loi: Vui long nhap so!\n";
            continue;
        }
        out = stoi(s);
        return true;
    }
}

// --- ve khung ---
static string uiPadCenter(const string& s, int width) {
    if ((int)s.length() >= width) return s.substr(0, width);
    int pad = width - (int)s.length();
    int left = pad / 2;
    return string(left, ' ') + s + string(pad - left, ' ');
}

static string uiPadRight(const string& s, int width) {
    if ((int)s.length() >= width) return s.substr(0, width);
    return s + string(width - (int)s.length(), ' ');
}

static int uiCalcBoxWidth(const vector<string>& lines, int minWidth = 70) {
    int w = minWidth;
    for (const auto& line : lines) {
        int need = (int)line.length() + 4;
        if (need > w) w = need;
    }
    return w;
}

void uiClear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void uiDrawBoxTop(const string& title, int width) {
    cout << "+" << string(width - 2, '-') << "+\n";
    cout << "|" << uiPadCenter(title, width - 2) << "|\n";
    cout << "+" << string(width - 2, '-') << "+\n";
}

void uiDrawBoxBottom(int width) {
    cout << "+" << string(width - 2, '-') << "+\n";
}

void uiDrawBoxLine(const string& text, int width) {
    int maxLen = width - 4;
    if ((int)text.length() <= maxLen) {
        cout << "| " << uiPadRight(text, maxLen) << " |\n";
        return;
    }
    for (size_t i = 0; i < text.length(); i += (size_t)maxLen) {
        string part = text.substr(i, (size_t)maxLen);
        cout << "| " << uiPadRight(part, maxLen) << " |\n";
    }
}

void uiDrawBoxSeparator(int width) {
    cout << "+" << string(width - 2, '-') << "+\n";
}

static void uiDrawBoxLines(const vector<string>& lines, int width) {
    for (const auto& line : lines)
        uiDrawBoxLine(line, width);
}

void uiPause(const string& msg) {
#ifndef _WIN32
    uiDisableRaw();
#endif
    cout << "\n" << msg;
    cin.clear();
    cin.ignore(10000, '\n');
    cin.get();
}

// --- menu ---
int uiMenu(const string& title, const vector<string>& items, const string& footer) {
    if (items.empty()) return -1;

    int highlight = 0;
    const vector<string> hints = footer.empty()
        ? vector<string>{
              "Mui ten: chon | Enter: xac nhan | So 1-9: chon nhanh",
              "q / Esc: thoat"
          }
        : vector<string>{footer};

    vector<string> measure;
    for (int i = 0; i < (int)items.size(); i++)
        measure.push_back("> " + to_string(i + 1) + ". " + items[i]);
    for (const auto& h : hints) measure.push_back(h);
    const int width = uiCalcBoxWidth(measure);

    while (true) {
        uiClear();
        uiDrawBoxTop(title, width);

        for (int i = 0; i < (int)items.size(); i++) {
            string line = (i == highlight)
                ? "> " + to_string(i + 1) + ". " + items[i]
                : "  " + to_string(i + 1) + ". " + items[i];
            uiDrawBoxLine(line, width);
        }

        uiDrawBoxSeparator(width);
        uiDrawBoxLines(hints, width);
        uiDrawBoxBottom(width);

        int ch = uiGetch();

        if (ch == 1000) {
            if (highlight > 0) highlight--;
        } else if (ch == 1001) {
            if (highlight < (int)items.size() - 1) highlight++;
        } else if (ch == '\r' || ch == '\n') {
#ifndef _WIN32
            uiDisableRaw();
#endif
            return highlight;
        } else if (ch == 27 || ch == 'q' || ch == 'Q') {
#ifndef _WIN32
            uiDisableRaw();
#endif
            return -1;
        } else if (ch >= '1' && ch <= '9') {
            int idx = ch - '1';
            if (idx < (int)items.size()) {
#ifndef _WIN32
                uiDisableRaw();
#endif
                return idx;
            }
        }
    }
}

// --- handlers ---
extern void handleMayBayCLI();
extern void handleChuyenBayCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleDatVeCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleHuyVeCLI(PTRCB &dscb);
extern void handleInDanhSachCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleInChuyenBayTheoNgayCLI(PTRCB &dscb);
extern void handleInVeConTrongCLI(PTRCB &dscb);
extern void handleThongKeCLI(PTRCB &dscb);

void uiRunApp(PTRCB &dscb, TreeHK &dshk) {
    const vector<string> mainItems = {
        "Quan ly May Bay (Them / Xoa / Sua / Hien thi)",
        "Quan ly Chuyen Bay (Lap moi / Sua ngay gio / Huy)",
        "Dat ve",
        "Huy ve",
        "In danh sach hanh khach cua chuyen bay",
        "In chuyen bay theo ngay va noi den (co ve)",
        "In danh sach ve con trong cua chuyen bay",
        "Thong ke so luot bay cua may bay (giam dan)",
        "Thoat"
    };

    uiResetCancel();

    while (true) {
        int choice = uiMenu("HE THONG QUAN LY MAY BAY", mainItems);
        if (choice == -1 || choice == 8) break;

        uiClear();
        const int width = uiCalcBoxWidth({mainItems[choice]});
        uiDrawBoxTop("DANG THUC HIEN", width);
        uiDrawBoxLine(mainItems[choice], width);
        uiDrawBoxBottom(width);
        cout << "\n";

        switch (choice) {
            case 0: handleMayBayCLI(); break;
            case 1: handleChuyenBayCLI(dscb, dshk); break;
            case 2: handleDatVeCLI(dscb, dshk); break;
            case 3: handleHuyVeCLI(dscb); break;
            case 4: handleInDanhSachCLI(dscb, dshk); break;
            case 5: handleInChuyenBayTheoNgayCLI(dscb); break;
            case 6: handleInVeConTrongCLI(dscb); break;
            case 7: handleThongKeCLI(dscb); break;
            default: break;
        }

        if (uiConsumeInputCancel()) continue;
        uiPause();
    }
}
