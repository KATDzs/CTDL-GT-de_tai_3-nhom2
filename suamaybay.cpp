#include "maybay.h"
#include <iostream>
using namespace std;

void SuaMayBay()
{
    string soHieu;
    cout << "Nhap so hieu can sua: ";
    cin >> soHieu;

    int index = TimMayBay(soHieu);

    if (index == -1)
    {
        cout << "Khong tim thay!\n";
        return;
    }

    cout << "Nhap loai moi: ";
    cin.ignore();
    getline(cin, ds[index].loai);

    cout << "Nhap so ghe moi: ";
    cin >> ds[index].soGhe;

    cout << "Sua thanh cong!\n";
}