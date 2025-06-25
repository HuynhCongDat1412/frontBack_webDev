# Demo Socket & MQTT & Web Login (C Compile)

## I. Task

- **Demo WebSocket:** Giao tiếp realtime giữa web và backend qua WebSocket.
- **Demo MQTT:** Client C subscribe và publish với MQTT broker.
- **Web đăng nhập:** Frontend web đăng nhập, giao tiếp với backend C.

*Lưu ý: Tất cả code backend đều viết bằng C, biên dịch được với gcc.*

---

## II. Demo cho WebSocket

### Tóm tắt hoạt động `websocketTest.c`

- Server lắng nghe tại `ws://localhost:8080`.
- Khi client (web) truy cập `/`, server upgrade lên WebSocket.
- Khi client gửi tin nhắn qua WebSocket, server nhận và gửi lại (echo) đúng nội dung đó.
- Khi client đóng kết nối, server log ra console.

### Web demo

- Trang web có **2 textarea**:
  - 1 textarea để nhập nội dung, nhấn "Gửi" sẽ gửi qua WebSocket cho backend.
  - 1 textarea hiển thị các tin nhắn nhận được từ backend qua WebSocket.
- Giao diện đơn giản, chỉ cần mở web là test được.
- Trong repo có file test WebSocket là `websocketTest.c` và file MQTT client là `mqttTest.c`. Cả hai đã test gửi/nhận thành công.

**Lệnh biên dịch WebSocket server:**
```sh
gcc websocketTest.c lib/mongoose.c -Ilib -o ws_server.exe -lws2_32
```

---

## III. Demo cho MQTT

### Tóm tắt hoạt động `mqttTest.c`

- Kết nối tới MQTT broker tại `127.0.0.1:1883`.
- Đăng ký (subscribe) topic `"helloBro"`.
- Định kỳ mỗi 3 giây, publish message `"hello bro"` lên topic này.
- Khi nhận được message từ broker, in ra màn hình.
- Client vừa publish vừa subscribe, kiểm tra được cả hai chiều.

**Lệnh biên dịch MQTT client:**
```sh
gcc mqttTest.c lib/mqtt.c -Ilib -o mqtt_client.exe -lws2_32
```

---

## IV. Web đăng nhập frontend giao tiếp backend

Repo có 2 branch hoạt động khác nhau

- **Branch `main`:**
  - Người dùng truy cập web frontend (có thể host ở ngoài).
  - Khi đăng nhập, web sẽ gửi thông tin đăng nhập qua WebSocket cho backend.
  - Backend kiểm tra thông tin, xác nhận lại với web.
  - Nếu hợp lệ, frontend sẽ chuyển sang trang chat demo.

- **Branch thứ 2:**
  - Người dùng truy cập trực tiếp `localhost`, backend sẽ phục vụ luôn giao diện web cho người dùng.
  - Người dùng đăng nhập, web gửi HTTP POST tới backend để xác nhận đăng nhập.
  - Nếu hợp lệ, frontend sẽ chuyển sang trang chat demo (giao tiếp qua WebSocket).

  - Web đăng nhập gửi HTTP POST `/api/login` với username/password tới backend C.
  - Nếu đăng nhập thành công (admin/123), giao diện chuyển sang trang chat demo WebSocket.
  - Chat realtime qua WebSocket như phần II.
  - Backend phục vụ file tĩnh (HTML, JS) từ thư mục `web`, kiểm tra đăng nhập qua REST API đơn giản.

**Lệnh biên dịch server HTTP + WebSocket + REST API:**
```sh
gcc main.c lib/mongoose.c -Ilib -o server.exe -lws2_32
```

---

**Repo này gồm:**
- `websocketTest.c`: Server WebSocket thuần C.
- `mqttTest.c`: MQTT client thuần C, vừa subscribe vừa publish.
- `main.c`: Server HTTP + WebSocket + REST API đăng nhập.
- `web/`: Thư mục chứa web frontend (HTML, JS) cho đăng nhập và chat test