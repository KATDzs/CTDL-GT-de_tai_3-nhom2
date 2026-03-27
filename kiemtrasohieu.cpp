#include "maybay.h"

bool KiemTraTrungSoHieu(string soHieu)
{
    for (int i = 0; i < ds.size(); i++)
    {
        if (ds[i].soHieu == soHieu)
            return true;
    }
    return false;
}

int TimMayBay(string soHieu)
{
    for (int i = 0; i < ds.size(); i++)
    {
        if (ds[i].soHieu == soHieu)
            return i;
    }
    return -1;
}