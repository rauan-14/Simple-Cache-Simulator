#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define BYTE_BITS 3

enum cache_block_state {
    INVALID,
    VALID
};

typedef unsigned uint;

typedef struct cache_block{
    enum cache_block_state status;
    uint tag;
    uint time;
}cache_block;

uint sets;
uint sets_log2 = 0;
uint assoc;
uint block;
uint block_log2 = 0;
uint total_cache_accesses = 0;
uint total_cache_misses = 0;
uint sim_time = 0;
struct cache_block *cache;

uint create_mask (int n) {
    int mask = 0;
    int i;
    for (i = 0; i < n; ++i)
        mask |= 1 << i;
    return (uint)mask;
}

uint set_num(uint addr) {
    uint block_offset_bits = block_log2;
    uint set_mask = create_mask(sets_log2);
    uint set_num = (addr >> block_offset_bits);
    set_num = set_num & set_mask;
    return set_num;
}

uint tag_num(uint addr) {
    uint line_bits = block_log2  + sets_log2;
    uint tag = addr >> line_bits;
    return tag;
}

void cache_access(char inst_type, uint addr){
    int set, tag;
    set = set_num(addr);
    tag = tag_num(addr);
    //printf("%x %x %x\n", addr, set, tag);

    int i;
    int invalid_index = -1;
    int evict_index = 0;
    for (i = 0; i < assoc; i++) {
        if((cache + set * assoc + i)->status == VALID && (cache + set * assoc + i)->tag == tag) {
            //if we hit
            (cache + set * assoc + i)->time = sim_time;
            return;
        }
        //otherwise we miss
        if((cache + set*assoc + i)->status == INVALID && invalid_index == -1)
            invalid_index = i;
        else if ((cache + set*assoc + evict_index)->time >= (cache + set*assoc + i)->time) {
            evict_index = i;
        }
    }
    total_cache_misses++;
    if(invalid_index != -1) { //if there is free line in set
        (cache + set*assoc + invalid_index)->status = VALID;
        (cache + set*assoc + invalid_index)->tag = tag;
        (cache + set*assoc + invalid_index)->time = sim_time+1;
    }
    else { //if we need to evict
        (cache + set*assoc + evict_index)->status = VALID;
        (cache + set*assoc + evict_index)->tag = tag;
        (cache + set*assoc + evict_index)->time = sim_time+1;
    }
    
}

int main(int argc, char** argv)
{
    /*
    if (argc != 5) {
        printf("Usage: %s <number_of_sets> <associativity> <cache_block_size> <trace_file>\n", argv[0]);
        return -1;
    }
    */

    /*
     input parameters
     argv[1] = number of sets
     argv[2] = associativity
     argv[3] = cache block size
     argv[4] = trace file name
     */

    int i, j;

    char inst_type = 0;
    uint addr = 0;
    sets = atoi(argv[1]);
    assoc = atoi(argv[2]);
    block = atoi(argv[3]);
    while (block >>= 1) ++block_log2;
    uint temp = sets;
    while (temp >>= 1) ++sets_log2;

    FILE *fp = fopen(argv[4], "r");
    if (fp == NULL){
        printf("trace file does not exist\n");
        return -1;
    }

    cache = (struct cache_block *)malloc(sizeof(struct cache_block)*sets*assoc);

    for(i = 0; i < sets; i++) {
        for(j = 0; j < assoc; j++) {
            (cache + i*assoc + j)->tag = 0;
            (cache + i*assoc + j)->status = INVALID;
            (cache + i*assoc + j)->time = 0;
        }
    }


    while (fscanf(fp, "%c 0x%x\n", &inst_type, &addr) != EOF) {
        cache_access(inst_type, addr);
        total_cache_accesses++;
        sim_time++;
    }

    printf("Cache accesses = %u\n", total_cache_accesses);
    printf("Cache misses = %u\n", total_cache_misses);

    free(cache);
    fclose(fp);

    return 0;
}
