#include "CTDL.h"
#include "maybay.h"
#include <iostream>
#include <cstring>
using namespace std;

// Thêm các file cài đặt (giữ như trước)
#include "data.cpp"
#include "maybay.cpp"
#include "danhsachmaybay.cpp"
#include "kiemtrasohieu.cpp"
#include "chuyenbay.cpp"
#include "ve.cpp"
#include "hienthi.cpp"
#include "hanhkhach.cpp"
#include "thongke.cpp"

// Cross-platform helper: use termios on POSIX, _getch on Windows
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
static bool win_raw_mode = false;
void enableRawMode() {
    // _getch on Windows reads a single char without echo, so no global raw-mode needed.
    // Optionally we could disable ENABLE_ECHO_INPUT using console APIs; keep no-op for simplicity.
    win_raw_mode = true;
}
void disableRawMode() {
    win_raw_mode = false;
}
int getch_block() {
    int c = _getch(); // returns int; does not echo
    return c;
}
#else
#include <termios.h>
#include <unistd.h>
static struct termios orig_termios;
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
int getch_block() {
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) == 1) return c;
    return -1;
}
#endif

// Nhập DateTime tương tác theo mẫu "hh:mm dd/mm/yyyy"
// Hiển thị mẫu, khi gõ số sẽ thay thế ký tự mẫu tương ứng; hỗ trợ backspace; Enter hoàn tất khi đủ chữ số
void InputDateTimeInteractive(DateTime &dt) {
    const char tmpl[] = "hh:mm dd/mm/yyyy";
    char buf[32];
    strncpy(buf, tmpl, sizeof(buf));
    const int digitPos[] = {0,1,3,4,6,7,9,10,12,13,14,15}; // positions of digits in buf
    const int totalDigits = sizeof(digitPos)/sizeof(digitPos[0]);
    int pos = 0;

    enableRawMode();
    // in mẫu lần đầu
    cout << buf << flush;

    while (true) {
        int c = getch_block();
        if (c == -1) continue;
        // On Windows _getch() returns '\r' for Enter; normalize both
        if (c == 3) { // Ctrl-C
            disableRawMode();
            exit(0);
        }
        if ((c == '\r' || c == '\n')) {
            if (pos == totalDigits) break; // only accept enter when full
            else continue;
        }
        // backspace codes: 127 (DEL), 8 (BS)
        if (c == 127 || c == 8) { // backspace
            if (pos > 0) {
                pos--;
                // restore template char based on position
                int idx = digitPos[pos];
                char restore;
                if (idx == 0 || idx == 1) restore = 'h';
                else if (idx == 3 || idx == 4) restore = 'm';
                else if (idx == 6 || idx == 7) restore = 'd';
                else if (idx == 9 || idx == 10) restore = 'M';
                else restore = 'y';
                buf[digitPos[pos]] = restore;
            }
        } else if (c >= '0' && c <= '9') {
            if (pos < totalDigits) {
                buf[digitPos[pos]] = (char)c;
                pos++;
            }
        } else {
            // ignore other keys
        }
        // redraw line: carriage return then print buffer
        cout << "\r" << buf << " " << flush;
    }

    // parse into dt (assume all digits entered)
    auto digitAt = [&](int idx)->int { return buf[digitPos[idx]] - '0'; };
    dt.gio  = digitAt(0)*10 + digitAt(1);
    dt.phut = digitAt(2)*10 + digitAt(3);
    dt.ngay = digitAt(4)*10 + digitAt(5);
    dt.thang= digitAt(6)*10 + digitAt(7);
    dt.nam  = digitAt(8)*1000 + digitAt(9)*100 + digitAt(10)*10 + digitAt(11);

    cout << "\n";
    disableRawMode();
}

// --- NEW: CLI handlers extracted from previous main loop ---
// Each handler uses std::cin / std::cout (CLI). TUI will call endwin(), invoke handler, then restart ncurses.

