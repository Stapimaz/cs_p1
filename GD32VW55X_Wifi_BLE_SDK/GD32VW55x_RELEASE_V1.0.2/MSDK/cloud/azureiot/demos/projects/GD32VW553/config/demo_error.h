/* Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License. */

#ifndef DEMO_ERROR_H
#define DEMO_ERROR_H

/* FreeRTOS config include. */
#include "FreeRTOSConfig.h"
#include "trng.h"
//#ifdef USE_THIRDPARTY_MBEDTLS
/**
 * \brief Translate the high-level part of an Mbed TLS error code into a string
 *        representation.
 *
 * This function returns a const pointer to an un-modifiable string. The caller
 * must not try to modify the string. It is intended to be used mostly for
 * logging purposes.
 *
 * \param error_code    error code
 *
 * \return The string representation of the error code, or \c NULL if the error
 *         code is unknown.
 */
const char * mbedtls_high_level_strerr( int error_code );

/**
 * \brief Translate the low-level part of an Mbed TLS error code into a string
 *        representation.
 *
 * This function returns a const pointer to an un-modifiable string. The caller
 * must not try to modify the string. It is intended to be used mostly for
 * logging purposes.
 *
 * \param error_code    error code
 *
 * \return The string representation of the error code, or \c NULL if the error
 *         code is unknown.
 */
const char * mbedtls_low_level_strerr( int error_code );
//#endif
#endif /* DEMO_CONFIG_H */
