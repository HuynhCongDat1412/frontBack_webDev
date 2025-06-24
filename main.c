#include "lib/mongoose.h"
#include <stdio.h>
#include <string.h>

static const char *s_listen_addr = "http://0.0.0.0:8080";
static const char *s_web_dir = "web";

// Hàm callback
static void fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    // Đăng nhập qua HTTP POST /api/login
    if (hm->method.len == 4 && memcmp(hm->method.buf, "POST", 4) == 0 &&
        hm->uri.len == strlen("/api/login") &&
        memcmp(hm->uri.buf, "/api/login", hm->uri.len) == 0) {

      char *user = mg_json_get_str(hm->body, "$.username");
      char *pass = mg_json_get_str(hm->body, "$.password");

      if (user && pass && strcmp(user, "admin") == 0 && strcmp(pass, "123") == 0) {
        mg_http_reply(c, 200, "Content-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n", "✅ Login OK\n");
      } else {
        mg_http_reply(c, 401, "Content-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n", "❌ Login Failed\n");
      }
      return;
    }

    // Xử lý preflight CORS (nếu cần)
    if (hm->method.len == 7 && strncmp(hm->method.buf, "OPTIONS", 7) == 0) {
      mg_http_reply(c, 200,
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n", "");
      return;
    }

    // Nếu là WebSocket upgrade
    if (hm->method.len == 3 && strncmp(hm->method.buf, "GET", 3) == 0 &&
        hm->uri.len == 1 && hm->uri.buf[0] == '/' &&
        mg_http_get_header(hm, "Upgrade") != NULL) {
      mg_ws_upgrade(c, hm, NULL);
      return;
    }

    // Phục vụ file tĩnh (HTML, JS, ...)
    struct mg_http_serve_opts opts = {.root_dir = s_web_dir};
    mg_http_serve_dir(c, hm, &opts);
  }
  else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    // Nhận tin nhắn chat và gửi lại cho client (echo)
    char buf[512];
    snprintf(buf, sizeof(buf), "%.*s", (int)wm->data.len, wm->data.buf);

    // Nếu là tin nhắn chat dạng JSON
    char msg[256] = {0};
    char *m = strstr(buf, "\"message\":\"");
    if (m && sscanf(m, "\"message\":\"%255[^\"]", msg) == 1) {
      char out[350];
      snprintf(out, sizeof(out), "{\"type\":\"chat\",\"message\":\"server gửi lại: %s\"}", msg);
      mg_ws_send(c, out, strlen(out), WEBSOCKET_OP_TEXT);
    }
  }
}

int main(void) {
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  struct mg_connection *conn = mg_http_listen(&mgr, s_listen_addr, fn, NULL);
  if (conn == NULL) {
    fprintf(stderr, "❌ Cannot start server on %s\n", s_listen_addr);
    return 1;
  }

  printf("🌐 Server running at http://localhost:8080\n");

  for (;;) mg_mgr_poll(&mgr, 100);
  mg_mgr_free(&mgr);
  return 0;
}