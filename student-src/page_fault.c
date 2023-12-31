#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
    vpn_t vpn = vaddr_vpn(addr);
    pfn_t pfn = free_frame();

    pte_t* pt_base = (pte_t*)(mem + (PTBR * PAGE_SIZE));
    pte_t* pt = pt_base + vpn;

    pt->pfn = pfn;
    pt->valid = 1;
    pt->dirty = 0;

    fte_t* ft = frame_table + pfn;
    ft->mapped = 1;
    ft->process = current_process;
    ft->vpn = vpn;
    ft->referenced = 1;

    uint8_t* new_frame = mem + (pfn * PAGE_SIZE);
    if(swap_exists(pt)){
        swap_read(pt, new_frame);
    } else {
        memset(new_frame, 0, PAGE_SIZE);
    }
    stats.page_faults += 1;
}

#pragma GCC diagnostic pop
