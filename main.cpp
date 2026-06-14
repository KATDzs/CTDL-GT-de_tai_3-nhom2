#define _HAS_STD_BYTE 0
#include "CTDL.h"
#include "maybay.h"
#include "ui.h"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <sstream>
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

// Forward declarations
void SaveAll(PTRCB head, TreeHK root);
void LoadAll(PTRCB &head, TreeHK &root);

// --- NEW: CLI handlers extracted from previous main loop ---
// Each handler uses std::cin / std::cout (CLI). TUI will call endwin(), invoke handler, then restart ncurses.

static DSMayBay* _dsmb_ptr = &dsmb; // convenience if needed

static string TrangThaiChuyenBayText(int trangThai) {
	switch (trangThai) {
		case 0: return "huy";
		case 1: return "con ve";
		case 2: return "het ve";
		case 3: return "hoan tat";
		default: return "khong ro";
	}
}

static string DinhDangNgayGio(const DateTime& dt) {
	ostringstream oss;
	oss << (dt.ngay < 10 ? "0" : "") << dt.ngay << "/"
		<< (dt.thang < 10 ? "0" : "") << dt.thang << "/"
		<< dt.nam << " "
		<< (dt.gio < 10 ? "0" : "") << dt.gio << ":"
		<< (dt.phut < 10 ? "0" : "") << dt.phut;
	return oss.str();
}

static string DongTomTatChuyenBay(PTRCB p, int stt) {
	ostringstream oss;
	oss << stt << ". " << p->cb.MACB
		<< " | MB:" << p->cb.SOHIEUMB
		<< " | " << p->cb.SANBAYDI << "->" << p->cb.SANBAYDEN
		<< " | " << DinhDangNgayGio(p->cb.TGKHOIHANH)
		<< " | trong:" << DemVeConTrong(p->cb);
	return oss.str();
}

static bool ChuyenBayHopLe(PTRCB p) {
	return p != NULL && p->cb.MACB[0] != '\0';
}

static int TaoThamChieuMayBay(string lines[], int maxLines) {
	if (maxLines <= 0) return 0;
	if (dsmb.n == 0) {
		lines[0] = "(Chua co may bay trong maybay.txt)";
		return 1;
	}
	int n = 0;
	lines[n++] = "So hieu may bay (maybay.txt):";
	for (int i = 0; i < dsmb.n && n < maxLines; i++) {
		ostringstream oss;
		oss << "  " << (i + 1) << ". " << dsmb.nodes[i]->SOHIEU
			<< " | Loai: " << dsmb.nodes[i]->LOAI
			<< " | So cho: " << dsmb.nodes[i]->SOCHO;
		lines[n++] = oss.str();
	}
	return n;
}

static int TaoThamChieuChuyenBay(PTRCB dscb, string lines[], int maxLines) {
	if (maxLines <= 0) return 0;
	int count = 0;
	for (PTRCB p = dscb; p != NULL; p = p->next) {
		if (ChuyenBayHopLe(p)) count++;
	}
	if (count == 0) {
		lines[0] = "(Chua co chuyen bay trong chuyenbay.txt)";
		return 1;
	}
	int n = 0;
	lines[n++] = "Ma chuyen bay hien co (chuyenbay.txt):";
	int stt = 1;
	for (PTRCB p = dscb; p != NULL && n < maxLines; p = p->next) {
		if (!ChuyenBayHopLe(p)) continue;
		lines[n++] = "  " + DongTomTatChuyenBay(p, stt++);
	}
	return n;
}

static int TaoThamChieuChuyenBayTheoMayBay(PTRCB* ds, int soChuyen, const char* soHieu, string lines[], int maxLines) {
	if (maxLines <= 0) return 0;
	int n = 0;
	ostringstream header;
	header << "Chuyen bay cua may bay " << soHieu << " (chuyenbay.txt):";
	lines[n++] = header.str();
	for (int i = 0; i < soChuyen && n < maxLines; i++) {
		lines[n++] = "  " + DongTomTatChuyenBay(ds[i], i + 1);
	}
	return n;
}

