#include "types.h"
#include "pagesim.h"
#include "mmu.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);

pfn_t last_evicted = 0;

/**
 * --------------------------------- PROBLEM 7 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Make a free frame for the system to use. You call the select_victim_frame() method
 * to identify an "available" frame in the system (already given). You will need to 
 * check to see if this frame is already mapped in, and if it is, you need to evict it.
 * 
 * @return victim_pfn: a phycial frame number to a free frame be used by other functions.
 * 
 * HINTS:
 *      - When evicting pages, remember what you checked for to trigger page faults 
 *      in mem_access
 *      - If the page table entry has been written to before, you will need to use
 *      swap_write() to save the contents to the swap queue.
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn;
    victim_pfn = select_victim_frame();

    if (frame_table[victim_pfn].mapped) {

        pte_t* pt_base = (pte_t*)(mem + frame_table[victim_pfn].process->saved_ptbr * PAGE_SIZE);
        pte_t* pt = pt_base + frame_table[victim_pfn].vpn;

        if (pt->dirty) {
            swap_write(pt, mem + pt->pfn * PAGE_SIZE);
            pt->dirty = 0;
            stats.writebacks += 1;
        }

        pt->valid = 0;
        frame_table[victim_pfn].mapped = 0;
    }

    return victim_pfn;
}



/**
 * --------------------------------- PROBLEM 9 --------------------------------------
 * Checkout PDF section 7, 9, and 11 for this problem
 * 
 * Finds a free physical frame. If none are available, uses either a
 * randomized, FCFS, or clocksweep algorithm to find a used frame for
 * eviction.
 * 
 * @return The physical frame number of a victim frame.
 * 
 * HINTS: 
 *      - Use the global variables MEM_SIZE and PAGE_SIZE to calculate
 *      the number of entries in the frame table.
 *      - Use the global last_evicted to keep track of the pointer into the frame table
 * ----------------------------------------------------------------------------------
 */
pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped) {
            return i;
        }
    }

    // RANDOM implemented for you.
    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t unprotected_found = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                unprotected_found = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (unprotected_found < NUM_FRAMES) {
            return unprotected_found;
        }


    } else if (replacement == FIFO) {
        
        pfn_t victim = last_evicted + 1;
        while (frame_table[victim].protected && victim != last_evicted) {
            victim += 1;
            if (victim > num_entries - 1) {
                victim = 0;
            }
        }
        last_evicted = victim;
        return victim;

    } else if (replacement == CLOCKSWEEP) {
        pfn_t = last_evicted + 1; 
        for (pfn_t i = 0; i < 2 * NUM_FRAMES; i++) {
            if (!frame_table[ptr].protected && frame_table[ptr].referenced == 0) {
                pfn_t ret = ptr;
                last_evicted = (ptr) % NUM_FRAMES;
                return ret;
            }
            frame_table[ptr].referenced = 0;
            ptr = (ptr + 1) % NUM_FRAMES;
        }
    }

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}
