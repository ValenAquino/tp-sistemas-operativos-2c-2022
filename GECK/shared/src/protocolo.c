#include "../include/protocolo.h"

bool send_debug(int fd) {
    op_code cop = DEBUG;
    if (send(fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
        return false;
    }
    return true;
}
