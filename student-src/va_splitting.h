#pragma once

#include "mmu.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 1 --------------------------------------
 * Checkout PDF Section 3 For this Problem
 *
 * Split the virtual address into its virtual page number and offset.
 * 
 * HINT: 
 *      -Examine the global defines in pagesim.h, which will be necessary in 
 *      implementing these functions.
 * ----------------------------------------------------------------------------------
 */
static inline vpn_t vaddr_vpn(vaddr_t addr) {
    vpn_t vpn = addr >> OFFSET_LEN;
    return vpn;
}

static inline uint16_t vaddr_offset(vaddr_t addr) {
    
    return (addr << VADDR_LEN) >> VADDR_LEN;
}

#pragma GCC diagnostic pop