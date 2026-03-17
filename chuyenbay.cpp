#include "CTDL.h"
#include <fstream>

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

void ThemCuoiCB(PTRCB &head, ChuyenBay cb) {
    nodeCB* p = new nodeCB;
    p->cb = cb;
    p->next = NULL;

    if (head == NULL) {
        head = p;
    } else {
        PTRCB q = head;
        while (q->next != NULL) q = q->next;
        q->next = p;
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