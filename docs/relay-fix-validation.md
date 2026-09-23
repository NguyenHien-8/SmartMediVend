# Kiểm chứng bản sửa relay — 2026-09-23

## Đã đạt

- Test hồi quy chạy trên hàm mô phỏng cũ: thất bại vì SIG chưa được cấu hình
  và không có điều khiển vật lý. Sau khi bổ sung driver, 5/5 test relay đạt
  khi chạy riêng bằng Python 3.12 và MinGW g++ 13.1.
- Kiểm tra đủ 16 địa chỉ, LOW 500000 µs rồi HIGH trên timer giả lập, trạng
  thái khởi động, lệnh sai/chồng xung, lỗi tạo và khởi động timer.
- Test `test_zero_length_lace_is_processed_without_more_input` đạt khi chạy riêng.
- `clang-format --dry-run -Werror` đạt cho các file C++ mới và các vùng sửa
  trong file hiện hữu. `git diff --check` đạt.
- Rà soát độc lập driver không phát hiện vấn đề cần sửa.
- ESP-IDF v6.1: lệnh sau kết thúc mã 0, build và merge-bin thành công:

```text
python scripts/build.py smartmedivend-s3 --name smartmedivend-s3
```

Build đã biên dịch ứng dụng, driver board/relay, giao diện, WebSocket và
MQTT/UDP. App binary là `0x3af140` byte, còn 6% trong partition `0x3f0000`.
Các cảnh báo Kconfig/include trong SDK và managed components vẫn tồn tại;
không chỉnh sửa vendor output.

## Cấu hình và firmware

Build mặc định trên chọn tiếng Trung và bật wake word. Cấu hình trước khi
sửa dùng tiếng Việt, wake word tắt. Lệnh build lại để giữ cấu hình đó cần
chạy SDK ngoài sandbox (Python SDK bị `Access is denied` trong sandbox),
nhưng yêu cầu quyền cho lượt build này bị từ chối:

```text
python scripts/build.py smartmedivend-s3 --name smartmedivend-s3 --language vi-VN --wake-word disabled
```

Đã khôi phục `sdkconfig` ban đầu từ bản sao, xác minh SHA-256 giống hệt.
Do đó binary còn trong `build/` thuộc bản build mặc định; cần build lại
bằng lệnh tiếng Việt ở trên trước khi dùng binary đó cho cấu hình hiện tại.
Không nạp firmware vào thiết bị trong phiên này.

## Bộ host test tổng chưa đạt toàn bộ

Lệnh `python -m unittest discover -s scripts/tests -v` báo 84 test đã chạy,
8 error và 1 skipped. Chi tiết:

| Test / bước | Kết quả quan sát |
|---|---|
| `test_configured_build_options_are_verified` | WinError 32 khi xóa thư mục tạm đang là cwd |
| `test_disabled_build_options_accept_symbols_hidden_by_kconfig` | Cùng lỗi dọn thư mục trên Windows |
| `test_configure_build_replaces_stale_sdkconfig_backup` | Cùng lỗi dọn thư mục trên Windows |
| `test_configure_build_uses_all_cmake_values_in_one_run` | Cùng lỗi dọn thư mục trên Windows |
| `test_zip_is_always_recreated` | Cùng lỗi dọn thư mục trên Windows |
| `test_zero_length_lace_is_processed_without_more_input` | g++ trả mã 1 trong suite tổng; chạy riêng đạt |
| `SmvRelayTest.setUpClass` | g++ trả mã 1 trong suite tổng; chạy riêng đủ 5 test đạt |
| `test_missing_inputs_do_not_touch_existing_assets` | Thiếu ffmpeg/ffprobe trên PATH |
| `test_16_recordings_produce_24k_mono_60ms_opus` | Skipped vì thiếu ffmpeg/ffprobe |

Các test build hiện hữu đổi cwd vào thư mục tạm và khôi phục cwd sau khi
thoát context dọn thư mục, gây lỗi trên Windows. Chưa kết luận nguyên nhân
g++ thất bại riêng trong suite tổng; không chỉnh sửa các test/build/audio
ngoài phạm vi bản sửa relay.

## Còn cần phần cứng

Chưa đo SIG/IN relay, tiếp điểm, EN, nguồn MUX, mức kéo lên hoặc dòng kích
trên mạch thực. Cần kiểm tra reset, từng relay, xung khi audio/mạng đang
hoạt động và hành vi SKU/TTS sau khi nạp. Timer task có độ trễ lập lịch;
500 ms ở test giả lập không chứng minh độ rộng xung cơ khí thực tế.

Xem hướng dẫn đấu nối và log tại
[`main/boards/smartmedivend-s3/README.md`](../main/boards/smartmedivend-s3/README.md).