static DSMayBay* _dsmb_ptr = &dsmb; // convenience if needed

void handleMayBayCLI() {
	int m;
	do {
		cout << "\n--- QUAN LY MAY BAY ---\n";
		cout << "1. Them may bay\n";
		cout << "2. Xoa may bay\n";
		cout << "3. Sua may bay\n";
		cout << "4. Hien thi danh sach\n";
		cout << "5. Back\n";
		cout << "Chon (1-5): ";
		cin >> m;
		switch (m) {
			case 1: ThemMayBay(); break;
			case 2: XoaMayBay(); break;
			case 3: SuaMayBay(); break;
			case 4: HienThi(); break;
			case 5: break;
			default: cout << "Lua chon khong hop le\n";
		}
	} while (m != 5);
}

void handleChuyenBayCLI(PTRCB &dscb, TreeHK &dshk) {
	int m;
	do {
		cout << "\n--- QUAN LY CHUYEN BAY ---\n";
		cout << "1. Lap chuyen bay moi\n";
		cout << "2. Sua ngay gio chuyen bay\n";
		cout << "3. Huy chuyen bay\n";
		cout << "4. Back\n";
		cout << "Chon (1-4): ";
		cin >> m;
		if (m == 1) {
			ChuyenBay cb;
			cout << "Nhap ma chuyen bay: ";
			cin >> cb.MACB;
			if (TimChuyenBay(dscb, cb.MACB) != NULL) {
				cout << "Ma chuyen bay da ton tai!\n";
				continue;
			}
			cout << "Nhap ngay thang nam gio phut: ";
			InputDateTimeInteractive(cb.TGKHOIHANH);
			cout << "Nhap noi den: ";
			cin.ignore();
			cin.getline(cb.SANBAYDEN, 41);
			cout << "Nhap so hieu may bay (thuoc ds may bay): ";
			string soHieu;
			getline(cin, soHieu);
			int idx = TimMayBay(soHieu);
			if (idx == -1) {
				cout << "Khong ton tai may bay voi so hieu tren!\n";
				continue;
			}
			strncpy(cb.SOHIEUMB, dsmb.nodes[idx]->SOHIEU, 15); cb.SOHIEUMB[15] = '\0';
			cb.SOCHO = dsmb.nodes[idx]->SOCHO;
			cb.TRANGTHAI = 1;
			cb.DSVE.ds = NULL; cb.DSVE.soLuongVe = 0;
			KhoiTaoVe(cb);
			ThemChuyenBay(dscb, cb);
			cout << "Them chuyen bay thanh cong!\n";
		} else if (m == 2) {
			char ma[16];
			cout << "Nhap ma chuyen bay: ";
			cin >> ma;
			PTRCB p = TimChuyenBay(dscb, ma);
			if (!p) { cout << "Khong tim thay!\n"; continue; }
			DateTime tg;
			cout << "Nhap ngay thang nam gio phut moi: ";
			InputDateTimeInteractive(tg);
			SuaNgayGioChuyenBay(dscb, ma, tg);
			cout << "Cap nhat thanh cong.\n";
		} else if (m == 3) {
			char ma[16];
			cout << "Nhap ma chuyen bay can huy: ";
			cin >> ma;
			PTRCB p = TimChuyenBay(dscb, ma);
			if (!p) { cout << "Khong tim thay!\n"; continue; }
			HuyChuyenBay(dscb, ma);
			p->cb.TRANGTHAI = 0;
			cout << "Da huy chuyen bay.\n";
		} else if (m == 4) {
			break;
		} else {
			cout << "Lua chon khong hop le\n";
		}
	} while (true);
}

