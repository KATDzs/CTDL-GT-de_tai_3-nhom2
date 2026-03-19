#include "CTDL.h"
#include <cstring>
#include <iostream>
using namespace std;

void KhoiTaoVe(ChuyenBay &cb) {
    cb.DSVE.soLuongVe = cb.SOCHO;
    cb.DSVE.ds = new Ve[cb.SOCHO];

    for (int i = 0; i < cb.SOCHO; i++) {
        cb.DSVE.ds[i].SOVE = i + 1;
        cb.DSVE.ds[i].SOCMND[0] = '\0';
    }
}
int DemVeConTrong(ChuyenBay cb) {
    if (cb.DSVE.ds == NULL) return 0;
    int dem = 0;
    for (int i = 0; i < cb.DSVE.soLuongVe; i++) {
        if (cb.DSVE.ds[i].SOCMND[0] == '\0') {  // Kiểm tra ký tự đầu tiên 
            dem++;
        }
    }
    return dem;
}
void InVeConTrong(PTRCB head, char maCB[]) {
    PTRCB p = TimChuyenBay(head, maCB);
    if (p == NULL) {
        cout << "Khong tim thay!\n";
        return;
    }
    if (p->cb.DSVE.ds == NULL) {
    cout << "Chua khoi tao ve!\n";
    return;
    }
    for (int i = 0; i < p->cb.DSVE.soLuongVe; i++) {
        if (p->cb.DSVE.ds[i].SOCMND[0] == '\0') {  // Kiểm tra ký tự đầu tiên
            cout << "Ve so: " << p->cb.DSVE.ds[i].SOVE << endl;
        }
    }
}