static int TaoThamChieuVeConTrong(const ChuyenBay& cb, string lines[], int maxLines) {
	if (maxLines <= 0) return 0;
	if (cb.DSVE.ds == NULL) {
		lines[0] = "(Chua khoi tao danh sach ve)";
		return 1;
	}
	int n = 0;
	ostringstream header;
	header << "Ve con trong chuyen bay " << cb.MACB << " (co the dat):";
	lines[n++] = header.str();
	bool coVe = false;
	for (int i = 0; i < cb.DSVE.soLuongVe && n < maxLines; i++) {
		if (cb.DSVE.ds[i].SOCMND[0] == '\0') {
			lines[n++] = "  Ve so: " + to_string(cb.DSVE.ds[i].SOVE);
			coVe = true;
		}
	}
	if (!coVe) lines[n++] = "  (Het ve trong)";
	return n;
}

static int TaoThamChieuHanhKhachTrenChuyenBay(PTRCB p, TreeHK dshk, string lines[], int maxLines) {
	if (maxLines <= 0) return 0;
	int n = 0;
	ostringstream header;
	header << "Hanh khach da dat ve chuyen bay " << p->cb.MACB << " (hanhkhach.txt):";
	lines[n++] = header.str();
	if (p->cb.DSVE.ds == NULL) {
		lines[n++] = "  (Chua co danh sach ve)";
		return n;
	}
	bool coKH = false;
	for (int i = 0; i < p->cb.DSVE.soLuongVe && n < maxLines; i++) {
		if (p->cb.DSVE.ds[i].SOCMND[0] != '\0') {
			nodeHK* hk = TimHanhKhach(dshk, p->cb.DSVE.ds[i].SOCMND);
			ostringstream oss;
			oss << "  Ve " << p->cb.DSVE.ds[i].SOVE << " | CMND: " << p->cb.DSVE.ds[i].SOCMND;
			if (hk) oss << " | " << hk->hk.HO << " " << hk->hk.TEN;
			lines[n++] = oss.str();
			coKH = true;
		}
	}
	if (!coKH) lines[n++] = "  (Chua co hanh khach nao)";
	return n;
}

static int TaoThamChieuChuyenBayDaChon(PTRCB p, string lines[], int maxLines) {
	if (maxLines <= 0 || p == NULL) return 0;
	lines[0] = "Chuyen bay da chon:";
	lines[1] = "  " + DongTomTatChuyenBay(p, 1);
	return 2;
}

static void InTomTatChuyenBay(PTRCB p, int stt) {
	cout << DongTomTatChuyenBay(p, stt) << "\n";
}

// Nhap ngay gio; tra ve false neu nguoi dung huy
bool InputDateTimeInteractive(DateTime &dt, PTRCB chuyenBayThamChieu = NULL) {
	const string labels[] = {
		"Gio (hh)",
		"Phut (mm)",
		"Ngay (dd)",
		"Thang (mm)",
		"Nam (yyyy)"
	};
	const int fieldCount = 5;
	string values[5] = {"", "", "", "", ""};
	string refLines[UI_REF_MAX_LINES];
	int refCount = 0;
	if (chuyenBayThamChieu != NULL) {
		refCount = TaoThamChieuChuyenBayDaChon(chuyenBayThamChieu, refLines, UI_REF_MAX_LINES);
	}

	if (!uiFormReadInt("NHAP NGAY GIO", refLines, refCount, labels, values, fieldCount, 0, dt.gio)) return false;
	if (!uiFormReadInt("NHAP NGAY GIO", refLines, refCount, labels, values, fieldCount, 1, dt.phut)) return false;
	if (!uiFormReadInt("NHAP NGAY GIO", refLines, refCount, labels, values, fieldCount, 2, dt.ngay)) return false;
	if (!uiFormReadInt("NHAP NGAY GIO", refLines, refCount, labels, values, fieldCount, 3, dt.thang)) return false;
	if (!uiFormReadInt("NHAP NGAY GIO", refLines, refCount, labels, values, fieldCount, 4, dt.nam)) return false;
	return true;
}

static bool MayBayDangCoChuyenBay(PTRCB dscb, const char* soHieu) {
	for (PTRCB p = dscb; p != NULL; p = p->next) {
		if (strcmp(p->cb.SOHIEUMB, soHieu) == 0) return true;
	}
	return false;
}