void handleDatVeCLI(PTRCB &dscb, TreeHK &dshk) {
	char ma[16], cmnd[16];
	int soVe;
	cout << "Nhap ma chuyen bay: ";
	cin >> ma;
	PTRCB p = TimChuyenBay(dscb, ma);
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }
	cout << "Nhap so ve (1.." << p->cb.SOCHO << "): ";
	cin >> soVe;
	cout << "Nhap CMND: ";
	cin >> cmnd;
	nodeHK* found = TimHanhKhach(dshk, cmnd);
	if (found == NULL) {
		cout << "Hanh khach chua co. Nhap thong tin:\n";
		HanhKhach hk;
		strncpy(hk.SOCMND, cmnd, 15); hk.SOCMND[15] = '\0';
		cout << "Ho: "; cin.ignore(); cin.getline(hk.HO, 51);
		cout << "Ten: "; cin.getline(hk.TEN, 11);
		cout << "Phai (Nam/Nu): "; cin.getline(hk.PHAI, 4);
		ThemHanhKhach(dshk, hk);
		cout << "Da them hanh khach.\n";
	} else {
		cout << "Thong tin hanh khach:\n";
		cout << found->hk.SOCMND << " " << found->hk.HO << " " << found->hk.TEN << " " << found->hk.PHAI << endl;
	}
	DatVe(dscb, ma, soVe, cmnd);
}

void handleHuyVeCLI(PTRCB &dscb) {
	char ma[16], cmnd[16];
	cout << "Nhap ma chuyen bay: ";
	cin >> ma;
	PTRCB p = TimChuyenBay(dscb, ma);
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }
	cout << "Nhap CMND: ";
	cin >> cmnd;
	bool ok = false;
	if (p->cb.DSVE.ds == NULL) { cout << "Chua khoi tao ve!\n"; return; }
	for (int i = 0; i < p->cb.DSVE.soLuongVe; i++) {
		if (strcmp(p->cb.DSVE.ds[i].SOCMND, cmnd) == 0) {
			p->cb.DSVE.ds[i].SOCMND[0] = '\0';
			CapNhatTrangThai(p->cb);
			ok = true;
			break;
		}
	}
	cout << (ok ? "Huy ve thanh cong\n" : "Khong tim thay CMND tren chuyen bay\n");
}

void handleInDanhSachCLI(PTRCB &dscb, TreeHK &dshk) {
	char ma[16];
	cout << "Nhap ma chuyen bay: ";
	cin >> ma;
	PTRCB p = TimChuyenBay(dscb, ma);
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }
	// print header+list (same as before)
	cout << "DANH SACH HANH KHACH THUOC CHUYEN BAY " << p->cb.MACB << "\n";
	cout << "Ngay gio: "
		 << (p->cb.TGKHOIHANH.ngay < 10 ? "0" : "") << p->cb.TGKHOIHANH.ngay << "/"
		 << (p->cb.TGKHOIHANH.thang < 10 ? "0" : "") << p->cb.TGKHOIHANH.thang << "/"
		 << p->cb.TGKHOIHANH.nam << " "
		 << (p->cb.TGKHOIHANH.gio < 10 ? "0" : "") << p->cb.TGKHOIHANH.gio << ":"
		 << (p->cb.TGKHOIHANH.phut < 10 ? "0" : "") << p->cb.TGKHOIHANH.phut << "\n";
	cout << "Noi den: " << p->cb.SANBAYDEN << "\n";
	cout << "STT\tSO VE\tSO CMND\t\tHO TEN\t\tPHAI\n";
	if (p->cb.DSVE.ds == NULL) { cout << "Chua co danh sach ve!\n"; return; }
	int stt = 1;
	for (int i = 0; i < p->cb.DSVE.soLuongVe; i++) {
		if (p->cb.DSVE.ds[i].SOCMND[0] != '\0') {
			nodeHK* hk = TimHanhKhach(dshk, p->cb.DSVE.ds[i].SOCMND);
			if (hk) {
				cout << stt++ << "\t" << p->cb.DSVE.ds[i].SOVE << "\t" << hk->hk.SOCMND << "\t" << hk->hk.HO << " " << hk->hk.TEN << "\t" << hk->hk.PHAI << "\n";
			} else {
				cout << stt++ << "\t" << p->cb.DSVE.ds[i].SOVE << "\t" << p->cb.DSVE.ds[i].SOCMND << "\n";
			}
		}
	}
}

