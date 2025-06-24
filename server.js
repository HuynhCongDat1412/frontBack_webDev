const http = require('http');
const fs = require('fs');
const path = require('path');
const WebSocket = require('ws');

// Tạo HTTP server để phục vụ file HTML
const server = http.createServer((req, res) => {
    if (req.url === "/") {
        fs.readFile(path.join(__dirname, 'index.html'), (err, data) => {
            if (err) {
                res.writeHead(500);
                res.end("Error loading index.html");
            } else {
                res.writeHead(200, {'Content-Type': 'text/html'});
                res.end(data);
            }
        });
    } else {
        res.writeHead(404);
        res.end("Not found");
    }
});

// Tạo WebSocket server trên cùng HTTP server
const wss = new WebSocket.Server({ server });

wss.on('connection', (ws) => {
    console.log("✅ Client connected!");

    ws.on('message', (message) => {
        console.log(`📩 Received: ${message}`);
        // Phản hồi lại client
        ws.send(`Server received: ${message}`);
    });

    ws.on('close', () => {
        console.log("❌ Client disconnected.");
    });
});

// Server lắng nghe ở cổng 8080
const PORT = 8080;
server.listen(PORT, () => {
    console.log(`🌐 Server running at http://localhost:${PORT}/`);
});
