const gateway = `ws://localhost:8080`;
var websocket;

window.addEventListener('load', function() {
    document.getElementById('chatContainer').style.display = 'none';
    document.getElementById('loginForm').style.display = 'block';
});

function login() {
    const user = document.getElementById('username').value;
    const pass = document.getElementById('password').value;
    initWebSocket(user, pass);
}

function initWebSocket(user, pass) {
    websocket = new WebSocket(gateway);
    websocket.onopen = function() {
        // Gửi thông tin đăng nhập khi kết nối mở
        websocket.send(JSON.stringify({ type: "login", username: user, password: pass }));
    };
    websocket.onmessage = function(event) {
        const msg = event.data;
        try {
            const data = JSON.parse(msg);
            if (data.type === "login" && data.success) {
                document.getElementById('loginForm').style.display = 'none';
                document.getElementById('chatContainer').style.display = 'block';
            } else if (data.type === "login") {
                document.getElementById('loginError').innerText = 'Sai tài khoản hoặc mật khẩu!';
                websocket.close();
            } else if (data.type === "chat") {
                showMessage(data.message);
            }
        } catch {
            showMessage(msg);
        }
    };
    websocket.onclose = function() {
        // Nếu chưa login thành công thì không tự reconnect
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