static PTRCB ChonChuyenBayTheoSoHieu(PTRCB dscb, const string& mucDich) {
	const string labels[] = { "So hieu may bay" };
	const int fieldCount = 1;
	string values[1] = { "" };
	string refLines[UI_REF_MAX_LINES];
	int refCount = TaoThamChieuMayBay(refLines, UI_REF_MAX_LINES);

	if (!uiFormReadWord("CHON CHUYEN BAY - " + mucDich, refLines, refCount, labels, values, fieldCount, 0)) return NULL;

	const string& soHieu = values[0];

	int idx = TimMayBay(soHieu);
	if (idx == -1) {
		cout << "Khong ton tai may bay voi so hieu tren!\n";
		return NULL;
	}

	int soChuyen = 0;
	for (PTRCB p = dscb; p != NULL; p = p->next) {
		if (strcmp(p->cb.SOHIEUMB, dsmb.nodes[idx]->SOHIEU) == 0) {
			soChuyen++;
		}
	}

	if (soChuyen == 0) {
		cout << "May bay nay chua co chuyen bay nao!\n";
		return NULL;
	}

	PTRCB* ds = new PTRCB[soChuyen];
	int viTri = 0;
	for (PTRCB p = dscb; p != NULL; p = p->next) {
		if (strcmp(p->cb.SOHIEUMB, dsmb.nodes[idx]->SOHIEU) == 0) {
			ds[viTri++] = p;
		}
	}

	if (soChuyen == 1) {
		PTRCB ketQua = ds[0];
		string cbRef[UI_REF_MAX_LINES];
		int cbRefCount = TaoThamChieuChuyenBayDaChon(ketQua, cbRef, UI_REF_MAX_LINES);
		uiShowDataBox("DA CHON CHUYEN BAY", cbRef, cbRefCount);
		delete[] ds;
		return ketQua;
	}

	const string sttLabels[] = { "STT chuyen bay (1.." + to_string(soChuyen) + ")" };
	string sttValues[1] = { "" };
	string cbRef[UI_REF_MAX_LINES];
	int cbRefCount = TaoThamChieuChuyenBayTheoMayBay(ds, soChuyen, dsmb.nodes[idx]->SOHIEU, cbRef, UI_REF_MAX_LINES);
	int stt;
	while (true) {
		if (!uiFormReadInt("CHON CHUYEN BAY", cbRef, cbRefCount, sttLabels, sttValues, 1, 0, stt)) {
			delete[] ds;
			return NULL;
		}
		if (stt >= 1 && stt <= soChuyen) {
			PTRCB ketQua = ds[stt - 1];
			string chosenRef[UI_REF_MAX_LINES];
			int chosenRefCount = TaoThamChieuChuyenBayDaChon(ketQua, chosenRef, UI_REF_MAX_LINES);
			uiShowDataBox("DA CHON CHUYEN BAY", chosenRef, chosenRefCount);
			delete[] ds;
			return ketQua;
		}
		cout << "STT khong hop le!\n";
		sttValues[0].clear();
	}
}

static void XoaMayBayTheoSoHieu(PTRCB dscb, TreeHK &dshk) {
	const string labels[] = { "So hieu may bay can xoa" };
	const int fieldCount = 1;
	string values[1] = { "" };
	string refLines[UI_REF_MAX_LINES];
	int refCount = TaoThamChieuMayBay(refLines, UI_REF_MAX_LINES);

	if (!uiFormReadWord("XOA MAY BAY", refLines, refCount, labels, values, fieldCount, 0)) return;

	const string& soHieu = values[0];

	int index = TimMayBay(soHieu);
	if (index == -1) {
		cout << "Khong tim thay!\n";
		return;
	}

	if (MayBayDangCoChuyenBay(dscb, dsmb.nodes[index]->SOHIEU)) {
		cout << "Khong the xoa: may bay da duoc lap chuyen bay.\n";
		return;
	}

	string confirm;
	cout << "Ban chac chan muon xoa may bay " << soHieu << "? (Y/N): ";
	if (!uiReadWord("", confirm)) return;
	if (confirm != "Y" && confirm != "y") {
		cout << "Da huy thao tac.\n";
		return;
	}

	delete dsmb.nodes[index];
	for (int i = index; i < dsmb.n - 1; i++) {
		dsmb.nodes[i] = dsmb.nodes[i + 1];
	}
	dsmb.nodes[--dsmb.n] = NULL;
	uiShowFormScreen("XOA MAY BAY", refLines, refCount, labels, values, fieldCount, -1, "Xoa thanh cong!");		SaveAll(dscb, dshk);	cout << "\n";
}

