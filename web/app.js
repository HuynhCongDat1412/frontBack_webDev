const gateway = `ws://localhost:8080`;
var websocket;

window.addEventListener('load', function() {
    document.getElementById('chatContainer').style.display = 'none';
    document.getElementById('loginForm').style.display = 'block';
});

function login() {
    const user = document.getElementById('username').value;
    const pass = document.getElementById('password').value;

    fetch('/api/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username: user, password: pass })
    })
    .then(res => res.text())
    .then(text => {
        if (text.includes('OK')) {
            document.getElementById('loginForm').style.display = 'none';
            document.getElementById('chatContainer').style.display = 'block';
            initWebSocket();
        } else {
            document.getElementById('loginError').innerText = 'Sai tài khoản hoặc mật khẩu!';
        }
    })
    .catch(() => {
        document.getElementById('loginError').innerText = 'Không thể kết nối server!';
    });
}

function initWebSocket() {
    websocket = new WebSocket(gateway);
    websocket.onopen = function() {
        // Đã kết nối WebSocket, không cần gửi thông tin đăng nhập nữa
    };
    websocket.onmessage = function(event) {
        const msg = event.data;
        try {
            const data = JSON.parse(msg);
            if (data.type === "chat") {
                showMessage(data.message);
            }
        } catch {
            showMessage(msg);
        }
    };
    websocket.onclose = function() {
        // Xử lý nếu cần
    };
}

function showMessage(message) {
    const receivedTextArea = document.getElementById('received');
    receivedTextArea.value += message + '\n';
}

function sendMsg() {
    const messageInput = document.getElementById('message');
    const message = messageInput.value;
    websocket.send(JSON.stringify({ type: "chat", message: message }));
    messageInput.value = '';
}