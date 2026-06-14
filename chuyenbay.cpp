#include "CTDL.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

PTRCB TaoNodeChuyenBay(ChuyenBay cb) {
    PTRCB p = new nodeCB;
    p->cb = cb;
    p->next = NULL;
    return p;
}

int SoSanhMa(char a[], char b[]) {
    return strcmp(a, b);
}

void ThemChuyenBay(PTRCB &head, ChuyenBay cb) {
    PTRCB p = TaoNodeChuyenBay(cb);

    if (head == NULL || SoSanhMa(cb.MACB, head->cb.MACB) < 0) {
        p->next = head;
        head = p;
        return;
    }

    PTRCB prev = NULL, cur = head;
    while (cur != NULL && SoSanhMa(cb.MACB, cur->cb.MACB) > 0) {
        prev = cur;
        cur = cur->next;
    }

    if (cur != NULL && SoSanhMa(cb.MACB, cur->cb.MACB) == 0) {
        cout << "Ma chuyen bay da ton tai!\n";
        delete p;
        return;
    }

    p->next = cur;
    prev->next = p;
}

void ThemCuoiCB(PTRCB &head, ChuyenBay cb) {
    PTRCB p = TaoNodeChuyenBay(cb);

    if (head == NULL) {
        head = p;
    } else {
        PTRCB q = head;
        while (q->next != NULL) q = q->next;
        q->next = p;
    }
}

PTRCB TimChuyenBay(PTRCB head, char maCB[]) {
    while (head != NULL) {
        if (strcmp(head->cb.MACB, maCB) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}

void HuyChuyenBay(PTRCB head, char maCB[]) {
    PTRCB p = TimChuyenBay(head, maCB);
    if (p == NULL) {
        cout << "Khong tim thay!\n";
        return;
    }

    if (p->cb.DSVE.ds != NULL) {
        for (int i = 0; i < p->cb.DSVE.soLuongVe; i++) {
            p->cb.DSVE.ds[i].SOCMND[0] = '\0';
        }
    }

    p->cb.TRANGTHAI = 0;
}

void SuaNgayGioChuyenBay(PTRCB head, char maCB[], DateTime tgMoi) {
    PTRCB p = TimChuyenBay(head, maCB);
    if (p == NULL) {
        cout << "Khong tim thay!\n";
        return;
    }
    p->cb.TGKHOIHANH = tgMoi;
}

void LuuChuyenBayFile(PTRCB head, ofstream &f) {
    PTRCB p = head;
    while (p != NULL) {
        // header info
        f << p->cb.MACB << "|"
          << p->cb.TGKHOIHANH.ngay << " "
          << p->cb.TGKHOIHANH.thang << " "
          << p->cb.TGKHOIHANH.nam << " "
          << p->cb.TGKHOIHANH.gio << " "
          << p->cb.TGKHOIHANH.phut << "|"
          << p->cb.SANBAYDEN << "|"
          << p->cb.TRANGTHAI << "|"
          << p->cb.SOHIEUMB << "|"
          << p->cb.SOCHO << "|";

        // write all ticket SOCMND tokens (ensure always SOCHO tokens for robust parsing)
        int so = p->cb.SOCHO;
        if (p->cb.DSVE.ds != NULL && p->cb.DSVE.soLuongVe == so) {
            for (int i = 0; i < so; i++) {
                // write SOCMND (may be empty)
                f << p->cb.DSVE.ds[i].SOCMND << "|";
            }
        } else {
            // no DSVE allocated -> write empty tokens
            for (int i = 0; i < so; i++) {
                f << "|";
            }
        }
        f << endl;

        p = p->next;
    }
}

void DocChuyenBayFile(PTRCB &head, ifstream &f) {
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;

        ChuyenBay cb;
        size_t pos = 0;
        string token;

        pos = line.find('|');
        if (pos == string::npos) continue;
        strncpy(cb.MACB, line.substr(0, pos).c_str(), 15);
        cb.MACB[15] = '\0';
        if (cb.MACB[0] == '\0') continue;

        line = line.substr(pos + 1);
        istringstream iss(line);
        if (!(iss >> cb.TGKHOIHANH.ngay >> cb.TGKHOIHANH.thang >> cb.TGKHOIHANH.nam
                  >> cb.TGKHOIHANH.gio >> cb.TGKHOIHANH.phut)) continue;

        string rest;
        getline(iss, rest);
        if (!rest.empty() && rest[0] == '|') rest = rest.substr(1);

        pos = rest.find('|');
        if (pos == string::npos) continue;
        strncpy(cb.SANBAYDEN, rest.substr(0, pos).c_str(), 40);
        cb.SANBAYDEN[40] = '\0';
        rest = rest.substr(pos + 1);

        pos = rest.find('|');
        if (pos == string::npos) continue;
        cb.TRANGTHAI = atoi(rest.substr(0, pos).c_str());
        rest = rest.substr(pos + 1);

        pos = rest.find('|');
        if (pos == string::npos) continue;
        strncpy(cb.SOHIEUMB, rest.substr(0, pos).c_str(), 15);
        cb.SOHIEUMB[15] = '\0';
        rest = rest.substr(pos + 1);

        pos = rest.find('|');
        if (pos == string::npos) continue;
        cb.SOCHO = atoi(rest.substr(0, pos).c_str());
        rest = rest.substr(pos + 1);

        if (cb.SOCHO > 0) {
            cb.DSVE.ds = new Ve[cb.SOCHO];
            cb.DSVE.soLuongVe = cb.SOCHO;
            for (int i = 0; i < cb.SOCHO; i++) {
                cb.DSVE.ds[i].SOVE = i + 1;
                cb.DSVE.ds[i].SOCMND[0] = '\0';
                pos = rest.find('|');
                if (pos == string::npos) {
                    strncpy(cb.DSVE.ds[i].SOCMND, rest.c_str(), 15);
                    cb.DSVE.ds[i].SOCMND[15] = '\0';
                    rest.clear();
                } else {
                    strncpy(cb.DSVE.ds[i].SOCMND, rest.substr(0, pos).c_str(), 15);
                    cb.DSVE.ds[i].SOCMND[15] = '\0';
                    rest = rest.substr(pos + 1);
                }
            }
        } else {
            cb.DSVE.ds = NULL;
            cb.DSVE.soLuongVe = 0;
        }

        ThemCuoiCB(head, cb);
    }
}