static void SuaMayBayTheoSoHieu(PTRCB dscb, TreeHK &dshk) {
	const string findLabels[] = { "So hieu may bay can sua" };
	const string editLabels[] = {
		"So hieu may bay",
		"So ghe (" + to_string(MIN_SO_GHE) + " - " + to_string(MAX_SO_GHE) + ")",
		"Loai may bay (toi da 40 ky tu)"
	};
	string findValues[1] = { "" };
	string editValues[3] = { "", "", "" };
	string refLines[UI_REF_MAX_LINES];
	int refCount = TaoThamChieuMayBay(refLines, UI_REF_MAX_LINES);

	if (!uiFormReadWord("SUA MAY BAY", refLines, refCount, findLabels, findValues, 1, 0)) return;

	const string& soHieu = findValues[0];

	int index = TimMayBay(soHieu);
	if (index == -1) {
		cout << "Khong tim thay!\n";
		return;
	}

	MayBay* mb = dsmb.nodes[index];
	bool daCoChuyenBay = MayBayDangCoChuyenBay(dscb, mb->SOHIEU);
	editValues[0] = mb->SOHIEU;
	editValues[1] = to_string(mb->SOCHO);
	editValues[2] = mb->LOAI;

	int soGhe;
	while (true) {
		if (!uiFormReadInt("SUA MAY BAY", refLines, refCount, editLabels, editValues, 3, 1, soGhe)) return;
		if (soGhe < MIN_SO_GHE) {
			cout << "Loi: So ghe phai lon hon hoac bang " << MIN_SO_GHE << "!\n";
			editValues[1].clear();
			continue;
		}
		if (soGhe > MAX_SO_GHE) {
			cout << "Loi: So ghe khong duoc vuot qua " << MAX_SO_GHE << "!\n";
			editValues[1].clear();
			continue;
		}
		if (daCoChuyenBay && soGhe != mb->SOCHO) {
			cout << "Khong the doi so ghe: may bay da co chuyen bay lien ket.\n";
			return;
		}
		break;
	}

	while (true) {
		if (!uiFormReadLine("SUA MAY BAY", refLines, refCount, editLabels, editValues, 3, 2)) return;
		if (editValues[2].length() > 40) {
			cout << "Loi: Loai may bay khong duoc qua 40 ky tu!\n";
			editValues[2].clear();
			continue;
		}
		break;
	}

	mb->SOCHO = soGhe;
	strncpy(mb->LOAI, editValues[2].c_str(), 40);
	mb->LOAI[40] = '\0';
	uiShowFormScreen("SUA MAY BAY", refLines, refCount, editLabels, editValues, 3, -1, "Sua thanh cong!");
	SaveAll(dscb, dshk);
	cout << "\n";
}

