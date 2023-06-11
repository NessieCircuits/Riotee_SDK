#ifndef __RIOTEE_SPI_H_
#define __RIOTEE_SPI_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SPIC_FREQUENCY_K125 = 0x02000000UL, /*!< 125 kbps */
  SPIC_FREQUENCY_K250 = 0x04000000UL, /*!< 250 kbps */
  SPIC_FREQUENCY_K500 = 0x08000000UL, /*!< 500 kbps */
  SPIC_FREQUENCY_M16 = 0x0A000000UL,  /*!< 16 Mbps */
  SPIC_FREQUENCY_M1 = 0x10000000UL,   /*!< 1 Mbps */
  SPIC_FREQUENCY_M32 = 0x14000000UL,  /*!< 32 Mbps */
  SPIC_FREQUENCY_M2 = 0x20000000UL,   /*!< 2 Mbps */
  SPIC_FREQUENCY_M4 = 0x40000000UL,   /*!< 4 Mbps */
  SPIC_FREQUENCY_M8 = 0x80000000UL,   /*!< 8 Mbps */
} riotee_spic_frequency_t;

typedef enum {
  SPIC_MODE0_CPOL0_CPHA0,
  SPIC_MODE1_CPOL0_CPHA1,
  SPIC_MODE2_CPOL1_CPHA0,
  SPIC_MODE3_CPOL1_CPHA1,
} riotee_spic_mode_t;

typedef struct {
  riotee_spic_mode_t mode;
  riotee_spic_frequency_t frequency;
  unsigned int pin_cs;
  unsigned int pin_sck;
  unsigned int pin_copi;
  unsigned int pin_cipo;
} riotee_spic_cfg_t;

int spic_init(riotee_spic_cfg_t* cfg);
int spic_transfer(uint8_t* data_tx, size_t n_tx, uint8_t* data_rx, size_t n_rx);

#ifdef __cplusplus
}
#endif

#endif /* __RIOTEE_SPI_H_ */