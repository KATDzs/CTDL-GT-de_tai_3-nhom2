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
    // Make reads return immediately or after a short timeout so that
    // pressing Esc alone doesn't block waiting for additional bytes.
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 10; // tenths of a second (1 second for macOS arrow keys)
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
#ifndef _WIN32
    uiDisableRaw(); // ensure raw mode disabled on UNIX/mac before printing/returning
#endif
    uiInputCancelled = true;
    cout << "Da huy thao tac.\n";
}

static void uiFlushInput() {
#ifdef _WIN32
    while (_kbhit()) (void)_getch();
#else
    tcflush(STDIN_FILENO, TCIFLUSH);
#endif
}

static bool uiReadChars(const string& prompt, string& out, bool allowSpaces, bool showCancelHint = false) {
    // Use canonical (getline) input for form fields to avoid raw-mode issues
    // that can make numeric keys / IME sequences invisible on some mac terminals.
    uiFlushInput();
#ifndef _WIN32
    // Ensure terminal is in canonical (cooked) mode so line editing/echo work normally.
    uiDisableRaw();
#endif
    // Print prompt; optionally suppress the "(Esc/q: huy)" hint when caller requests.
    cout << prompt;
    if (showCancelHint) cout << " (Esc/q: huy)";
    cout << " ";
    out.clear();

    string line;
    if (!std::getline(cin, line)) {
        // input error / EOF
        uiInputCancelled = true;
        return false;
    }

    // If user typed only 'q' or 'Q' treat as cancel (backwards-compatible)
    if ((line == "q" || line == "Q") && line.length() == 1) {
        uiCancelInput();
        return false;
    }

    if (!allowSpaces) {
        // take first token only
        size_t pos = line.find_first_of(" \t");
        if (pos != string::npos) line = line.substr(0, pos);
    }

    out = line;
    return true;
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

static int uiCalcBoxWidth(const string lines[], int lineCount, int minWidth = UI_DEFAULT_BOX_WIDTH) {
    return UI_DEFAULT_BOX_WIDTH;
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
    string display = text;
    if ((int)display.length() > maxLen) {
        if (maxLen > 3) display = display.substr(0, (size_t)maxLen - 3) + "...";
        else display = display.substr(0, (size_t)maxLen);
    }
    cout << "| " << uiPadRight(display, maxLen) << " |\n";
}

void uiDrawBoxSeparator(int width) {
    cout << "+" << string(width - 2, '-') << "+\n";
}

void uiShowTaskScreen(const string& title, const string& subtitle) {
    uiClear();
    uiDrawBoxTop(title);
    if (!subtitle.empty()) {
        uiDrawBoxLine(subtitle);
    }
    uiDrawBoxBottom();
    cout << "\n";
}

void uiShowDataBox(const string& title, const string lines[], int lineCount) {
    if (lines == NULL || lineCount <= 0) return;

    const int width = UI_DEFAULT_BOX_WIDTH;
    uiClear();
    uiDrawBoxTop(title, width);
    int showCount = lineCount;
    if (showCount > UI_REF_DISPLAY_MAX + 2) showCount = UI_REF_DISPLAY_MAX + 2;
    for (int i = 0; i < showCount; i++) {
        uiDrawBoxLine(lines[i], width);
    }
    if (lineCount > showCount) {
        uiDrawBoxLine("  ... va " + to_string(lineCount - showCount) + " dong khac", width);
    }
    uiDrawBoxBottom(width);
    cout << "\n";
}

static string uiFormFieldLine(const string& label, const string& value, bool active) {
    string line = active ? "> " : "  ";
    line += label + ": ";
    if (!value.empty()) {
        line += value;
    } else if (active) {
        line += "_";
    }
    return line;
}

void uiShowFormScreen(const string& title, const string refLines[], int refCount, const string labels[], const string values[], int fieldCount, int activeIndex, const string& footer) {
    if (labels == NULL || values == NULL || fieldCount <= 0) return;
    if (fieldCount > UI_FORM_MAX_FIELDS) fieldCount = UI_FORM_MAX_FIELDS;
    if (refCount > UI_REF_MAX_LINES) refCount = UI_REF_MAX_LINES;

    const int width = UI_DEFAULT_BOX_WIDTH;
    uiClear();
    uiDrawBoxTop(title, width);

    if (refLines != NULL && refCount > 0) {
        int showCount = refCount;
        if (showCount > UI_REF_DISPLAY_MAX) showCount = UI_REF_DISPLAY_MAX;
        for (int i = 0; i < showCount; i++) {
            uiDrawBoxLine(refLines[i], width);
        }
        if (refCount > UI_REF_DISPLAY_MAX) {
            uiDrawBoxLine("  ... va " + to_string(refCount - UI_REF_DISPLAY_MAX) + " dong tham khao khac", width);
        }
        uiDrawBoxSeparator(width);
    }

    for (int i = 0; i < fieldCount; i++) {
        uiDrawBoxLine(uiFormFieldLine(labels[i], values[i], i == activeIndex), width);
    }

    if (!footer.empty()) {
        uiDrawBoxSeparator(width);
        uiDrawBoxLine(footer, width);
    }
    uiDrawBoxBottom(width);
    cout << "\n";
}

bool uiFormReadWord(const string& title, const string refLines[], int refCount, const string labels[], string values[], int fieldCount, int fieldIndex) {
    uiShowFormScreen(title, refLines, refCount, labels, values, fieldCount, fieldIndex);
    string prompt = "> " + labels[fieldIndex];
    if (!uiReadWord(prompt, values[fieldIndex])) return false;
    return true;
}

bool uiFormReadLine(const string& title, const string refLines[], int refCount, const string labels[], string values[], int fieldCount, int fieldIndex) {
    uiShowFormScreen(title, refLines, refCount, labels, values, fieldCount, fieldIndex);
    string prompt = "> " + labels[fieldIndex];
    if (!uiReadLine(prompt, values[fieldIndex])) return false;
    return true;
}

bool uiFormReadInt(const string& title, const string refLines[], int refCount, const string labels[], string values[], int fieldCount, int fieldIndex, int& out) {
    while (true) {
        if (!uiFormReadWord(title, refLines, refCount, labels, values, fieldCount, fieldIndex)) return false;

        const string& s = values[fieldIndex];
        if (s.empty()) {
            cout << "Loi: Vui long nhap so!\n";
            values[fieldIndex].clear();
            continue;
        }

        bool ok = true;
        for (char c : s) {
            if (!isdigit((unsigned char)c)) { ok = false; break; }
        }
        if (!ok) {
            cout << "Loi: Vui long nhap so!\n";
            values[fieldIndex].clear();
            continue;
        }

        out = stoi(s);
        return true;
    }
}

static void uiDrawBoxLines(const string lines[], int lineCount, int width) {
    for (int i = 0; i < lineCount; i++)
        uiDrawBoxLine(lines[i], width);
}

void uiPause(const string& msg) {
#ifndef _WIN32
    uiDisableRaw();
#endif
    cout << "\n" << (msg.empty() ? "Nhan Enter de tiep tuc..." : msg);
    uiFlushInput();
    while (true) {
        int c = uiGetch();
        if (c == '\r' || c == '\n') break;
    }
}

// --- menu ---
int uiMenu(const string& title, const string items[], int itemCount, const string& footer) {
    if (items == NULL || itemCount <= 0) return -1;

    int highlight = 0;
    string hints[2];
    int hintCount;
    if (footer.empty()) {
        hints[0] = "Mui ten: chon | Enter: xac nhan | So 1-9: chon nhanh";
        hints[1] = "q / Esc: thoat";
        hintCount = 2;
    } else {
        hints[0] = footer;
        hintCount = 1;
    }

    string* measure = new string[itemCount + hintCount];
    for (int i = 0; i < itemCount; i++)
        measure[i] = "> " + to_string(i + 1) + ". " + items[i];
    for (int i = 0; i < hintCount; i++)
        measure[itemCount + i] = hints[i];
    const int width = uiCalcBoxWidth(measure, itemCount + hintCount);
    delete[] measure;

    while (true) {
        uiFlushInput();
        uiClear();
        uiDrawBoxTop(title, width);

        for (int i = 0; i < itemCount; i++) {
            string line = (i == highlight)
                ? "> " + to_string(i + 1) + ". " + items[i]
                : "  " + to_string(i + 1) + ". " + items[i];
            uiDrawBoxLine(line, width);
        }

        uiDrawBoxSeparator(width);
        uiDrawBoxLines(hints, hintCount, width);
        uiDrawBoxBottom(width);

        int ch = uiGetch();

        if (ch == 1000) {
            if (highlight > 0) highlight--;
        } else if (ch == 1001) {
            if (highlight < itemCount - 1) highlight++;
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
            if (idx < itemCount) {
#ifndef _WIN32
                uiDisableRaw();
#endif
                return idx;
            }
        }
    }
}

// --- handlers ---
extern void handleMayBayCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleChuyenBayCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleDatVeCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleHuyVeCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleInDanhSachCLI(PTRCB &dscb, TreeHK &dshk);
extern void handleInChuyenBayTheoNgayCLI(PTRCB &dscb);
extern void handleInVeConTrongCLI(PTRCB &dscb);
extern void handleThongKeCLI(PTRCB &dscb);

void uiRunApp(PTRCB &dscb, TreeHK &dshk) {
    const string mainItems[] = {
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
    const int mainItemCount = sizeof(mainItems) / sizeof(mainItems[0]);

    uiResetCancel();

    while (true) {
        int choice = uiMenu("HE THONG QUAN LY MAY BAY", mainItems, mainItemCount);
        if (choice == -1 || choice == 8) break;

        switch (choice) {
            case 0: handleMayBayCLI(dscb, dshk); break;
            case 1: handleChuyenBayCLI(dscb, dshk); break;
            case 2: handleDatVeCLI(dscb, dshk); break;
            case 3: handleHuyVeCLI(dscb, dshk); break;
            case 4: handleInDanhSachCLI(dscb, dshk); break;
            case 5: handleInChuyenBayTheoNgayCLI(dscb); break;
            case 6: handleInVeConTrongCLI(dscb); break;
            case 7: handleThongKeCLI(dscb); break;
            default: break;
        }

        
    }
}


