#include "CTDL.h"
#include <cstring>
#include <iostream>
using namespace std;

void ThongKeSoLuotBay(PTRCB head, DSMayBay dsMB, TK kq[]) {
    for (int i = 0; i < dsMB.n; i++) {
        strcpy(kq[i].SOHIEU, dsMB.nodes[i]->SOHIEU);
        kq[i].soLuot = 0;
    }
    PTRCB current = head;
    while (current != NULL) {
        // ✅ ĐỊA LUẬT: Chỉ đếm chuyến có TRANGTHAI == 3 (hoàn tất)
        if (current->cb.TRANGTHAI == 3) {
            for (int i = 0; i < dsMB.n; i++) {
                if (strcmp(current->cb.SOHIEUMB, dsMB.nodes[i]->SOHIEU) == 0) {
                    kq[i].soLuot++;
                    break;
                }
            }
        }
        current = current->next;
    }
}
void SapXepGiamDanTheoSoLuot(TK kq[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (kq[j].soLuot > kq[maxIdx].soLuot) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            TK temp = kq[i];
            kq[i] = kq[maxIdx];
            kq[maxIdx] = temp;
        }
    }
}