void handleInChuyenBayTheoNgayCLI(PTRCB &dscb) {
	int ngay, thang, nam;
	char noiDen[41];
	cout << "Nhap ngay thang nam: ";
	cin >> ngay >> thang >> nam;
	cout << "Nhap noi den: ";
	cin.ignore();
	cin.getline(noiDen, 41);
	InChuyenBayTheoNgayVaNoiDen(dscb, ngay, thang, nam, noiDen);
}

void handleInVeConTrongCLI(PTRCB &dscb) {
	char ma[16];
	cout << "Nhap ma chuyen bay: ";
	cin >> ma;
	InVeConTrong(dscb, ma);
}

void handleThongKeCLI(PTRCB &dscb) {
	if (dsmb.n == 0) { cout << "Chua co may bay trong danh sach!\n"; return; }
	TK* kq = new TK[dsmb.n];
	ThongKeSoLuotBay(dscb, dsmb, kq);
	SapXepGiamDanTheoSoLuot(kq, dsmb.n);
	cout << "THONG KE SO LUOT BAY (giam dan):\n";
	for (int i = 0; i < dsmb.n; i++) {
		cout << kq[i].SOHIEU << " : " << kq[i].soLuot << endl;
	}
	delete[] kq;
}

// --- END handlers ---

// --- NEW: ncurses TUI main ---
#ifdef USE_NCURSES
#include <ncurses.h>

void runTui(PTRCB &dscb, TreeHK &dshk) {
	initscr(); noecho(); cbreak(); keypad(stdscr, TRUE); curs_set(0);
	const char *choices[] = {
		"Quan ly May Bay (Them/Xoa/Sua/Hien thi)",
		"Quan ly Chuyen Bay (Lap moi / Sua ngay gio / Huy)",
		"Dat ve",
		"Huy ve",
		"In danh sach hanh khach cua chuyen bay",
		"In chuyen bay theo ngay va noi den (co ve)",
		"In danh sach ve con trong cua chuyen bay",
		"Thong ke so luot bay cua may bay (giam dan)",
		"Thoat"
	};
	int n_choices = sizeof(choices)/sizeof(choices[0]);
	int highlight = 0, ch;
	while (1) {
		clear();
		mvprintw(0, 2, "=== QUAN LY MAY BAY - TUI ===");
		for (int i = 0; i < n_choices; ++i) {
			if (i == highlight) attron(A_REVERSE);
			mvprintw(2 + i, 4, "%s", choices[i]);
			if (i == highlight) attroff(A_REVERSE);
		}
		mvprintw(LINES-2, 2, "Use arrow keys to navigate, Enter to select, q to quit.");
		refresh();
		ch = getch();
		if (ch == KEY_UP) { if (highlight > 0) highlight--; }
		else if (ch == KEY_DOWN) { if (highlight < n_choices-1) highlight++; }
		else if (ch == '\n' || ch == KEY_ENTER) {
			endwin();
			switch (highlight) {
				case 0: handleMayBayCLI(); break;
				case 1: handleChuyenBayCLI(dscb, dshk); break;
				case 2: handleDatVeCLI(dscb, dshk); break;
				case 3: handleHuyVeCLI(dscb); break;
				case 4: handleInDanhSachCLI(dscb, dshk); break;
				case 5: handleInChuyenBayTheoNgayCLI(dscb); break;
				case 6: handleInVeConTrongCLI(dscb); break;
				case 7: handleThongKeCLI(dscb); break;
				case 8: cout << "Thoat.\n"; return;
			}
			cout << "\nNhan Enter de tro ve TUI...";
			cin.ignore(); cin.get();
			initscr(); noecho(); cbreak(); keypad(stdscr, TRUE); curs_set(0);
		} else if (ch == 'q' || ch == 'Q') { break; }
	}
	endwin();
}

