#include "sdcard.hpp"
#include "camera.hpp"

extern "C"{
    #include <esp_log.h>
    #include <esp_system.h>
    #include <sys/param.h>
    #include <string.h>

};




#define BOARD_ESP32CAM_AITHINKER 1


#include <iostream>




extern "C" void app_main(void)
{
    char data[100] = "text to save\0";
    sdcard->writeTextFile("long_.txt", data);
    cameraInit();

    while (1)
    {

    camera_example_capture();

        // camera_fb_t *pic = esp_camera_fb_get();
        // use pic->buf to access the image
        // ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
        // snprintf(filename, 50, MOUNT_POINT "/%d_pic.jpg", ++id);
        // save_bin(filename, pic->buf, pic->len);
        // esp_camera_fb_return(pic);
        // led_on(false);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

}

