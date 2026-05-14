# CTDL-GT-de_tai_3-nhom2

MỤC TIÊU: Cài đặt hệ thống quản lý máy bay, chuyến bay, vé và hành khách theo các yêu cầu dưới đây. Tài liệu này xác định chính xác yêu cầu chức năng (a → h) và giao diện CLI chi tiết (có hỗ trợ điều hướng mũi tên / Enter / Esc).

1) YÊU CẦU CHỨC NĂNG (chi tiết theo a → h)
a) Quản lý danh sách máy bay
- Thêm máy bay: nhập Số hiệu (unique, tối đa 15 ký tự), Loại (<=40 chars), Số ghế (>=20). Kiểm tra trùng Số hiệu. Lưu vào dsmb.nodes[].
- Xóa máy bay: nhập Số hiệu → nếu tồn tại và không có chuyến bay liên kết đang có vé thì xóa; nếu có chuyến đang dùng => cảnh báo/hủy bị từ chối.
- Sửa máy bay: tìm theo Số hiệu → cho phép sửa Loại, Số ghế (không đổi Số hiệu).

b) Quản lý chuyến bay (DSCB theo thứ tự tăng dần mã chuyến)
- Lập chuyến bay mới: nhập MACB (unique), ngày giờ, SANBAYDEN, chọn máy bay theo SOHIEU (phải có trong dsmb), tự gán SOCHO từ máy bay, khởi tạo DSVE (KhoiTaoVe) theo SOCHO, gán TRANGTHAI = 1. Chèn vị trí đúng theo sắp xếp tăng dần MACB (ThemChuyenBay).
- Sửa ngày giờ: tìm MACB → cập nhật TGKHOIHANH.
- Hủy chuyến: tìm MACB → hủy vé đã cấp (giải phóng DSVE), đặt TRANGTHAI = 0. (Không xóa node khỏi danh sách; chỉ đổi trạng thái).

c) Đặt vé
- Chọn MACB và Số vé (1..SOCHO).
- Kiểm tra: chuyến không bị hủy/hoàn tất/hết vé; vé chưa được đặt; CMND không rỗng; cùng CMND chưa đặt vé khác trên chuyến.
- Nếu CMND chưa có trong cây HanhKhach: yêu cầu nhập HO, TEN, PHAI rồi ThemHanhKhach.
- Nếu CMND đã có: in thông tin HK để kiểm tra.
- Gán SOCMND vào vé, cập nhật CapNhatTrangThai.

d) Hủy vé
- Nhập MACB và CMND, tìm vé tương ứng (theo SOCMND), xóa CMND khỏi vé (set '\0'), CapNhatTrangThai.

e) In danh sách hành khách của 1 chuyến bay (format)
- Header: "DANH SACH HANH KHACH THUOC CHUYEN BAY <MACB>"
- Ngày giờ: dd/mm/yyyy hh:mm (zero‑pad)
- Noi den: <SANBAYDEN>
- Bảng: STT | SO VE | SO CMND | HO TEN | PHAI
- In theo thứ tự số vé tăng.

f) In danh sách chuyến bay khởi hành trong ngày dd/mm/yyyy đến nơi X mà còn vé
- Lọc: TGKHOIHANH ngày/tháng/năm đúng + SANBAYDEN == X + DemVeConTrong > 0
- In: MACB | HH:MM | Ve trong (số vé trống)

g) In danh sách vé còn trống của chuyến (theo MACB)
- Liệt kê số vé chưa đặt (SOVE).

h) Thống kê số lượt bay cho từng máy bay
- Với tất cả node chuyến, đếm số chuyến có TRANGTHAI == 3 cho mỗi SOHIEU.
- Sắp xếp giảm dần theo số lượt => in SOHIEU : SỐ_LƯỢT.

2) GIAO DIỆN (CLI) — chi tiết, không trùng lặp chức năng
- Nguyên tắc: mỗi chức năng duy nhất mapped 1 menu item; con trỏ/mũi tên di chuyển giữa item; Enter chọn; Esc/Back về menu trước.
- Kiến trúc menu:
  - Main menu: "May Bay", "Chuyen Bay", "Ve & Hanh Khach", "Thong Ke / Bao Cao", "Thoat"
  - Submenu "May Bay": Them / Xoa / Sua / Hien Thi / Back
  - Submenu "Chuyen Bay": Lap moi / Sua ngay gio / Huy / Hien Thi tat ca (tu day dùng ThemChuyenBay/TimChuyenBay) / Back
  - Submenu "Ve & Hanh Khach": Dat ve / Huy ve / In danh sach hanh khach (theo MACB) / In ve con trong / Back
  - Submenu "Thong Ke / Bao Cao": In chuyen bay theo ngay+noiDen / Thong ke so luot bay / Back
