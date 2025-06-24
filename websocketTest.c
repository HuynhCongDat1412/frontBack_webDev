#include "lib/mongoose.h"
#include <stdio.h>

static const char *s_listen_addr = "http://0.0.0.0:8080";

// Hàm callback đúng chuẩn bản 7.18 (3 tham số)
static void fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    // Bản 7.18 không có mg_http_match_uri(), ta so sánh thủ công
    if (hm->uri.len == 1 && hm->uri.buf[0] == '/') {
      mg_ws_upgrade(c, hm, NULL);
    }
  } else if (ev == MG_EV_WS_OPEN) {
    printf("✅ WebSocket client connected\n");
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    printf("📩 Received from client: %.*s\n", (int) wm->data.len, wm->data.buf);
    mg_ws_send(c, wm->data.buf, wm->data.len, WEBSOCKET_OP_TEXT);
  } else if (ev == MG_EV_CLOSE) {
    printf("❌ WebSocket client disconnected\n");
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

  printf("🌐 WebSocket server running at ws://localhost:8080\n");

  for (;;) mg_mgr_poll(&mgr, 100);
  mg_mgr_free(&mgr);
  return 0;
}
