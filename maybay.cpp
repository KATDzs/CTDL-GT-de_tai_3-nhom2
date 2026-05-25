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

    // If node exists, delete it and (optionally) clear the pointer before shifting
    if (dsmb.nodes[index] != nullptr) {
        delete dsmb.nodes[index];
        dsmb.nodes[index] = nullptr;
    }

    for (int i = index; i < dsmb.n - 1; i++) {
        dsmb.nodes[i] = dsmb.nodes[i + 1];
    }
    dsmb.nodes[--dsmb.n] = nullptr;
    cout << "Xoa thanh cong!\n";
}

void SuaMayBay()
{
    string soHieu;
    cout << "Nhap so hieu may bay can sua: ";
    cin >> soHieu;

    int index = TimMayBay(soHieu);
    if (index == -1)
    {
        cout << "Khong tim thay!\n";
        return;
    }

    MayBay* mb = dsmb.nodes[index];

    while (true)
    {
        cout << "Sua so hieu (toi da 15 ky tu): ";
        cin >> soHieu;

        if (cin.fail())
        {
            cout << "Loi: Nhap chuoi khong hop le!\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (soHieu.length() == 0)
        {
            cout << "Loi: So hieu khong duoc rong!\n";
            continue;
        }

        if (soHieu.length() > 15)
        {
            cout << "Loi: So hieu khong duoc qua 15 ky tu!\n";
            continue;
        }

        if (KiemTraTrungSoHieu(soHieu) && soHieu != string(mb->SOHIEU))
        {
            cout << "Loi: So hieu bi trung!\n";
            continue;
        }

        break;
    }

    strcpy(mb->SOHIEU, soHieu.c_str());
    
        int soGhe;

    while (true)
    {
        cout << "Sua so ghe (" << MIN_SO_GHE << " - " << MAX_SO_GHE << "): ";
        cin >> soGhe;

        if (cin.fail())
        {
            cout << "Loi: Vui long nhap so!\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (soGhe < MIN_SO_GHE)
        {
            cout << "Loi: So ghe phai lon hon hoac bang " << MIN_SO_GHE << "!\n";
            continue;
        }

        if (soGhe > MAX_SO_GHE)
        {
            cout << "Loi: So ghe khong duoc vuot qua " << MAX_SO_GHE << "!\n";
            continue;
        }

        break;
    }

    mb->SOCHO = soGhe;

        cout << "Sua loai may bay (toi da 40 ky tu): ";
        cin.ignore(10000, '\n');
        string loai;
        getline(cin, loai);

        if (loai.length() > 40)
        {
            cout << "Loi: Loai may bay khong duoc qua 40 ky tu!\n";
            return;
        }

        strncpy(mb->LOAI, loai.c_str(), 40);
        mb->LOAI[40] = '\0';
        cout << "Sua thanh cong!\n";

}

void ThemMayBay()
    {

        if (dsmb.n >= MAX_MB) {
            cout << "Danh sach may bay day!\n";
            return;
        }
        cout << "Con " << (MAX_MB - dsmb.n) << " cho trong.\n";
        MayBay* mb = new MayBay;

        string soHieu;

        while (true)
        {
            cout << "Nhap so hieu (toi da 15 ky tu): ";
            cin >> soHieu;

            if (cin.fail())
            {
                cout << "Loi: Nhap chuoi khong hop le!\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }

            if (soHieu.length() == 0)
            {
                cout << "Loi: So hieu khong duoc rong!\n";
                continue;
            }

            if (soHieu.length() > 15)
            {
                cout << "Loi: So hieu khong duoc qua 15 ky tu!\n";
                continue;
            }

            if (KiemTraTrungSoHieu(soHieu))
            {
                cout << "Loi: So hieu bi trung!\n";
                continue;
            }

            break;
        }

        strcpy(mb->SOHIEU, soHieu.c_str());
    
        int soGhe;
string input;

while (true)
{
    cout << "Nhap so ghe (" << MIN_SO_GHE << " - " << MAX_SO_GHE << "): ";
    cin >> input;

    bool isNumber = true;
    for (char c : input)
    {
        if (!isdigit(c))
        {
            isNumber = false;
            break;
        }
    }

    if (!isNumber)
    {
        cout << "Loi: Vui long nhap so!\n";
        continue;
    }

    soGhe = stoi(input);

    if (soGhe < MIN_SO_GHE)
    {
        cout << "Loi: So ghe phai lon hon hoac bang " << MIN_SO_GHE << "!\n";
        continue;
    }

    if (soGhe > MAX_SO_GHE)
    {
        cout << "Loi: So ghe khong duoc vuot qua " << MAX_SO_GHE << "!\n";
        continue;
    }

    break;
}

        mb->SOCHO = soGhe;

        cout << "Nhap loai may bay (toi da 40 ky tu): ";
        cin.ignore(10000, '\n');
        string loai;
        getline(cin, loai);

        if (loai.length() > 40)
        {
            cout << "Loi: Loai may bay khong duoc qua 40 ky tu!\n";
            delete mb;
            return;
        }

        strncpy(mb->LOAI, loai.c_str(), 40);
        mb->LOAI[40] = '\0';

        dsmb.nodes[dsmb.n++] = mb;
        cout << "Them may bay thanh cong!\n";
    }

void LuuMayBayFile(DSMayBay dsmb_local, ofstream &f) {
    f << dsmb_local.n << endl;
    for (int i = 0; i < dsmb_local.n; i++) {
        f << dsmb_local.nodes[i]->SOHIEU << "|"
          << dsmb_local.nodes[i]->LOAI << "|"
          << dsmb_local.nodes[i]->SOCHO << endl;
    }
}

void XoaToanBo()
{
    for (int i = 0; i < dsmb.n; i++)
        delete dsmb.nodes[i];
    dsmb.n = 0;
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