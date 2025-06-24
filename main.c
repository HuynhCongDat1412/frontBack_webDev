#include "lib/mongoose.h"
#include <stdio.h>
#include <string.h>

static const char *s_listen_addr = "http://0.0.0.0:8080";

struct client_data {
  int logged_in;
};

static void fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (hm->uri.len == 1 && hm->uri.buf[0] == '/') {
      mg_ws_upgrade(c, hm, NULL);
      c->fn_data = calloc(1, sizeof(struct client_data));
    }
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    struct client_data *cd = (struct client_data *)c->fn_data;

    char buf[256];
    snprintf(buf, sizeof(buf), "%.*s", (int)wm->data.len, wm->data.buf);

    // Nếu chưa đăng nhập, kiểm tra đăng nhập
    if (!cd->logged_in) {
      char user[64], pass[64];
      int ok = 0;
      if (sscanf(buf, "{\"type\":\"login\",\"username\":\"%63[^\"]\",\"password\":\"%63[^\"]\"}", user, pass) == 2) {
        if (strcmp(user, "admin") == 0 && strcmp(pass, "123") == 0) ok = 1;
      }
      if (ok) {
        cd->logged_in = 1;
        mg_ws_send(c, "{\"type\":\"login\",\"success\":true}", 31, WEBSOCKET_OP_TEXT);
      } else {
        mg_ws_send(c, "{\"type\":\"login\",\"success\":false}", 32, WEBSOCKET_OP_TEXT);
      }
      return;
    }

    // Đã đăng nhập, xử lý chat
    char msg[256];
    if (sscanf(buf, "{\"type\":\"chat\",\"message\":\"%255[^\"]\"}", msg) == 1) {
      char out[300];
      snprintf(out, sizeof(out), "{\"type\":\"chat\",\"message\":\"%s\"}", msg);
      mg_ws_send(c, out, strlen(out), WEBSOCKET_OP_TEXT);
    }
  } else if (ev == MG_EV_CLOSE) {
    free(c->fn_data);
  }
}

int main(void) {
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  struct mg_connection *conn = mg_http_listen(&mgr, s_listen_addr, fn, NULL);
  if (conn == NULL) {
    fprintf(stderr, "❌ Failed to create listener on %s\n", s_listen_addr);
    return 1;
  }

  printf("🌐 WebSocket login/chat server running at ws://localhost:8080\n");

  for (;;) mg_mgr_poll(&mgr, 100);
  mg_mgr_free(&mgr);
  return 0;
}