void handleMayBayCLI(PTRCB &dscb, TreeHK &dshk) {
	const string items[] = {
		"Them may bay",
		"Xoa may bay",
		"Sua may bay",
		"Hien thi danh sach",
		"Quay lai"
	};
	const int itemCount = sizeof(items) / sizeof(items[0]);
	int m;
	do {
		m = uiMenu("QUAN LY MAY BAY", items, itemCount, "Esc / q: quay lai menu chinh");
		if (m == -1) break;
		uiResetCancel();
		switch (m) {
			case 0: ThemMayBay(); SaveAll(dscb, dshk); break;
			case 1: XoaMayBayTheoSoHieu(dscb, dshk); break;
			case 2: SuaMayBayTheoSoHieu(dscb, dshk); break;
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
	const string items[] = {
		"Lap chuyen bay moi",
		"Sua ngay gio chuyen bay",
		"Huy chuyen bay",
		"Xac nhan chuyen bay da hoan tat",  // ✅ THÊM
		"Quay lai"
	};
	const int itemCount = sizeof(items) / sizeof(items[0]);
	int m;
	do {
		m = uiMenu("QUAN LY CHUYEN BAY", items, itemCount, "Esc / q: quay lai menu chinh");
		if (m == -1) break;
		uiResetCancel();
		if (m == 0) {
			const string labels[] = {
				"Ma chuyen bay",
				"Gio (hh)",
				"Phut (mm)",
				"Ngay (dd)",
				"Thang (mm)",
				"Nam (yyyy)",
				"Noi xuat phat",
				"Noi den",
				"So hieu may bay"
			};
			const int fieldCount = 9;
			string values[9] = {"", "", "", "", "", "", "", "", ""};
			string cbRef[UI_REF_MAX_LINES];
			string mbRef[UI_REF_MAX_LINES];
			int cbRefCount = TaoThamChieuChuyenBay(dscb, cbRef, UI_REF_MAX_LINES);
			int mbRefCount = TaoThamChieuMayBay(mbRef, UI_REF_MAX_LINES);

			ChuyenBay cb;
			memset(&cb, 0, sizeof(ChuyenBay));
			while (true) {
				if (!uiFormReadWord("LAP CHUYEN BAY MOI", cbRef, cbRefCount, labels, values, fieldCount, 0)) {
					if (uiConsumeInputCancel()) return;
					continue;
				}
				strncpy(cb.MACB, values[0].c_str(), 15); cb.MACB[15] = '\0';
				if (TimChuyenBay(dscb, cb.MACB) != NULL) {
					cout << "Ma chuyen bay da ton tai!\n";
					values[0].clear();
					continue;
				}
				break;
			}
			if (!uiFormReadInt("LAP CHUYEN BAY MOI", NULL, 0, labels, values, fieldCount, 1, cb.TGKHOIHANH.gio)) { if (uiConsumeInputCancel()) return; else continue; }
			if (!uiFormReadInt("LAP CHUYEN BAY MOI", NULL, 0, labels, values, fieldCount, 2, cb.TGKHOIHANH.phut)) { if (uiConsumeInputCancel()) return; else continue; }
			if (!uiFormReadInt("LAP CHUYEN BAY MOI", NULL, 0, labels, values, fieldCount, 3, cb.TGKHOIHANH.ngay)) { if (uiConsumeInputCancel()) return; else continue; }
			if (!uiFormReadInt("LAP CHUYEN BAY MOI", NULL, 0, labels, values, fieldCount, 4, cb.TGKHOIHANH.thang)) { if (uiConsumeInputCancel()) return; else continue; }
			if (!uiFormReadInt("LAP CHUYEN BAY MOI", NULL, 0, labels, values, fieldCount, 5, cb.TGKHOIHANH.nam)) { if (uiConsumeInputCancel()) return; else continue; }
			if (!uiFormReadLine("LAP CHUYEN BAY MOI", NULL, 0, labels, values, fieldCount, 6)) { if (uiConsumeInputCancel()) return; else continue; }
			strncpy(cb.SANBAYDI, values[6].c_str(), 40); cb.SANBAYDI[40] = '\0';
			if (!uiFormReadLine("LAP CHUYEN BAY MOI", NULL, 0, labels, values, fieldCount, 7)) { if (uiConsumeInputCancel()) return; else continue; }
			strncpy(cb.SANBAYDEN, values[7].c_str(), 40); cb.SANBAYDEN[40] = '\0';

			while (true) {
				if (!uiFormReadLine("LAP CHUYEN BAY MOI", mbRef, mbRefCount, labels, values, fieldCount, 8)) {
					if (uiConsumeInputCancel()) return;
					continue;
				}
				int idx = TimMayBay(values[8]);
				if (idx == -1) {
					cout << "Khong ton tai may bay voi so hieu tren!\n";
					values[8].clear();
					continue;
				}
				strncpy(cb.SOHIEUMB, dsmb.nodes[idx]->SOHIEU, 15); cb.SOHIEUMB[15] = '\0';
				cb.SOCHO = dsmb.nodes[idx]->SOCHO;
				break;
			}

			cb.TRANGTHAI = 1;
			cb.DSVE.ds = NULL; cb.DSVE.soLuongVe = 0;
			KhoiTaoVe(cb);
			ThemChuyenBay(dscb, cb);
			SaveAll(dscb, dshk);
			uiShowFormScreen("LAP CHUYEN BAY MOI", mbRef, mbRefCount, labels, values, fieldCount, -1, "Them chuyen bay thanh cong!");
			cout << "\n";
			uiPause();
		} else if (m == 1) {
			PTRCB p = ChonChuyenBayTheoSoHieu(dscb, "sua ngay gio chuyen bay");
			if (!p) { cout << "Khong tim thay!\n"; uiPause(); continue; }
			DateTime tg;
			if (!InputDateTimeInteractive(tg, p)) continue;
			SuaNgayGioChuyenBay(dscb, p->cb.MACB, tg);
			SaveAll(dscb, dshk);
			cout << "Cap nhat thanh cong.\n";
			uiPause();
		} else if (m == 2) {
			PTRCB p = ChonChuyenBayTheoSoHieu(dscb, "huy chuyen bay");
			if (!p) { cout << "Khong tim thay!\n"; uiPause(); continue; }		string confirm;
		cout << "Ban chac chan muon huy chuyen bay " << p->cb.MACB << "? (Y/N): ";
		if (!uiReadWord("", confirm)) continue;
		if (confirm != "Y" && confirm != "y") {
			cout << "Da huy thao tac.\n";
			uiPause();
			continue;
		}			HuyChuyenBay(dscb, p->cb.MACB);
			SaveAll(dscb, dshk);
			cout << "Da huy chuyen bay.\n";
			uiPause();
		} else if (m == 3) {  // ✅ THÊM CASE MỚI
			PTRCB p = ChonChuyenBayTheoSoHieu(dscb, "xac nhan hoan tat");
			if (!p) { cout << "Khong tim thay!\n"; uiPause(); continue; }
			
			string confirm;
			cout << "Xac nhan chuyen bay " << p->cb.MACB << " da hoan tat? (Y/N): ";
			if (!uiReadWord("", confirm)) continue;
			if (confirm != "Y" && confirm != "y") {
				cout << "Da huy thao tac.\n";
				uiPause();
				continue;
			}
			
			XacNhanChuyenBayHoanTat(dscb, p->cb.MACB);
			SaveAll(dscb, dshk);
			cout << "Da cap nhat.\n";
			uiPause();
		} else if (m == 4) {
			break;
		}
	} while (m != 4 && m != -1);
}

void handleDatVeCLI(PTRCB &dscb, TreeHK &dshk) {
	PTRCB p = ChonChuyenBayTheoSoHieu(dscb, "dat ve");
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }

	const string labels[] = {
		"So ve (1.." + to_string(p->cb.SOCHO) + ")",
		"CMND",
		"Ho",
		"Ten",
		"Phai (Nam/Nu)"
	};
	const int fieldCount = 5;
	string values[5] = {"", "", "", "", ""};
	string refLines[UI_REF_MAX_LINES];
	int refCount = 0;
	refCount = TaoThamChieuChuyenBayDaChon(p, refLines, UI_REF_MAX_LINES);
	string veRef[UI_REF_MAX_LINES];
	int veRefCount = TaoThamChieuVeConTrong(p->cb, veRef, UI_REF_MAX_LINES);
	for (int i = 0; i < veRefCount && refCount < UI_REF_MAX_LINES; i++) {
		refLines[refCount++] = veRef[i];
	}
	int soVe;

	uiResetCancel();
	if (!uiFormReadInt("DAT VE", refLines, refCount, labels, values, fieldCount, 0, soVe)) return;
	if (!uiFormReadWord("DAT VE", refLines, refCount, labels, values, fieldCount, 1)) return;

	char cmndBuf[16];
	strncpy(cmndBuf, values[1].c_str(), 15); cmndBuf[15] = '\0';
	nodeHK* found = TimHanhKhach(dshk, cmndBuf);
	if (found == NULL) {
		if (!uiFormReadLine("DAT VE", refLines, refCount, labels, values, fieldCount, 2)) return;
		if (!uiFormReadLine("DAT VE", refLines, refCount, labels, values, fieldCount, 3)) return;
		if (!uiFormReadLine("DAT VE", refLines, refCount, labels, values, fieldCount, 4)) return;

		HanhKhach hk;
		strncpy(hk.SOCMND, cmndBuf, 15); hk.SOCMND[15] = '\0';
		strncpy(hk.HO, values[2].c_str(), 50); hk.HO[50] = '\0';
		strncpy(hk.TEN, values[3].c_str(), 10); hk.TEN[10] = '\0';
		strncpy(hk.PHAI, values[4].c_str(), 3); hk.PHAI[3] = '\0';
		ThemHanhKhach(dshk, hk);
		cout << "Da them hanh khach.\n";
	} else {
		values[2] = found->hk.HO;
		values[3] = found->hk.TEN;
		values[4] = found->hk.PHAI;
		uiShowFormScreen("DAT VE", refLines, refCount, labels, values, fieldCount, -1, "Da co thong tin hanh khach");
		cout << "\n";
	}
	if (!DatVe(dscb, p->cb.MACB, soVe, cmndBuf)) {
		cout << "Dat ve that bai!\n";
	} else {
		SaveAll(dscb, dshk);
	}
}

void handleHuyVeCLI(PTRCB &dscb, TreeHK &dshk) {
	PTRCB p = ChonChuyenBayTheoSoHieu(dscb, "huy ve");
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }

	const string labels[] = { "CMND" };
	string values[1] = { "" };
	string refLines[UI_REF_MAX_LINES];
	int refCount = TaoThamChieuHanhKhachTrenChuyenBay(p, dshk, refLines, UI_REF_MAX_LINES);

	uiResetCancel();
	if (!uiFormReadWord("HUY VE", refLines, refCount, labels, values, 1, 0)) return;

	char cmndBuf[16];
	strncpy(cmndBuf, values[0].c_str(), 15); cmndBuf[15] = '\0';
	bool ok = false;
	if (p->cb.DSVE.ds == NULL) { cout << "Chua khoi tao ve!\n"; return; }
	for (int i = 0; i < p->cb.DSVE.soLuongVe; i++) {
		if (strcmp(p->cb.DSVE.ds[i].SOCMND, cmndBuf) == 0) {
			string confirm;
			cout << "Ban chac chan muon huy ve nay? (Y/N): ";
			if (!uiReadWord("", confirm)) return;
			if (confirm != "Y" && confirm != "y") {
				cout << "Da huy thao tac.\n";
				return;
			}
			p->cb.DSVE.ds[i].SOCMND[0] = '\0';
			CapNhatTrangThai(p->cb);
			ok = true;
			break;
		}
	}
	if (ok) {
		SaveAll(dscb, dshk);
		cout << "Huy ve thanh cong\n";
	} else {
		cout << "Khong tim thay CMND tren chuyen bay\n";
	}
}

