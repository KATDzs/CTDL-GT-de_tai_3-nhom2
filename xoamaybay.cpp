#include "maybay.h"
#include <iostream>
using namespace std;

// xoa may bay theo so hieu
void XoaMayBay()
{
    string soHieu;
    cout << "Nhap so hieu can xoa: ";
    cin >> soHieu;

    int index = TimMayBay(soHieu);

    if (index == -1)
    {
        cout << "Khong tim thay!\n";
        return;
    }

    ds.erase(ds.begin() + index);
    cout << "Xoa thanh cong!\n";
}