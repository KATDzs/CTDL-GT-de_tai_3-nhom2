#include "CTDL.h"
#include "maybay.h"
#include <string>
#include <cstring>
using namespace std;

bool KiemTraTrungSoHieu(string soHieu)
{
    const char* s = soHieu.c_str();
    for (int i = 0; i < dsmb.n; i++)
    {
        if (strcmp(dsmb.nodes[i]->SOHIEU, s) == 0)
            return true;
    }
    return false;
}

int TimMayBay(string soHieu)
{
    const char* s = soHieu.c_str();
    for (int i = 0; i < dsmb.n; i++)
    {
        if (strcmp(dsmb.nodes[i]->SOHIEU, s) == 0)
            return i;
    }
    return -1;
}