void handleInDanhSachCLI(PTRCB &dscb, TreeHK &dshk) {
	PTRCB p = ChonChuyenBayTheoSoHieu(dscb, "in danh sach hanh khach");
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }

	string lines[UI_REF_MAX_LINES];
	int lineCount = 0;
	lines[lineCount++] = "Chuyen bay: " + string(p->cb.MACB)
		+ " | " + DinhDangNgayGio(p->cb.TGKHOIHANH)
		+ " | Tu: " + string(p->cb.SANBAYDI)
		+ " | Den: " + string(p->cb.SANBAYDEN);
	lines[lineCount++] = "STT | SO VE | SO CMND | HO | TEN | PHAI";

	if (p->cb.DSVE.ds == NULL) {
		lines[lineCount++] = "(Chua co danh sach ve)";
	} else {
		int stt = 1;
		for (int i = 0; i < p->cb.DSVE.soLuongVe && lineCount < UI_REF_MAX_LINES; i++) {
			if (p->cb.DSVE.ds[i].SOCMND[0] != '\0') {
				nodeHK* hk = TimHanhKhach(dshk, p->cb.DSVE.ds[i].SOCMND);
				ostringstream oss;
				oss << "  " << setw(3) << stt++
					<< " | " << setw(4) << p->cb.DSVE.ds[i].SOVE
					<< " | " << setw(14) << p->cb.DSVE.ds[i].SOCMND;
				if (hk) {
					oss << " | " << hk->hk.HO << " | " << hk->hk.TEN << " | " << hk->hk.PHAI;
				} else {
					oss << " | (chua co thong tin)";
				}
				lines[lineCount++] = oss.str();
			}
		}
		if (stt == 1) lines[lineCount++] = "(Chua co hanh khach nao)";
	}
	uiShowDataBox("DANH SACH HANH KHACH", lines, lineCount);
}

