#pragma once
#include <vector>
#include <string>
using namespace std;

struct MayBay
{
    string soHieu;
    string loai;
    int soGhe;
};

extern vector<MayBay> ds;

bool KiemTraTrungSoHieu(string soHieu);
int TimMayBay(string soHieu);

void ThemMayBay();
void XoaMayBay();
void SuaMayBay();
void HienThi();