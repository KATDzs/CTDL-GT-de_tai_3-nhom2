#define _HAS_STD_BYTE 0
#include "CTDL.h"
#include "maybay.h"
#include "ui.h"
#include <iostream>
#include <cstring>
#include <vector>
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
#include "ui.cpp"

// Nhap ngay gio; tra ve false neu nguoi dung huy
bool InputDateTimeInteractive(DateTime &dt) {
    if (!uiReadInt("Nhap gio (hh):", dt.gio)) return false;
    if (!uiReadInt("Nhap phut (mm):", dt.phut)) return false;
    if (!uiReadInt("Nhap ngay (dd):", dt.ngay)) return false;
    if (!uiReadInt("Nhap thang (mm):", dt.thang)) return false;
    if (!uiReadInt("Nhap nam (yyyy):", dt.nam)) return false;
    return true;
}

// --- NEW: CLI handlers extracted from previous main loop ---
// Each handler uses std::cin / std::cout (CLI). TUI will call endwin(), invoke handler, then restart ncurses.

static DSMayBay* _dsmb_ptr = &dsmb; // convenience if needed

void handleMayBayCLI() {
	const vector<string> items = {
		"Them may bay",
		"Xoa may bay",
		"Sua may bay",
		"Hien thi danh sach",
		"Quay lai"
	};
	int m;
	do {
		m = uiMenu("QUAN LY MAY BAY", items, "Esc / q: quay lai menu chinh");
		if (m == -1) break;
		switch (m) {
			case 0: ThemMayBay(); break;
			case 1: XoaMayBay(); break;
			case 2: SuaMayBay(); break;
			case 3: HienThi(); break;
			case 4: break;
			default: cout << "Lua chon khong hop le\n";
		}
		if (m >= 0 && m < 4) {
			if (uiConsumeInputCancel()) continue;
			uiPause();
		}
		if (uiConsumeInputCancel()) continue;
	} while (m != 4 && m != -1);
}

void handleChuyenBayCLI(PTRCB &dscb, TreeHK &dshk) {
	const vector<string> items = {
		"Lap chuyen bay moi",
		"Sua ngay gio chuyen bay",
		"Huy chuyen bay",
		"Quay lai"
	};
	int m;
	do {
		m = uiMenu("QUAN LY CHUYEN BAY", items, "Esc / q: quay lai menu chinh");
		if (m == -1) break;
		if (m == 0) {
			ChuyenBay cb;
			string maCB;
			if (!uiReadWord("Nhap ma chuyen bay:", maCB)) continue;
			strncpy(cb.MACB, maCB.c_str(), 15); cb.MACB[15] = '\0';
			if (TimChuyenBay(dscb, cb.MACB) != NULL) {
				cout << "Ma chuyen bay da ton tai!\n";
				uiPause(); continue;
			}
			if (!InputDateTimeInteractive(cb.TGKHOIHANH)) continue;
			string noiDen;
			if (!uiReadLine("Nhap noi den:", noiDen)) continue;
			strncpy(cb.SANBAYDEN, noiDen.c_str(), 40); cb.SANBAYDEN[40] = '\0';
			string soHieu;
			if (!uiReadLine("Nhap so hieu may bay (thuoc ds may bay):", soHieu)) continue;
			int idx = TimMayBay(soHieu);
			if (idx == -1) {
				cout << "Khong ton tai may bay voi so hieu tren!\n";
				uiPause(); continue;
			}
			strncpy(cb.SOHIEUMB, dsmb.nodes[idx]->SOHIEU, 15); cb.SOHIEUMB[15] = '\0';
			cb.SOCHO = dsmb.nodes[idx]->SOCHO;
			cb.TRANGTHAI = 1;
			cb.DSVE.ds = NULL; cb.DSVE.soLuongVe = 0;
			KhoiTaoVe(cb);
			ThemChuyenBay(dscb, cb);
			cout << "Them chuyen bay thanh cong!\n";
			uiPause();
		} else if (m == 1) {
			string ma;
			if (!uiReadWord("Nhap ma chuyen bay:", ma)) continue;
			char maBuf[16];
			strncpy(maBuf, ma.c_str(), 15); maBuf[15] = '\0';
			PTRCB p = TimChuyenBay(dscb, maBuf);
			if (!p) { cout << "Khong tim thay!\n"; uiPause(); continue; }
			DateTime tg;
			if (!InputDateTimeInteractive(tg)) continue;
			SuaNgayGioChuyenBay(dscb, maBuf, tg);
			cout << "Cap nhat thanh cong.\n";
			uiPause();
		} else if (m == 2) {
			string ma;
			if (!uiReadWord("Nhap ma chuyen bay can huy:", ma)) continue;
			char maBuf[16];
			strncpy(maBuf, ma.c_str(), 15); maBuf[15] = '\0';
			PTRCB p = TimChuyenBay(dscb, maBuf);
			if (!p) { cout << "Khong tim thay!\n"; uiPause(); continue; }
			HuyChuyenBay(dscb, maBuf);
			p->cb.TRANGTHAI = 0;
			cout << "Da huy chuyen bay.\n";
			uiPause();
		} else if (m == 3) {
			break;
		}
	} while (m != 3 && m != -1);
}

