#include "maybay.h"
#include <iostream>
using namespace std;

// Them may bay
void ThemMayBay()
{
    MayBay mb;

    cout << "Nhap so hieu: ";
    cin >> mb.soHieu;

    // kiem tra trung
    if (KiemTraTrungSoHieu(mb.soHieu))
    {
        cout << "So hieu bi trung!\n";
        return;
    }

    cout << "Nhap loai may bay: ";
    cin.ignore(); // tránh lỗi getline bị skip
    getline(cin, mb.loai);

    cout << "Nhap so ghe: ";
    cin >> mb.soGhe;

    ds.push_back(mb);
    cout << "Them thanh cong!\n";
}