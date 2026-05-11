#include "CTDL.h"
#include "maybay.h"
#include <iostream>
#include <cstring>
using namespace std;

void HienThi()
{
    cout << "\n===== DANH SACH MAY BAY =====\n";

    for (int i = 0; i < dsmb.n; i++)
    {
        cout << "So hieu: " << dsmb.nodes[i]->SOHIEU << endl;
        cout << "Loai: " << dsmb.nodes[i]->LOAI << endl;
        cout << "So ghe: " << dsmb.nodes[i]->SOCHO << endl;
        cout << "---------------------\n";
    }
}