void handleInChuyenBayTheoNgayCLI(PTRCB &dscb) {
	const string labels[] = {
		"Ngay",
		"Thang",
		"Nam",
		"Noi xuat phat",
		"Noi den"
	};
	const int fieldCount = 5;
	string values[5] = {"", "", "", "", ""};
	int ngay, thang, nam;

	if (!uiFormReadInt("IN CHUYEN BAY THEO NGAY", NULL, 0, labels, values, fieldCount, 0, ngay)) return;
	if (!uiFormReadInt("IN CHUYEN BAY THEO NGAY", NULL, 0, labels, values, fieldCount, 1, thang)) return;
	if (!uiFormReadInt("IN CHUYEN BAY THEO NGAY", NULL, 0, labels, values, fieldCount, 2, nam)) return;
	if (!uiFormReadLine("IN CHUYEN BAY THEO NGAY", NULL, 0, labels, values, fieldCount, 3)) return;

	char noiDiaBuf[41];
	strncpy(noiDiaBuf, values[3].c_str(), 40); noiDiaBuf[40] = '\0';
	
	if (!uiFormReadLine("IN CHUYEN BAY THEO NGAY", NULL, 0, labels, values, fieldCount, 4)) return;

	char noiDenBuf[41];
	strncpy(noiDenBuf, values[4].c_str(), 40); noiDenBuf[40] = '\0';

	string lines[UI_REF_MAX_LINES];
	int lineCount = 0;
	lines[lineCount++] = "Tim chuyen bay ngay " + to_string(ngay) + "/"
		+ to_string(thang) + "/" + to_string(nam) + " tu " + string(noiDiaBuf) + " den " + string(noiDenBuf) + " (con ve):";
	bool coKetQua = false;
	int stt = 1;
	for (PTRCB cur = dscb; cur != NULL && lineCount < UI_REF_MAX_LINES; cur = cur->next) {
		if (!ChuyenBayHopLe(cur)) continue;
		int veTrong = DemVeConTrong(cur->cb);
		if (TrungNgay(cur->cb.TGKHOIHANH, ngay, thang, nam)
			&& strcmp(cur->cb.SANBAYDI, noiDiaBuf) == 0
			&& strcmp(cur->cb.SANBAYDEN, noiDenBuf) == 0 && veTrong > 0) {
			lines[lineCount++] = "  " + DongTomTatChuyenBay(cur, stt++);
			coKetQua = true;
		}
	}
	if (!coKetQua) lines[lineCount++] = "  (Khong tim thay chuyen bay phu hop)";
	uiShowDataBox("IN CHUYEN BAY THEO NGAY", lines, lineCount);
}

