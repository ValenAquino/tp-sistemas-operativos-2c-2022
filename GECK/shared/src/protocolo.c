#include "../include/protocolo.h"

static op_code recibir_cop(int fd) {
    op_code cop;
    if(recv(fd, &cop, sizeof(op_code), 0) != 0)
        return cop;
    else {
        close(fd);
        return -1;
    }
}

bool send_debug(int fd) {
    op_code cop = DEBUG;
    if (send(fd, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
        return false;
    }
    return true;
}
