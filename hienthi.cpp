#include "CTDL.h"
#include <cstring>
#include <iostream>
using namespace std;

void InDanhSachHanhKhach(ChuyenBay cb) {
    cout << "DANH SACH HANH KHACH THUOC CHUYEN BAY " << cb.MACB << endl;
    cout << "Ngay gio: "
         << cb.TGKHOIHANH.ngay << "/"
         << cb.TGKHOIHANH.thang << "/"
         << cb.TGKHOIHANH.nam << " "
         << cb.TGKHOIHANH.gio << ":"
         << cb.TGKHOIHANH.phut << endl;
    cout << "Noi den: " << cb.SANBAYDEN << endl;
    int stt = 1;
    if (cb.DSVE.ds == NULL) {
    cout << "Chua co danh sach ve!\n";
    return;
}
    for (int i = 0; i < cb.DSVE.soLuongVe; i++) {
        if (cb.DSVE.ds[i].SOCMND[0] != '\0') {  // Kiểm tra ký tự đầu tiên
            cout << stt++ << "\t"
                 << cb.DSVE.ds[i].SOVE << "\t"
                 << cb.DSVE.ds[i].SOCMND << endl;
        }
    }
}
bool TrungNgay(DateTime a, int ngay, int thang, int nam) {
    return (a.ngay == ngay && a.thang == thang && a.nam == nam);
}
void InChuyenBayTheoNgayVaNoiDen(PTRCB head, int ngay, int thang, int nam, char noiDen[]) {
    PTRCB current = head;
    while (current != NULL) {
        int veTrong = DemVeConTrong(current->cb);  // Tính một lần cho mỗi node
        if (TrungNgay(current->cb.TGKHOIHANH, ngay, thang, nam) &&
            strcmp(current->cb.SANBAYDEN, noiDen) == 0 && veTrong > 0) {
            cout << "Ma: " << current->cb.MACB
                 << " | Gio: " << current->cb.TGKHOIHANH.gio
                 << ":" << current->cb.TGKHOIHANH.phut
                 << " | Ve trong: " << veTrong
                 << endl;
        }
        current = current->next;
    }
}