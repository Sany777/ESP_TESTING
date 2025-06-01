#ifndef OTA_UPDATE_STREAM_H
#define OTA_UPDATE_STREAM_H


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef bool (*ota_check_header_fn)(const uint8_t *header, size_t len, void *ctx);
typedef bool (*ota_process_chunk_fn)(const uint8_t *data, size_t len, void *ctx);

typedef struct {
    void *ctx;
    int (*read)(void *ctx, uint8_t *buf, size_t len); 
} ota_input_stream_t;


bool ota_update_stream(const ota_input_stream_t *stream,
                      ota_check_header_fn check_header,
                      ota_process_chunk_fn process_chunk,
                      void *ctx);


                      


#endif // OTA_UPDATE_STREAM_H                      