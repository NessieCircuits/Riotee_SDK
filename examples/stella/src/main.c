
#include "riotee.h"
#include "riotee_stella.h"
#include "riotee_timing.h"
#include "printf.h"

static unsigned int counter = 0;

/* Buffer for receiving incoming packet. */
uint8_t rx_buf[RIOTEE_STELLA_MAX_DATA];

void reset_callback(void) {
  riotee_stella_init();
}

int main() {
  riotee_rc_t rc;
  for (;;) {
    riotee_wait_cap_charged();
    rc = riotee_stella_transceive(rx_buf, sizeof(rx_buf), &counter, sizeof(counter));
    if (rc < 0)
      printf("Error %d\r\n", rc);
    else if (rc == 0)
      printf("Successful transmission. No data received.\r\n");
    else
      printf("Successful transmission. Received %d bytes from basestation.\r\n");

    counter++;
  }
}
