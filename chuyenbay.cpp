#include "CTDL.h"
#include <cstring>
#include <iostream>
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
    // giai phong ve
    if (p->cb.DSVE.ds != NULL) {
        delete[] p->cb.DSVE.ds;
        p->cb.DSVE.ds = NULL;
    }

    delete[] p->cb.DSVE.ds;
    p->cb.DSVE.ds = NULL;
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