#else // fallback numeric menu to avoid ncurses linker errors

void runTui(PTRCB &dscb, TreeHK &dshk) {
	// simple numeric menu loop that calls CLI handlers directly
	while (1) {
		cout << "\n===== HE THONG QUAN LY (FALLBACK MENU) =====\n";
		cout << "1. Quan ly May Bay (Them/Xoa/Sua/Hien thi)\n";
		cout << "2. Quan ly Chuyen Bay (Lap moi / Sua ngay gio / Huy)\n";
		cout << "3. Dat ve\n";
		cout << "4. Huy ve\n";
		cout << "5. In danh sach hanh khach cua chuyen bay\n";
		cout << "6. In chuyen bay theo ngay va noi den (co ve)\n";
		cout << "7. In danh sach ve con trong cua chuyen bay\n";
		cout << "8. Thong ke so luot bay cua may bay (giam dan)\n";
		cout << "9. Thoat\n";
		cout << "Lua chon (1-9): ";
		int choice; if (!(cin >> choice)) { cin.clear(); cin.ignore(10000,'\n'); continue; }
		switch (choice) {
			case 1: handleMayBayCLI(); break;
			case 2: handleChuyenBayCLI(dscb, dshk); break;
			case 3: handleDatVeCLI(dscb, dshk); break;
			case 4: handleHuyVeCLI(dscb); break;
			case 5: handleInDanhSachCLI(dscb, dshk); break;
			case 6: handleInChuyenBayTheoNgayCLI(dscb); break;
			case 7: handleInVeConTrongCLI(dscb); break;
			case 8: handleThongKeCLI(dscb); break;
			case 9: cout << "Thoat.\n"; return;
			default: cout << "Lua chon khong hop le.\n"; break;
		}
	}
}

#endif

// Save all data to files
void SaveAll(PTRCB head, TreeHK root) {
    // maybay
    ofstream fm("maybay.txt");
    if (fm.is_open()) {
        LuuMayBayFile(dsmb, fm);
        fm.close();
        cout << "Da luu maybay.txt\n";
    } else {
        cerr << "Khong the mo maybay.txt de ghi\n";
    }

    // chuyenbay
    ofstream fc("chuyenbay.txt");
    if (fc.is_open()) {
        LuuChuyenBayFile(head, fc);
        fc.close();
        cout << "Da luu chuyenbay.txt\n";
    } else {
        cerr << "Khong the mo chuyenbay.txt de ghi\n";
    }

    // hanhkhach
    ofstream fh("hanhkhach.txt");
    if (fh.is_open()) {
        LuuHanhKhachFile(root, fh);
        fh.close();
        cout << "Da luu hanhkhach.txt\n";
    } else {
        cerr << "Khong the mo hanhkhach.txt de ghi\n";
    }
}

// Load all data from files
void LoadAll(PTRCB &head, TreeHK &root) {
    // maybay
    // clear existing maybay list to avoid leaks
    XoaToanBo();
    ifstream fm("maybay.txt");
    if (fm.is_open()) {
        DocMayBayFile(dsmb, fm);
        fm.close();
        cout << "Da doc maybay.txt\n";
    } // else: no file yet, skip

    // chuyenbay
    ifstream fc("chuyenbay.txt");
    if (fc.is_open()) {
        DocChuyenBayFile(head, fc);
        fc.close();
        cout << "Da doc chuyenbay.txt\n";
    }

    // hanhkhach
    ifstream fh("hanhkhach.txt");
    if (fh.is_open()) {
        DocHanhKhachFile(root, fh);
        fh.close();
        cout << "Da doc hanhkhach.txt\n";
    }
}

int main() {
    dsmb.n = 0;
    PTRCB dscb = NULL;
    TreeHK dshk = NULL;

    // load existing data if có
    LoadAll(dscb, dshk);

    runTui(dscb, dshk);

    // save before exit
    SaveAll(dscb, dshk);

    return 0;
}