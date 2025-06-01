#include "device_memory.h"


#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "device.h"

static const char *SPACE_NAME = "nvs";


bool init_nvs()
{
    esp_err_t e = nvs_flash_init();
    if (e == ESP_ERR_NVS_NO_FREE_PAGES || e == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        CHECK_RET_FALSE(nvs_flash_erase());
        e = nvs_flash_init();
    }
    return e == ESP_OK;
}

bool read_flash(const char* data_name, void *data_buf, unsigned data_size)
{
    nvs_handle_t nvs_handle;
    if(data_size == 0 || data_buf == NULL)return ESP_ERR_INVALID_ARG;
    CHECK_AND_GOTO(nvs_open(SPACE_NAME, NVS_READONLY, &nvs_handle), exit);
    CHECK_AND_GOTO(nvs_get_blob(nvs_handle, data_name, data_buf, &data_size), exit);
    nvs_close(nvs_handle);
    return true;
exit:
    nvs_close(nvs_handle);
    return false;
}

bool write_flash(const char* data_name, const void *data, unsigned data_size)
{
    nvs_handle_t nvs_handle;
    if(data_size == 0 || data == NULL)return ESP_ERR_INVALID_ARG;
    CHECK_AND_GOTO(nvs_open(SPACE_NAME, NVS_READWRITE, &nvs_handle), exit);
    CHECK_AND_GOTO(nvs_set_blob(nvs_handle, data_name, data, data_size), exit);
    CHECK_AND_GOTO(nvs_commit(nvs_handle), exit);
    nvs_close(nvs_handle);
    return true;
exit:
    nvs_close(nvs_handle);
    return false;
}