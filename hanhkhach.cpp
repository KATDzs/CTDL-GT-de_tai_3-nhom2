#include "CTDL.h"
#include <fstream>

nodeHK* TaoNodeHanhKhach(HanhKhach hk) {
    nodeHK* p = new nodeHK;
    p->hk = hk;
    p->left = p->right = NULL;
    return p;
}

void ThemHanhKhach(TreeHK &root, HanhKhach hk) {
    if (root == NULL) {
        root = TaoNodeHanhKhach(hk);
        return;
    }

    int cmp = strcmp(hk.SOCMND, root->hk.SOCMND);

    if (cmp < 0)
        ThemHanhKhach(root->left, hk);
    else if (cmp > 0)
        ThemHanhKhach(root->right, hk);
    // nếu trùng thì bỏ qua
}

nodeHK* TimHanhKhach(TreeHK root, const char* cmnd) {
    if (root == NULL) return NULL;

    int cmp = strcmp(cmnd, root->hk.SOCMND);

    if (cmp == 0) return root;
    else if (cmp < 0) return TimHanhKhach(root->left, cmnd);
    else return TimHanhKhach(root->right, cmnd);
}
void LuuHanhKhachFile(TreeHK root, ofstream &f) {
    if (root == NULL) return;

    LuuHanhKhachFile(root->left, f);

    f << root->hk.SOCMND << "|"
      << root->hk.HO << "|"
      << root->hk.TEN << "|"
      << root->hk.PHAI << endl;

    LuuHanhKhachFile(root->right, f);
}

void DocHanhKhachFile(TreeHK &root, ifstream &f) {
    while (!f.eof()) {
        HanhKhach hk;

        f.getline(hk.SOCMND, 16, '|');
        if (strlen(hk.SOCMND) == 0) break;

        f.getline(hk.HO, 51, '|');
        f.getline(hk.TEN, 11, '|');
        f.getline(hk.PHAI, 4);

        ThemHanhKhach(root, hk);
    }
}