- Điều hướng:
  - Sử dụng mã phím ANSI: mũi tên lên/xuống để chọn dòng, Enter để chọn, Esc để thoát menu con.
  - Nếu triển khai không muốn xử lý phím ANSI, fallback: vẫn có lựa chọn số (1..n) và Enter; arrow keys là nâng cấp UI.
- Input patterns:
  - Tất cả input chuỗi: trim, kiểm tra chiều dài; CMND tối đa 15 ký tự; ghi rõ thông báo lỗi nếu không hợp lệ.
  - Các thao tác thay đổi/hủy có confirm (Y/N).
- Tránh trùng lặp:
  - Không tạo hai hàm ThemMayBay trong hai file.
  - Menu chỉ gọi các hàm đã có (ThemMayBay, XoaMayBay, SuaMayBay, KhoiTaoVe, ThemChuyenBay, TimChuyenBay, DatVe/HuyVe, ThemHanhKhach, TimHanhKhach, InDanhSachHanhKhach, InChuyenBayTheoNgayVaNoiDen, ThongKeSoLuotBay, SapXepGiamDanTheoSoLuot).
  - Nếu cần helper UI logic, tạo file ui.cpp chứa chỉ logic hiển thị/điều hướng; không lặp business logic.

3) RÀNG BUỘC DỮ LIỆU (invariants)
- SOHIEU unique trong dsmb.
- MACB unique trong danh sách chuyến; danh sách chuyến luôn sắp xếp tăng theo MACB.
- Trên 1 chuyến, mỗi CMND chỉ được 1 vé.
- Nếu máy bay bị xóa khi còn chuyến đã lập: từ chối xóa hoặc yêu cầu chuyển chuyến trước.
- Mọi bộ nhớ động (DSVE.ds, dsmb.nodes[i], cây hành khách nodes) phải được giải phóng khi hủy tương ứng hoặc khi kết thúc.

4) LƯU / ĐỌC FILE (ghi chú)
- Maybay: LuuMayBayFile / DocMayBayFile (đã có).
- Chuyen bay, hanh khach: dùng các hàm tương ứng đã định nghĩa.
- Khi load: khôi phục DSVE cho mỗi chuyến nếu cần — sau khi đọc chuyến và biết SOCHO, gọi KhoiTaoVe nếu cần.

5) GHI CHÚ THỰC HIỆN NHẸ (hướng dẫn code)
- Implement UI loop trong main.cpp hoặc tách ra ui.cpp; UI chỉ điều khiển nhập và gọi các hàm nghiệp vụ hiện có.
- Để hỗ trợ mũi tên trong UNIX/mac: implement readKey() với termios; trên Windows dùng _getch(). Có fallback dùng nhập số.
- Test kịch bản chính: thêm máy bay → lập chuyến → đặt vé (với HK mới và HK đã có) → in danh sách → hủy vé → thống kê.

6) Tóm tắt giao diện tương tác (ví dụ)
- Main:
  > ▶ May Bay
    Chuyen Bay
    Ve & Hanh Khach
    Thong Ke / Bao Cao
    Thoat
- Người dùng bấm ↓ để chọn "Chuyen Bay", Enter → submenu;
- Mỗi form nhập có validate, thông báo lỗi màu nếu muốn (CLI).

KẾT LUẬN: Tài liệu này là bản thiết kế chi tiết để hiện thực các yêu cầu a→h và UI. Tiếp theo, tôi có thể:
- a) Thêm file ui.cpp chứa menu/điều hướng (arrow keys + fallback);
- b) Sửa main.cpp để gọi ui; hoặc
- c) Triển khai trực tiếp interactive menu trong main.cpp theo spec.

Chọn 1 trong (a,b,c) để tôi cung cấp patch mã nguồn tối thiểu tương ứng.
