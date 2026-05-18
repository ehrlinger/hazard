#ifndef SETLIK_H
#define SETLIK_H

/**
 * @file setlik.h
 * @brief Log-likelihood function interface.
 * @see setlik.c
 */

#include "structures.h" /* Defines logical data type */

/**
 * @brief Retrieve the computed log-likelihood value.
 *
 * Called after SETLIK_obs_loop() completes. Copies the accumulated
 * log-likelihood from `Setobj.lnlike` into `*llike`.
 *
 * @param llike Output pointer; receives the log-likelihood value.
 */
void dllike(double *llike);

/**
 * @brief Inner observation loop for log-likelihood evaluation.
 *
 * Iterates over all observations, accumulates the log-likelihood
 * contributions from events, right-censored, and interval-censored
 * records, and precomputes derivative quantities if needed.
 *
 * @return TRUE on success, FALSE if a domain error was encountered.
 */
logical SETLIK_obs_loop(void);

#endif /* SETLIK_H */
