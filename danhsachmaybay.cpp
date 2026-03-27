#include "maybay.h"
#include <iostream>
using namespace std;

vector<MayBay> ds;

void ThemMayBay()
{
    MayBay mb;

    cout << "Nhap so hieu: ";
    cin >> mb.soHieu;

    if (KiemTraTrungSoHieu(mb.soHieu))
    {
        cout << "So hieu bi trung!\n";
        return;
    }

    cout << "Nhap loai may bay: ";
    cin.ignore();
    getline(cin, mb.loai);

    cout << "Nhap so ghe: ";
    cin >> mb.soGhe;

    ds.push_back(mb);
    cout << "Them thanh cong!\n";
}

void HienThi()
{
    cout << "\n===== DANH SACH MAY BAY =====\n";

    for (int i = 0; i < ds.size(); i++)
    {
        cout << "So hieu: " << ds[i].soHieu << endl;
        cout << "Loai: " << ds[i].loai << endl;
        cout << "So ghe: " << ds[i].soGhe << endl;
        cout << "---------------------\n";
    }
}