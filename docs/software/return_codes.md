# Return Codes

Most of the SDK's API functions return a `riotee_rc_t` that indicates the status of the transaction.

There are a number of common return codes defined in `core/include/riotee.h`.
Modules can additionally define their own return codes starting at a module-specific offset, which is also defined in `core/include/riotee.h`.

Long-running interactions with peripherals may be interrupted by a reset or a teardown event.
To ensure correct application execution, always check the return codes of such functions for the return values `RIOTEE_ERR_RESET` and `RIOTEE_ERR_TEARDOWN`.
If a function call failed with one of these return codes, the application can, for example, wait until the capacitor is fully charged with `riotee_wait_cap_charged()` before trying again.

