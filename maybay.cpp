#include "CTDL.h"
#include <fstream>


void LuuMayBayFile(DSMayBay dsmb, ofstream &f) {
    f << dsmb.n << endl;
    for (int i = 0; i < dsmb.n; i++) {
        f << dsmb.nodes[i]->SOHIEU << "|"
          << dsmb.nodes[i]->LOAI << "|"
          << dsmb.nodes[i]->SOCHO << endl;
    }
}

void DocMayBayFile(DSMayBay &dsmb, ifstream &f) {
    f >> dsmb.n;
    f.ignore();

    for (int i = 0; i < dsmb.n; i++) {
        MayBay* mb = new MayBay;

        f.getline(mb->SOHIEU, 16, '|');
        f.getline(mb->LOAI, 41, '|');
        f >> mb->SOCHO;
        f.ignore();

        dsmb.nodes[i] = mb;
    }
}