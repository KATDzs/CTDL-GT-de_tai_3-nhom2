#include "CTDL.h"
#include <cstring>
#include <iostream>
#include <fstream>
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
        delete[] p->cb.DSVE.ds;
        p->cb.DSVE.ds = NULL;
    }

    p->cb.DSVE.soLuongVe = 0;
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
        f << p->cb.MACB << "|"
          << p->cb.TGKHOIHANH.ngay << " "
          << p->cb.TGKHOIHANH.thang << " "
          << p->cb.TGKHOIHANH.nam << " "
          << p->cb.TGKHOIHANH.gio << " "
          << p->cb.TGKHOIHANH.phut << "|"
          << p->cb.SANBAYDEN << "|"
          << p->cb.TRANGTHAI << "|"
          << p->cb.SOHIEUMB << "|"
          << p->cb.SOCHO << endl;

        p = p->next;
    }
}

void DocChuyenBayFile(PTRCB &head, ifstream &f) {
    while (!f.eof()) {
        ChuyenBay cb;

        f.getline(cb.MACB, 16, '|');
        if (strlen(cb.MACB) == 0) break;

        f >> cb.TGKHOIHANH.ngay
          >> cb.TGKHOIHANH.thang
          >> cb.TGKHOIHANH.nam
          >> cb.TGKHOIHANH.gio
          >> cb.TGKHOIHANH.phut;
        f.ignore();

        f.getline(cb.SANBAYDEN, 41, '|');
        f >> cb.TRANGTHAI;
        f.ignore();

        f.getline(cb.SOHIEUMB, 16, '|');
        f >> cb.SOCHO;
        f.ignore();

        ThemCuoiCB(head, cb);
    }
}