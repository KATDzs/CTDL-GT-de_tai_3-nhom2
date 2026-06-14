#include "CTDL.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include "maybay.h"
#include "ui.h"
using namespace std;
// xoa may bay theo so hieu
void XoaMayBay()
{
    string soHieu;
    if (!uiReadWord("Nhap so hieu can xoa:", soHieu)) return;

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
    if (!uiReadWord("Nhap so hieu may bay can sua:", soHieu)) return;

    int index = TimMayBay(soHieu);
    if (index == -1)
    {
        cout << "Khong tim thay!\n";
        return;
    }

    MayBay* mb = dsmb.nodes[index];
    
    int soGhe;

    while (true)
    {
        if (!uiReadInt("Sua so ghe (" + to_string(MIN_SO_GHE) + " - " + to_string(MAX_SO_GHE) + "):", soGhe)) return;
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

        string loai;
        if (!uiReadLine("Sua loai may bay (toi da 40 ky tu):", loai)) return;

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

        const string labels[] = {
            "So hieu (toi da 15 ky tu)",
            "So ghe (" + to_string(MIN_SO_GHE) + " - " + to_string(MAX_SO_GHE) + ")",
            "Loai may bay (toi da 40 ky tu)"
        };
        const int fieldCount = 3;
        string values[3] = {"", "", ""};
        string refLines[UI_REF_MAX_LINES];
        int refCount = 0;
        if (dsmb.n > 0) {
            refLines[refCount++] = "So hieu da co trong maybay.txt (tranh trung):";
            for (int i = 0; i < dsmb.n && refCount < UI_REF_MAX_LINES; i++) {
                refLines[refCount++] = "  " + string(dsmb.nodes[i]->SOHIEU)
                    + " | " + string(dsmb.nodes[i]->LOAI)
                    + " | " + to_string(dsmb.nodes[i]->SOCHO) + " cho";
            }
        } else {
            refLines[refCount++] = "(Chua co may bay nao trong maybay.txt)";
        }

        uiShowTaskScreen("THEM MAY BAY", "Con " + to_string(MAX_MB - dsmb.n) + " cho trong");

        while (true) {
            if (!uiFormReadWord("THEM MAY BAY", refLines, refCount, labels, values, fieldCount, 0)) return;

            if (values[0].empty()) {
                cout << "Loi: So hieu khong duoc rong!\n";
                values[0].clear();
                continue;
            }
            if (values[0].length() > 15) {
                cout << "Loi: So hieu khong duoc qua 15 ky tu!\n";
                values[0].clear();
                continue;
            }
            if (KiemTraTrungSoHieu(values[0])) {
                cout << "Loi: So hieu bi trung!\n";
                values[0].clear();
                continue;
            }
            break;
        }

        int soGhe;
        while (true) {
            if (!uiFormReadInt("THEM MAY BAY", refLines, refCount, labels, values, fieldCount, 1, soGhe)) return;

            if (soGhe < MIN_SO_GHE) {
                cout << "Loi: So ghe phai lon hon hoac bang " << MIN_SO_GHE << "!\n";
                values[1].clear();
                continue;
            }
            if (soGhe > MAX_SO_GHE) {
                cout << "Loi: So ghe khong duoc vuot qua " << MAX_SO_GHE << "!\n";
                values[1].clear();
                continue;
            }
            break;
        }

        while (true) {
            if (!uiFormReadLine("THEM MAY BAY", refLines, refCount, labels, values, fieldCount, 2)) return;

            if (values[2].length() > 40) {
                cout << "Loi: Loai may bay khong duoc qua 40 ky tu!\n";
                values[2].clear();
                continue;
            }
            break;
        }

        MayBay* mb = new MayBay;
        strcpy(mb->SOHIEU, values[0].c_str());
        mb->SOCHO = soGhe;
        strncpy(mb->LOAI, values[2].c_str(), 40);
        mb->LOAI[40] = '\0';

        dsmb.nodes[dsmb.n++] = mb;
        uiShowFormScreen("THEM MAY BAY", refLines, refCount, labels, values, fieldCount, -1, "Them may bay thanh cong!");
        cout << "\n";
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
