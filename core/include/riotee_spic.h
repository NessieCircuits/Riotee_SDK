/**
 * @defgroup spic SPI controller driver
 *  @{
 */
#ifndef __RIOTEE_SPI_H_
#define __RIOTEE_SPI_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPIC_PIN_UNUSED (1UL << 31)

typedef enum {
  /** 125kHz Clock frequency. */
  SPIC_FREQUENCY_K125 = 0x02000000UL,
  /** 250kHz Clock frequency. */
  SPIC_FREQUENCY_K250 = 0x04000000UL,
  /** 500kHz Clock frequency. */
  SPIC_FREQUENCY_K500 = 0x08000000UL,
  /** 16MHz Clock frequency. */
  SPIC_FREQUENCY_M16 = 0x0A000000UL,
  /** 1MHz Clock frequency.*/
  SPIC_FREQUENCY_M1 = 0x10000000UL,
  /** 32MHz Clock frequency. */
  SPIC_FREQUENCY_M32 = 0x14000000UL,
  /** 2MHz Clock frequency. */
  SPIC_FREQUENCY_M2 = 0x20000000UL,
  /** 4MHz Clock frequency. */
  SPIC_FREQUENCY_M4 = 0x40000000UL,
  /** 8MHz Clock frequency. */
  SPIC_FREQUENCY_M8 = 0x80000000UL,
} riotee_spic_frequency_t;

typedef enum {
  /** */
  SPIC_MODE0_CPOL0_CPHA0,
  SPIC_MODE1_CPOL0_CPHA1,
  SPIC_MODE2_CPOL1_CPHA0,
  SPIC_MODE3_CPOL1_CPHA1,
} riotee_spic_mode_t;

typedef struct {
  /** SPI mode. */
  riotee_spic_mode_t mode;
  /** Clock frequency.*/
  riotee_spic_frequency_t frequency;
  /** Chip select pin. */
  unsigned int pin_cs;
  /** Clock pin. */
  unsigned int pin_sck;
  /** Controller Out Peripheral In pin. */
  unsigned int pin_copi;
  /** Controller In Peripheral Out pin. */
  unsigned int pin_cipo;
} riotee_spic_cfg_t;

/**
 * @brief Initializes the SPI peripheral. Must be called once after every reset before SPI can be used.
 *
 * @param cfg
 * @return int
 */
int spic_init(riotee_spic_cfg_t* cfg);

/**
 * @brief Transfers given number of bytes to and from the peripheral.
 *
 * @param data_tx Pointer to TX data. Can be NULL, if n_tx=0.
 * @param n_tx Number of bytes to transmit. Can be 0.
 * @param data_rx Pointer to RX data buffer. Can be NULL if n_rx=0.
 * @param n_rx Number of bytes to receive. Can be 0.
 * @return int 0 on success, <0 else.
 */
int spic_transfer(uint8_t* data_tx, size_t n_tx, uint8_t* data_rx, size_t n_rx);

#ifdef __cplusplus
}
#endif

#endif /* @} __RIOTEE_SPI_H_ */