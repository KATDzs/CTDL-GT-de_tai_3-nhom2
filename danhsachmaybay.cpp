#include "CTDL.h"
#include "maybay.h"
#include "ui.h"
#include <iostream>
#include <sstream>
using namespace std;

void HienThi()
{
    string lines[UI_REF_MAX_LINES];
    int lineCount = 0;

    if (dsmb.n == 0) {
        lines[lineCount++] = "(Chua co may bay trong maybay.txt)";
    } else {
        lines[lineCount++] = "Du lieu maybay.txt:";
        for (int i = 0; i < dsmb.n && lineCount < UI_REF_MAX_LINES; i++) {
            ostringstream oss;
            oss << "  " << (i + 1) << ". So hieu: " << dsmb.nodes[i]->SOHIEU
                << " | Loai: " << dsmb.nodes[i]->LOAI
                << " | So ghe: " << dsmb.nodes[i]->SOCHO;
            lines[lineCount++] = oss.str();
        }
    }

    uiShowDataBox("DANH SACH MAY BAY", lines, lineCount);
}
