#include "CTDL.h"
#include <cstring>

int main() {
    DSMayBay dsmb;
    dsmb.n = 0;

    PTRCB dscb = NULL;
    TreeHK dshk = NULL;
#include "maybay.h"
#include <iostream>
using namespace std;

int main()
{
    int choice;

    do
    {
        cout << "\n===== QUAN LY MAY BAY =====\n";
        cout << "1. Them may bay\n";
        cout << "2. Xoa may bay\n";
        cout << "3. Sua may bay\n";
        cout << "4. Hien thi danh sach\n";
        cout << "5. Thoat\n";
        cout << "Chon (1-5): ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            ThemMayBay();
            break;
        case 2:
            XoaMayBay();
            break;
        case 3:
            SuaMayBay();
            break;
        case 4:
            HienThi();
            break;
        case 5:
            cout << "Thoat chuong trinh.\n";
            break;
        default:
            cout << "Lua chon khong hop le, vui long chon lai.\n";
            break;
        }
    } while (choice != 5);

    return 0;
}
}
