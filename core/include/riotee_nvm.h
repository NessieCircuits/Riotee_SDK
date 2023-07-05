/**
 * @defgroup nvm Non-volatile memory driver
 * @{
 */

#ifndef __RIOTEE_NVM_H_
#define __RIOTEE_NVM_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Initializes non-volatile memory.
 *
 * @return int 0 on success, <0 else.
 */
int nvm_init();

/**
 * @brief Prepares NVM for write transfer(s).
 *
 * @param address Start address that data is written to.
 * @return int 0 on success, <0 else.
 */
int nvm_begin_write(uint32_t address);

/**
 * @brief Prepares NVM for read transfer(s).
 *
 * @param address Start address that data is from from.
 * @return int 0 on success, <0 else.
 */
int nvm_begin_read(uint32_t address);

/**
 * @brief Writes data to NVM.
 *
 * @param src Pointer to data.
 * @param size Number of bytes to write.
 * @return int 0 on success, <0 else.
 */
int nvm_write(uint8_t* src, size_t size);

/**
 * @brief Reads data from the NVM.
 *
 * @param dst Pointer to destination buffer.
 * @param size Number of bytes to read.
 * @return int 0 on success, <0 else.
 */
int nvm_read(uint8_t* dst, size_t size);

/**
 * @brief Ends read/write transfer.
 *
 * @return int 0 on success, <0 else.
 */
int nvm_end(void);

#endif /** @} __RIOTEE_NVM_H_ */
