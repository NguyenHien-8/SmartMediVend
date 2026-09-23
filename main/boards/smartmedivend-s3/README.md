# SmartMediVend ESP32-S3: relay qua CD74HC4067

Board: ESP32-S3-WROOM-1-N16R8, TFT ST7789 240×320, micro INMP441,
loa I2S và 16 đầu vào relay tích cực LOW. Board/variant vẫn là
`smartmedivend-s3`; không thay đổi GPIO hoặc định danh OTA.

## Nguyên nhân relay không hoạt động

Nhánh khớp STT trong `main/application.cc` trước đây gọi
`smv::ReportTestRelay()`. Hàm này chỉ ghi
`SIMULATED C...; physical relay OFF / not configured`, không khởi tạo GPIO,
không chọn kênh MUX và không tạo xung. Log `docs/Log/Khop_Keyword.txt`
có thông báo này cho C16, C8 và C3. Vì vậy khớp keyword, hiển thị SKU và
hủy TTS vẫn hoạt động trong khi relay không nhận tín hiệu.

## Luồng xử lý sau khi sửa

1. Callback nhận JSON `type=stt`, kiểm tra chuỗi `text`, rồi đưa công việc
   vào `Application::Schedule()` để xử lý trên task ứng dụng.
2. `MatchTestKeyword()` dùng nguyên quy tắc hiện có: chuẩn hóa tiếng Việt,
   khớp cụm từ có ranh giới, ưu tiên cụm dài hơn (`đau bụng kinh` trước
   `đau bụng`), loại câu phủ định/câu hỏi và trường hợp nhiều keyword độc lập.
3. Gate chỉ chấp nhận quyết định đầu tiên khi đang `Waiting`. Nếu khớp,
   gate bỏ audio Xiaozhi và ứng dụng gọi `Board::PulseRelay(relay)`.
   Việc hiển thị SKU, gửi abort TTS và phát âm thanh nội bộ được giữ lại.
4. Board chuyển lời gọi sang `smv::MuxRelay`. Driver giữ SIG HIGH,
   đặt địa chỉ `channel = relay - 1`, chờ địa chỉ ổn định 2 µs rồi kéo SIG LOW.
5. Timer một lần được đặt `500000` µs. Callback đưa SIG HIGH để ngắt relay,
   độc lập với mạng, TTS, giải mã MP3 và vòng lặp ứng dụng.

Timer không chặn task ứng dụng 500 ms. Độ trễ lập lịch timer có thể khiến
xung thực tế dài hơn 500 ms một chút; log `elapsed` giúp đối chiếu.
Thời gian tiếp điểm cơ khí đóng/mở còn phụ thuộc module relay.

Mỗi lần chỉ một relay hoạt động. Lệnh đến khi đang có xung bị từ chối,
không xếp hàng và không kéo dài xung đang chạy. Nếu tạo timer thất bại,
driver giữ HIGH và không nhận lệnh. Nếu khởi động timer thất bại, driver
trả HIGH ngay. Giao diện báo `RELAY: BẬN / LỖI` khi lệnh không được nhận.
Timer/controller có vòng đời toàn bộ firmware, giống singleton `Board`.

## Đấu nối

| ESP32-S3 | Chân CD74HC4067 | Chức năng |
|---|---|---|
| GPIO39 | S0 | Bit thấp nhất |
| GPIO40 | S1 | Bit 1 |
| GPIO41 | S2 | Bit 2 |
| GPIO42 | S3 | Bit cao nhất |
| GPIO17 | SIG/COM | HIGH khi nghỉ, LOW trong xung |
| GND | EN | Cho phép MUX hoạt động; không có GPIO EN trong `BoardPins.h` |
| GND | GND | Mass chung phía tín hiệu điều khiển |

Nối MUX C0 → IN1 của relay C1, C1 → IN2, …, C15 → IN16.
Ví dụ: relay C1 có S3S2S1S0=`0000`; C8=`0111`; C16=`1111`.
Bảng keyword/SKU hiện có trong `smv_keyword_matcher.h` được giữ nguyên.

Điều kiện điện cần kiểm tra trên mạch thực tế:

- EN phải LOW; EN HIGH ngắt tất cả kênh, nên firmware vẫn có log ON/OFF
  nhưng đầu vào relay không nhận xung.
- Các kênh không được chọn là trở kháng cao, không phải đầu ra HIGH.
  Đầu vào relay phải có pull-up về mức ngắt tương thích. SIG và đầu vào
  relay cần được giữ ngắt bằng phần cứng trong thời gian ESP32 reset.
- GPIO ESP32-S3 dùng mức logic 3,3 V. Kiểm tra VCC MUX, ngưỡng S0–S3,
  điện áp kéo lên ở các IN relay và dòng kích qua MUX. Nếu phía relay
  kéo lên 5 V hoặc MUX cấp 5 V, cần kiểm tra/chọn mạch chuyển mức và tầng
  đệm phù hợp; không coi các mức điện này là tự động tương thích.
- MUX chỉ chuyển tín hiệu đầu vào module relay, không cấp điện cuộn dây.
  Module relay cần nguồn đúng thông số của chính module.

Tham khảo điện áp, trở kháng và chân enable trong
[datasheet CD74HC4067 của TI](https://www.ti.com/lit/ds/symlink/cd74hc4067.pdf).
Ảnh module chưa xác định được điện áp cấp hoặc sơ đồ đầu vào relay thực tế.

## Build và kiểm tra

Sau khi kích hoạt ESP-IDF 6.1 (tối thiểu 6.0.1):

```sh
idf.py --version
python scripts/build.py smartmedivend-s3 --name smartmedivend-s3 --language vi-VN --wake-word disabled
python -m unittest discover -s scripts/tests -v
```

Lệnh trên giữ tiếng Việt và chế độ nút nhấn (wake word tắt) của cấu hình
đang dùng khi sửa relay. Build script tạo lại cấu hình; không sửa `sdkconfig`
bằng tay.

Host test `test_smv_relay.py` cần compiler C++17 (biến `CXX`, mặc định `c++`).
Test dùng driver thật với GPIO/timer giả lập để kiểm tra đủ 16 kênh, HIGH
khi khởi động, LOW 500 ms rồi HIGH, lệnh sai/chồng xung và lỗi timer.

Trên phần cứng, kiểm tra GPIO17 và IN relay tương ứng bằng logic analyzer
hoặc oscilloscope. Log dự kiến khi nói keyword `thuốc hạ sốt`:

```text
SMV-Relay: C16 ON: MUX C15, SIG=LOW, pulse=500ms
SMV-Relay: C16 OFF: SIG=HIGH, elapsed=500...us
```

Kiểm tra thêm C1 và C8, đảm bảo các relay khác không đóng, SKU vẫn đúng,
TTS Xiaozhi bị chặn, âm thanh nội bộ hoạt động và câu không khớp vẫn nhận
TTS bình thường. Lặp lại lúc phát âm thanh hoặc mất mạng để đo thời gian
ngắt. Log GPIO và build thành công chưa xác nhận tiếp điểm relay đã đóng
hoặc vật phẩm đã được cấp; firmware hiện không có cảm biến phản hồi đó.
