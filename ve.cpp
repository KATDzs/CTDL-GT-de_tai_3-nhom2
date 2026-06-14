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
void CapNhatTrangThai(ChuyenBay &cb) {
    // ✅ Nếu hủy rồi thì không cập nhật lại (giữ nguyên 0)
    if (cb.TRANGTHAI == 0) return;
    
    int veTrong = DemVeConTrong(cb);
    
    // ✅ Cập nhật logic đúng:
    // Nếu hết vé -> trạng thái 2
    // Nếu còn vé -> trạng thái 1
    cb.TRANGTHAI = (veTrong == 0) ? 2 : 1;
}
bool DatVe(PTRCB head, char maCB[], int soVe, char cmnd[]) {
    PTRCB p = TimChuyenBay(head, maCB);

    if (p == NULL) {
        cout << "Khong tim thay chuyen bay!\n";
        return false;
    }
    // Không cho đặt nếu đã hủy, hết vé hoặc hoàn tất
    if (p->cb.TRANGTHAI == 0 || p->cb.TRANGTHAI == 2 || p->cb.TRANGTHAI == 3) {
        cout << "Chuyen bay khong the dat ve!\n";
        return false;
    }
    // Chưa khởi tạo vé
    if (p->cb.DSVE.ds == NULL) {
        cout << "Chua khoi tao ve!\n";
        return false;
    }
    // Kiểm tra CMND rỗng
    if (strlen(cmnd) == 0) {
        cout << "CMND khong hop le!\n";
        return false;
    }
    // Kiểm tra số vé hợp lệ
    if (soVe < 1 || soVe > p->cb.DSVE.soLuongVe) {
        cout << "So ve khong hop le!\n";
        return false;
    }
    // Kiểm tra CMND đã tồn tại trong chuyến bay
    for (int i = 0; i < p->cb.DSVE.soLuongVe; i++) {
        if (strcmp(p->cb.DSVE.ds[i].SOCMND, cmnd) == 0) {
            cout << "CMND da dat ve!\n";
            return false;
        }
    }
    // Kiểm tra đã có người đặt chưa
    if (p->cb.DSVE.ds[soVe - 1].SOCMND[0] != '\0') {
        cout << "Ve da duoc dat!\n";
        return false;
    }
    // Đặt vé
    strncpy(p->cb.DSVE.ds[soVe - 1].SOCMND, cmnd, 15);
    p->cb.DSVE.ds[soVe - 1].SOCMND[15] = '\0';
    // Cập nhật trạng thái
    CapNhatTrangThai(p->cb);
    cout << "Dat ve thanh cong!\n";
    return true;
}