#include "bgcp_net.h"


bool check_bgcp_protocol(uint8_t *data, uint16_t size)
{
    if (size < MIN_BGCP_MES_SIZE || data[0] != 0xFD || data[1] != 0xFD) {
        return false;
    }

    uint16_t chksum1 = 0, chksum2 = 0;
    for (uint16_t i = 2; i <= size - 3; i++) {
        chksum1 += data[i];
    }
    chksum2 = (uint16_t)(data[size - 1] << 8) | (uint16_t)(data[size - 2]);
    return (chksum1 == chksum2);
}