#include "CTDL.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include "maybay.h"
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

    delete dsmb.nodes[index];
    for (int i = index; i < dsmb.n - 1; i++) {
        dsmb.nodes[i] = dsmb.nodes[i + 1];
    }
    dsmb.nodes[--dsmb.n] = nullptr;
    cout << "Xoa thanh cong!\n";
}

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
    string loai;
    getline(cin, loai);
    strncpy(dsmb.nodes[index]->LOAI, loai.c_str(), 40);
    dsmb.nodes[index]->LOAI[40] = '\0';

    cout << "Nhap so ghe moi: ";
    cin >> dsmb.nodes[index]->SOCHO;

    cout << "Sua thanh cong!\n";
}

void ThemMayBay()
{
    if (dsmb.n >= MAX_MB) {
        cout << "Danh sach may bay day!\n";
        return;
    }

    MayBay* mb = new MayBay;

    string soHieu;
    cout << "Nhap so hieu: ";
    cin >> soHieu;

    // kiem tra trung
    if (KiemTraTrungSoHieu(soHieu))
    {
        cout << "So hieu bi trung!\n";
        delete mb;
        return;
    }

    // copy so hieu vao struct
    strncpy(mb->SOHIEU, soHieu.c_str(), 15);
    mb->SOHIEU[15] = '\0';

    cout << "Nhap loai may bay: ";
    cin.ignore(); // tránh lỗi getline bị skip
    string loai;
    getline(cin, loai);
    strncpy(mb->LOAI, loai.c_str(), 40);
    mb->LOAI[40] = '\0';

    cout << "Nhap so ghe: ";
    cin >> mb->SOCHO;

    dsmb.nodes[dsmb.n++] = mb;
    cout << "Them thanh cong!\n";
}

void LuuMayBayFile(DSMayBay dsmb_local, ofstream &f) {
    f << dsmb_local.n << endl;
    for (int i = 0; i < dsmb_local.n; i++) {
        f << dsmb_local.nodes[i]->SOHIEU << "|"
          << dsmb_local.nodes[i]->LOAI << "|"
          << dsmb_local.nodes[i]->SOCHO << endl;
    }
}

void DocMayBayFile(DSMayBay &dsmb_local, ifstream &f) {
    f >> dsmb_local.n;
    f.ignore();

    for (int i = 0; i < dsmb_local.n; i++) {
        MayBay* mb = new MayBay;

        f.getline(mb->SOHIEU, 16, '|');
        f.getline(mb->LOAI, 41, '|');
        f >> mb->SOCHO;
        f.ignore();

        dsmb_local.nodes[i] = mb;
    }
}