void handleDatVeCLI(PTRCB &dscb, TreeHK &dshk) {
	string ma, cmnd;
	int soVe;
	if (!uiReadWord("Nhap ma chuyen bay:", ma)) return;
	char maBuf[16];
	strncpy(maBuf, ma.c_str(), 15); maBuf[15] = '\0';
	PTRCB p = TimChuyenBay(dscb, maBuf);
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }
	if (!uiReadInt("Nhap so ve (1.." + to_string(p->cb.SOCHO) + "):", soVe)) return;
	if (!uiReadWord("Nhap CMND:", cmnd)) return;
	char cmndBuf[16];
	strncpy(cmndBuf, cmnd.c_str(), 15); cmndBuf[15] = '\0';
	nodeHK* found = TimHanhKhach(dshk, cmndBuf);
	if (found == NULL) {
		cout << "Hanh khach chua co. Nhap thong tin:\n";
		HanhKhach hk;
		strncpy(hk.SOCMND, cmndBuf, 15); hk.SOCMND[15] = '\0';
		string ho, ten, phai;
		if (!uiReadLine("Ho:", ho)) return;
		if (!uiReadLine("Ten:", ten)) return;
		if (!uiReadLine("Phai (Nam/Nu):", phai)) return;
		strncpy(hk.HO, ho.c_str(), 50); hk.HO[50] = '\0';
		strncpy(hk.TEN, ten.c_str(), 10); hk.TEN[10] = '\0';
		strncpy(hk.PHAI, phai.c_str(), 3); hk.PHAI[3] = '\0';
		ThemHanhKhach(dshk, hk);
		cout << "Da them hanh khach.\n";
	} else {
		cout << "Thong tin hanh khach:\n";
		cout << found->hk.SOCMND << " " << found->hk.HO << " " << found->hk.TEN << " " << found->hk.PHAI << endl;
	}
	DatVe(dscb, maBuf, soVe, cmndBuf);
}

void handleHuyVeCLI(PTRCB &dscb) {
	string ma, cmnd;
	if (!uiReadWord("Nhap ma chuyen bay:", ma)) return;
	char maBuf[16];
	strncpy(maBuf, ma.c_str(), 15); maBuf[15] = '\0';
	PTRCB p = TimChuyenBay(dscb, maBuf);
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }
	if (!uiReadWord("Nhap CMND:", cmnd)) return;
	char cmndBuf[16];
	strncpy(cmndBuf, cmnd.c_str(), 15); cmndBuf[15] = '\0';
	bool ok = false;
	if (p->cb.DSVE.ds == NULL) { cout << "Chua khoi tao ve!\n"; return; }
	for (int i = 0; i < p->cb.DSVE.soLuongVe; i++) {
		if (strcmp(p->cb.DSVE.ds[i].SOCMND, cmndBuf) == 0) {
			p->cb.DSVE.ds[i].SOCMND[0] = '\0';
			CapNhatTrangThai(p->cb);
			ok = true;
			break;
		}
	}
	cout << (ok ? "Huy ve thanh cong\n" : "Khong tim thay CMND tren chuyen bay\n");
}

void handleInDanhSachCLI(PTRCB &dscb, TreeHK &dshk) {
	string ma;
	if (!uiReadWord("Nhap ma chuyen bay:", ma)) return;
	char maBuf[16];
	strncpy(maBuf, ma.c_str(), 15); maBuf[15] = '\0';
	PTRCB p = TimChuyenBay(dscb, maBuf);
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
	string noiDen;
	if (!uiReadInt("Nhap ngay:", ngay)) return;
	if (!uiReadInt("Nhap thang:", thang)) return;
	if (!uiReadInt("Nhap nam:", nam)) return;
	if (!uiReadLine("Nhap noi den:", noiDen)) return;
	char noiDenBuf[41];
	strncpy(noiDenBuf, noiDen.c_str(), 40); noiDenBuf[40] = '\0';
	InChuyenBayTheoNgayVaNoiDen(dscb, ngay, thang, nam, noiDenBuf);
}

void handleInVeConTrongCLI(PTRCB &dscb) {
	string ma;
	if (!uiReadWord("Nhap ma chuyen bay:", ma)) return;
	char maBuf[16];
	strncpy(maBuf, ma.c_str(), 15); maBuf[15] = '\0';
	InVeConTrong(dscb, maBuf);
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

    LoadAll(dscb, dshk);

    uiRunApp(dscb, dshk);

    uiClear();
    uiDrawBoxTop("HE THONG QUAN LY MAY BAY", 64);
    uiDrawBoxLine("Dang luu du lieu...", 64);
    uiDrawBoxBottom(64);
    cout << "\n";

    SaveAll(dscb, dshk);
    cout << "Tam biet!\n";

    return 0;
}