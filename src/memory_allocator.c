#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WARD_SIZE 20
#define PAGE_SIZE 2    // NAYA: Har page 2 units ka hoga
#define NUM_PAGES (WARD_SIZE / PAGE_SIZE) // Total 10 pages banenge
#define MAX_BLOCKS 50

typedef struct {
    int pid;    
    int start;
    int size;   // Ab yeh size hamesha Page Size ka multiple hoga
    int actual_needed; // NAYA: Patient ko asal mein kitne chahiye the
} Block;

Block memory[MAX_BLOCKS];
int page_table[NUM_PAGES]; // NAYA: Page Table Array
int num_blocks = 1;
char strategy[10] = "best"; 

void init_memory() {
    memory[0].pid = -1;
    memory[0].start = 0;
    memory[0].size = WARD_SIZE;
    memory[0].actual_needed = 0;
    num_blocks = 1;
    
    // Page table ko khali (-1) karein
    for(int i = 0; i < NUM_PAGES; i++) {
        page_table[i] = -1;
    }
    
    FILE *f = fopen("memory_log.txt", "w");
    if (f) fclose(f);
}

void print_ward_map(const char* title) {
    printf("\n=== Ward Map: %s ===\n", title);
    for (int i = 0; i < num_blocks; i++) {
        if (memory[i].pid == -1)
            printf("[ FREE : %d units ] ", memory[i].size);
        else
            printf("[ PID %d : %d units ] ", memory[i].pid, memory[i].size);
    }
    printf("\n==================================\n");
}

// NAYA FUNCTION: Page Table ko print karna
void print_page_table() {
    printf("\n--- Page Table (Page Size = %d units) ---\n", PAGE_SIZE);
    for(int i = 0; i < NUM_PAGES; i++) {
        if(page_table[i] == -1) {
            printf("Page %d : [ FREE ]\n", i);
        } else {
            printf("Page %d : [ PID %d ]\n", i, page_table[i]);
        }
    }
    printf("-----------------------------------------\n");
}

void report_fragmentation() {
    int total_free = 0, largest_free = 0;
    for (int i = 0; i < num_blocks; i++) {
        if (memory[i].pid == -1) {
            total_free += memory[i].size;
            if (memory[i].size > largest_free) largest_free = memory[i].size;
        }
    }

    float external_frag = 0.0;
    if (total_free > 0) external_frag = (1.0 - ((float)largest_free / total_free)) * 100.0;

    FILE *f = fopen("memory_log.txt", "a");
    if (f) {
        time_t t = time(NULL);
        char *time_str = ctime(&t);
        time_str[strlen(time_str)-1] = '\0'; 
        fprintf(f, "[%s] Free: %d, Largest: %d, ExtFrag: %.2f%%\n", time_str, total_free, largest_free, external_frag);
        fclose(f);
    }
}

void coalesce() {
    int coalesced = 0, i = 0;
    while (i < num_blocks - 1) {
        if (memory[i].pid == -1 && memory[i+1].pid == -1) {
            memory[i].size += memory[i+1].size;
            for (int j = i + 1; j < num_blocks - 1; j++) { memory[j] = memory[j+1]; }
            num_blocks--;
            coalesced = 1;
        } else {
            i++;
        }
    }
    if (coalesced) print_ward_map("After Coalescing");
}

void allocate_memory(int pid, int size) {
    // PAGING LOGIC: Patient ke required size ko Pages mein convert karein
    int pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE; // Ceiling division
    int allocated_size = pages_needed * PAGE_SIZE;         // e.g., 3 becomes 4
    int internal_frag = allocated_size - size;             // e.g., 4 - 3 = 1 wasted unit

    int target_idx = -1, target_size = -1;

    // Allocate karte waqt 'allocated_size' (rounded up) use karein
    for (int i = 0; i < num_blocks; i++) {
        if (memory[i].pid == -1 && memory[i].size >= allocated_size) {
            if (strcmp(strategy, "first") == 0) { target_idx = i; break; } 
            else if (strcmp(strategy, "best") == 0) {
                if (target_idx == -1 || memory[i].size < target_size) { target_idx = i; target_size = memory[i].size; }
            } else if (strcmp(strategy, "worst") == 0) {
                if (target_idx == -1 || memory[i].size > target_size) { target_idx = i; target_size = memory[i].size; }
            }
        }
    }

    if (target_idx != -1) {
        if (memory[target_idx].size > allocated_size) {
            for (int i = num_blocks; i > target_idx; i--) { memory[i] = memory[i-1]; }
            num_blocks++;
            memory[target_idx].pid = pid;
            memory[target_idx].size = allocated_size;
            memory[target_idx].actual_needed = size;
            
            memory[target_idx+1].pid = -1;
            memory[target_idx+1].start = memory[target_idx].start + allocated_size;
            memory[target_idx+1].size -= allocated_size;
        } else {
            memory[target_idx].pid = pid;
            memory[target_idx].actual_needed = size;
        }

        // UPDATE PAGE TABLE
        int start_page = memory[target_idx].start / PAGE_SIZE;
        for (int p = 0; p < pages_needed; p++) {
            page_table[start_page + p] = pid;
        }

        printf("\n[ALLOCATED] Patient %d asked for %d units.\n", pid, size);
        printf(" -> Paging: Assigned %d pages (%d units). Internal Fragmentation: %d units.\n", 
                pages_needed, allocated_size, internal_frag);
        
        print_ward_map("After Allocation");
        report_fragmentation();
    } else {
        printf("\n[FAILED] Cannot allocate %d units for Patient %d.\n", size, pid);
    }
}

void deallocate_memory(int pid) {
    for (int i = 0; i < num_blocks; i++) {
        if (memory[i].pid == pid) {
            memory[i].pid = -1;
            
            // CLEAR FROM PAGE TABLE
            int start_page = memory[i].start / PAGE_SIZE;
            int pages = memory[i].size / PAGE_SIZE;
            for(int p = 0; p < pages; p++) {
                page_table[start_page + p] = -1;
            }

            printf("\n[DISCHARGED] Patient %d freed %d units.\n", pid, memory[i].size);
            
            print_ward_map("Before Coalescing");
            coalesce();
            report_fragmentation();
            return;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "--strategy") == 0) { strcpy(strategy, argv[2]); }
    
    init_memory();
    
    // TEST CASE FOR PAGING (Odd numbers triggers Internal Fragmentation)
    allocate_memory(101, 3); // Needs 3, gets 4 (1 unit wasted)
    print_page_table();

    allocate_memory(102, 5); // Needs 5, gets 6 (1 unit wasted)
    print_page_table();

    deallocate_memory(101); 
    print_page_table();

    return 0;
}
