#include "ota_update_stream.h"
#include "ota_internal.h"
#include "device.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "mbedtls/pk.h"
#include "mbedtls/gcm.h"
#include "esp_partition.h"
#include "esp_system.h"

DEF_EMB_FILE(rsa_private_pem);

static const char *TAGE = "OTA ERR";





#define AES_KEY_SIZE_BITS                256
#define AES_KEY_SIZE_BYTES               (AES_KEY_SIZE_BITS / 8)




bool ota_update_stream(const ota_input_stream_t *stream,
                       ota_check_header_fn check_header,
                       ota_process_chunk_fn process_chunk,
                       void *ctx)
{
    bool res;
    const emb_file_t rsa_key_file = EMBEDDED_FILE(rsa_private_pem);
    esp_enc_img_rsa_header_t header;

    // 1. Зчитування та перевірка заголовка
    res = stream->read(stream->ctx, (uint8_t *)&header, sizeof(header)) == sizeof(header);
    CHECK_TRUE_AND_GOTO(res, "READ HEADER", exit);

    res = check_header((const uint8_t *)&header, sizeof(header), ctx);
    CHECK_TRUE_AND_GOTO(res, "CHECK HEADER", exit);

    // 2. Ініціалізація RSA ключа
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    res = mbedtls_pk_parse_key(&pk,
                                (const unsigned char *)rsa_key_file.start,
                                rsa_key_file.size,
                                NULL, 0, NULL, NULL) == 0;
    CHECK_TRUE_AND_GOTO(res, "RSA KEY PARSE", free_pk);

    // 3. Дешифрування AES-ключа
    uint8_t aes_key[AES_KEY_SIZE_BYTES] = {0};
    size_t actual_len = 0;
    res = mbedtls_pk_decrypt(&pk,
                              header.enc_gcm_key, sizeof(header.enc_gcm_key),
                              aes_key, &actual_len,
                              sizeof(aes_key),
                              NULL, NULL) == 0;
    CHECK_TRUE_AND_GOTO(res, "AES KEY DECRYPT", free_pk);

    res = actual_len == sizeof(aes_key);
    CHECK_TRUE_AND_GOTO(res, "AES KEY LENGTH", free_pk);
    mbedtls_pk_free(&pk);

    // 4. Ініціалізація AES-GCM
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    res = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, aes_key, AES_KEY_SIZE_BITS) == 0;
    CHECK_TRUE_AND_GOTO(res, "GCM KEY SETUP", free_gcm);

    res = mbedtls_gcm_starts(&gcm, MBEDTLS_GCM_DECRYPT,
                              header.iv, sizeof(header.iv)) == 0;
    CHECK_TRUE_AND_GOTO(res, "GCM START", free_gcm);

    // 5. Читання, розшифрування та передача чанкiв
    uint32_t remaining = read_u32_le(header.bin_size);
    uint8_t buf_in[1024];
    uint8_t buf_out[1024];

    while (remaining > 0) {
        size_t chunk_len = (remaining > sizeof(buf_in)) ? sizeof(buf_in) : remaining;
    
        res = stream->read(stream->ctx, buf_in, chunk_len) == chunk_len;
        CHECK_TRUE_AND_GOTO(res, "CHUNK READ", free_gcm);
    
        size_t decrypted_len = 0;
        res = esp_aes_gcm_update(&gcm, buf_in, chunk_len, buf_out, sizeof(buf_out), &decrypted_len) == 0;
        CHECK_TRUE_AND_GOTO(res, "GCM UPDATE", free_gcm);
    
        res = process_chunk(buf_out, decrypted_len, ctx);
        CHECK_TRUE_AND_GOTO(res, "PROCESS CHUNK", free_gcm);
    
        remaining -= chunk_len;
    }
    // 6. Перевірка автентифікаційного тега
    uint8_t tag_check[16];
    size_t dummy_out_len = 0;
    res = mbedtls_gcm_finish(&gcm, NULL, 0, &dummy_out_len, tag_check, sizeof(tag_check)) == 0;
    CHECK_TRUE_AND_GOTO(res, "GCM FINISH FAILED", free_gcm);
    
    res = memcmp(tag_check, header.auth_tag, sizeof(tag_check)) == 0;
    CHECK_TRUE_AND_GOTO(res, "AUTH TAG MISMATCH", free_gcm);
    
    mbedtls_gcm_free(&gcm);
    return true;

free_gcm:
    mbedtls_gcm_free(&gcm);
free_pk:
    mbedtls_pk_free(&pk);
exit:
    return false;
}

bool check_header(const uint8_t *header_raw, size_t len, void *ctx) 
{
    if (len < sizeof(esp_enc_img_rsa_header_t)) {
        ESP_LOGE(TAGE, "Header too small");
        return false;
    }
    const esp_enc_img_rsa_header_t *hdr = (const esp_enc_img_rsa_header_t *)header_raw;
    if (memcmp(hdr->magic, ESP_ENC_HEADER_MAGIC, 4) != 0) {
        ESP_LOGE(TAGE, "Invalid magic");
        return false;
    }
    uint32_t bin_size = read_u32_le(hdr->bin_size);
    if (bin_size == 0 || bin_size > BIN_SIZE) {
        ESP_LOGE(TAGE, "Invalid bin size: %u", (unsigned)bin_size);
        return false;
    }
    ESP_LOGI(TAGE, "Header OK. Size: %u bytes", (unsigned)bin_size);
    return true;
}



typedef struct {
    esp_ota_handle_t handle;
    const esp_partition_t *partition;
    size_t written;
} ota_ctx_t;

bool process_chunk(const uint8_t *chunk, size_t len, void *user_ctx) 
{
    ota_ctx_t *ctx = (ota_ctx_t *)user_ctx;
    if (ctx->handle == 0) {
        ctx->partition = esp_ota_get_next_update_partition(NULL);
        if (!ctx->partition) {
            ESP_LOGE(TAGE, "No OTA partition found");
            return false;
        }
        if (esp_ota_begin(ctx->partition, OTA_SIZE_UNKNOWN, &ctx->handle) != ESP_OK) {
            ESP_LOGE(TAGE, "OTA begin failed");
            return false;
        }
        ctx->written = 0;
    }
    if (esp_ota_write(ctx->handle, chunk, len) != ESP_OK) {
        ESP_LOGE(TAGE, "OTA write failed at %u", ctx->written);
        return false;
    }
    ctx->written += len;
    return true;
}


esp_err_t complete_ota(ota_ctx_t *ctx) 
{
    if (esp_ota_end(ctx->handle) != ESP_OK) {
        ESP_LOGE(TAGE, "OTA end failed");
        return ESP_FAIL;
    }
    if (esp_ota_set_boot_partition(ctx->partition) != ESP_OK) {
        ESP_LOGE(TAGE, "Set boot partition failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAGE, "OTA success. Rebooting...");
    esp_restart();
    return ESP_OK;
}