#pragma once
#include <iostream>
using namespace std;

/*  HẰNG SỐ  */
const int MAX_MB = 300;

/*  MÁY BAY  */
struct MayBay {
    char SOHIEU[16];     // C15
    char LOAI[41];       // C40
    int SOCHO;           // >= 20
};

struct DSMayBay {
    int n = 0;
    MayBay* nodes[MAX_MB];   // mảng con trỏ
};

/*  VÉ  */
struct Ve {
    int SOVE;                // 1 -> số chỗ
    char SOCMND[16];         // rỗng nếu chưa đặt
};

/*  DANH SÁCH VÉ */
struct DanhSachVe {
    Ve* ds = NULL;           // cấp phát động
    int soLuongVe = 0;
};

/*  NGÀY GIỜ  */
struct DateTime {
    int ngay, thang, nam;
    int gio, phut;
};

/*  CHUYẾN BAY  */
struct ChuyenBay {
    char MACB[16];           // C15
    DateTime TGKHOIHANH;
    char SANBAYDEN[41];
    int TRANGTHAI = 1;           // 0:hủy, 1:còn vé, 2:hết vé, 3:hoàn tất
    char SOHIEUMB[16];
    int SOCHO;               // liên kết máy bay
    DanhSachVe DSVE;
};

struct nodeCB {
    ChuyenBay cb;
    nodeCB* next;
};
typedef nodeCB* PTRCB;

/* HÀNH KHÁCH */
struct HanhKhach {
    char SOCMND[16];
    char HO[51];
    char TEN[11];
    char PHAI[4];            // Nam / Nu
};

struct nodeHK {
    HanhKhach hk;
    nodeHK* left;
    nodeHK* right;
};
typedef nodeHK* TreeHK;