void handleInVeConTrongCLI(PTRCB &dscb) {
	PTRCB p = ChonChuyenBayTheoSoHieu(dscb, "in ve con trong");
	if (!p) { cout << "Khong tim thay chuyen bay!\n"; return; }

	string lines[UI_REF_MAX_LINES];
	int lineCount = TaoThamChieuVeConTrong(p->cb, lines, UI_REF_MAX_LINES);
	uiShowDataBox("VE CON TRONG", lines, lineCount);
}

void handleThongKeCLI(PTRCB &dscb) {
	if (dsmb.n == 0) {
		string lines[1] = { "(Chua co may bay trong maybay.txt)" };
		uiShowDataBox("THONG KE SO LUOT BAY", lines, 1);
		return;
	}
	TK* kq = new TK[dsmb.n];
	ThongKeSoLuotBay(dscb, dsmb, kq);
	SapXepGiamDanTheoSoLuot(kq, dsmb.n);

	string lines[UI_REF_MAX_LINES];
	int lineCount = 0;
	lines[lineCount++] = "Thong ke so luot bay (giam dan):";
	for (int i = 0; i < dsmb.n && lineCount < UI_REF_MAX_LINES; i++) {
		lines[lineCount++] = "  " + string(kq[i].SOHIEU) + " : " + to_string(kq[i].soLuot) + " luot";
	}
	uiShowDataBox("THONG KE SO LUOT BAY", lines, lineCount);
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
        // ✅ COMMENT OUT LOG SPAM
        // cout << "Da luu maybay.txt\n";
    } else {
        cerr << "Khong the mo maybay.txt de ghi\n";
    }

    // chuyenbay
    ofstream fc("chuyenbay.txt");
    if (fc.is_open()) {
        LuuChuyenBayFile(head, fc);
        fc.close();
        // ✅ COMMENT OUT LOG SPAM
        // cout << "Da luu chuyenbay.txt\n";
    } else {
        cerr << "Khong the mo chuyenbay.txt de ghi\n";
    }

    // hanhkhach
    ofstream fh("hanhkhach.txt");
    if (fh.is_open()) {
        LuuHanhKhachFile(root, fh);
        fh.close();
        // ✅ COMMENT OUT LOG SPAM
        // cout << "Da luu hanhkhach.txt\n";
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
    }

    ifstream fc("chuyenbay.txt");
    if (fc.is_open()) {
        DocChuyenBayFile(head, fc);
        fc.close();
    }

    ifstream fh("hanhkhach.txt");
    if (fh.is_open()) {
        DocHanhKhachFile(root, fh);
        fh.close();
    }
}

int main() {
    dsmb.n = 0;
    PTRCB dscb = NULL;
    TreeHK dshk = NULL;

    LoadAll(dscb, dshk);

    uiRunApp(dscb, dshk);

    uiClear();
    uiDrawBoxTop("HE THONG QUAN LY MAY BAY");
    uiDrawBoxLine("Dang luu du lieu...");
    uiDrawBoxBottom();
    cout << "\n";

    SaveAll(dscb, dshk);
    
    // ✅ CLEANUP - GIẢI PHÓNG TẤT CẢ BỘNHỚ ĐỘNG
    XoaChuyenBayToanBo(dscb);
    XoaHanhKhachToanBo(dshk);
    XoaToanBo();  // Giải phóng dsmb
    
    cout << "Tam biet!\n";

    return 0;
}
