#include "ota_internal.h"
#include "ota_update_stream.h"
#include "ota_adapters.h"

#include "esp_log.h"
#include <string.h>
#include "driver/uart.h"


static int httpd_read(void *ctx, uint8_t *buf, size_t len) 
{
    httpd_req_t *req = (httpd_req_t *)ctx;
    int rlen = httpd_req_recv(req, (char *)buf, len);
    return (rlen > 0) ? rlen : 0;
}

void httpd_input_stream_init(ota_input_stream_t *stream, httpd_req_t *req) 
{
    stream->ctx = req;
    stream->read = httpd_read;
}



typedef struct {
    int uart_num;
} uart_ctx_t;

static int uart_read(void *ctx, uint8_t *buf, size_t len) 
{
    uart_ctx_t *uart_ctx = (uart_ctx_t *)ctx;
    int bytes_read = uart_read_bytes(uart_ctx->uart_num, buf, len, pdMS_TO_TICKS(1000));
    return bytes_read > 0 ? bytes_read : 0;
}

void uart_input_stream_init(ota_input_stream_t *stream, int uart_num) 
{
    uart_ctx_t *uart_ctx = malloc(sizeof(uart_ctx_t));
    if (!uart_ctx) {
        stream->ctx = NULL;
        stream->read = NULL;
        return;
    }
    uart_ctx->uart_num = uart_num;
    stream->ctx = uart_ctx;
    stream->read = uart_read;
}
