/*
 * mm.c
 * 2000012965 凤大骅
 * 
 * 显示空闲链表：每个空闲块有一个头部、一个脚部，还有一个前驱和一个后继，
 * 已分配块没有前驱和后继；
 * 头部和脚部存放这个块的大小和是否空闲，前驱和后继存放该空闲块在链表中的
 * 前驱和后继地址相对于堆起点地址的偏移量，这样头部、脚部、前驱、后继都占用
 * 4字节；已分配的块没有前驱和后继部分；
 * 
 * 去脚部优化：用每个块头部的第0位表示该块是否空闲，用第1位表示前面的块是否
 * 空闲，这样对于已分配的块就不再需要脚部了；
 * 
 * 分离适配：由于一个空闲块包括头部、脚部、前驱、后继，至少16字节
 * 故大小类设为{16-31}{32-63}{64-127}{128-255}{256-511}{512-1023}
 * {1024-2047}{2048-4095}{4096-无穷}；
 *
 * 空闲链表的组织选择按块大小顺序组织，寻找空闲块的策略是首次适配，
 * 释放已分配块后立即合并；
 * 
 * 
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */

#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT - 1)) & ~0x7)
/* Basic constants and macros */
#define WSIZE 4             /* Word and header/footer size (bytes) */
#define DSIZE 8             /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_PREV_ALLOC(p) (GET(p) & 0x2)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

/* Given block ptr p, 
 * set its header indicating the previous block allocated
  */
#define PREV_ALLOC(p) (GET(HDRP(p)) |= 0x2)
#define PREV_FREE(p) (GET(HDRP(p)) &= ~0x2)

/* Given block ptr bp, compute address of its pred and succ */
#define PRED(bp) ((char *)(bp) + WSIZE)
#define SUCC(bp) ((char *)(bp))

/* Given block ptr bp, 
 * computer logical address of its precursor and successor 
 */
#define PRED_BLKP(bp) (GET(PRED(bp)))
#define SUCC_BLKP(bp) (GET(SUCC(bp)))

#define MAXIDX 8        //一共分9个大小类,根节点下标最大为8;
#define MAXSIZESET 4096 //最后一个大小类是4096-无穷;

static char *heap_listp;
static char *heap_listp_head; //基准地址,prev和succ都保存的是相对此地址的偏移量;

/*
 * 给定一个指针bp, 计算其相对heap_listp_head的相对地址;
 */
static unsigned int get_logical_address(void *bp)
{
    return (unsigned int)((char *)bp - heap_listp_head);
}

/*
 * 给定一个指针bp, 计算其前驱的地址;
 */
static void *get_pred_blkp(void *bp)
{
    return (heap_listp_head + PRED_BLKP(bp));
}

/*
 * 给定一个指针bp, 计算其后继的地址;
 */
static void *get_succ_blkp(void *bp)
{
    return (heap_listp_head + SUCC_BLKP(bp));
}

/*
 * 计算size对应的大小类;
 */
static int get_size_index(unsigned int size)
{
    //errno size;
    if (size < 16)
    {
        fprintf(stderr, "size errno\n");
        return -1;
    }

    if (size >= MAXSIZESET)
        return MAXIDX;

    int cnt = 0;
    size >>= 5;

    while (size)
    {
        ++cnt;
        size >>= 1;
    }
    return cnt;
}

/*
 * 在根节点为root的链表中插入一个空闲块bp,采用按块大小顺序的方法插入;
 */
static void *insert_free_block(void *bp)
{
    unsigned int bp_size = GET_SIZE(HDRP(bp));
    int bp_size_idx = get_size_index(bp_size);

    void *root = heap_listp_head + bp_size_idx * WSIZE;
    void *ptr = root;
    //寻找插入位置;
    while (SUCC_BLKP(ptr))
    {
        ptr = get_succ_blkp(ptr);
        if (GET_SIZE(HDRP(ptr)) >= bp_size)
            break;
    }

    if (SUCC_BLKP(ptr))
    {
        PUT(SUCC(get_pred_blkp(ptr)), get_logical_address(bp));
        PUT(PRED(bp), PRED_BLKP(ptr));
        PUT(SUCC(bp), get_logical_address(ptr));
        PUT(PRED(ptr), get_logical_address(bp));
    }
    else
    {
        PUT(SUCC(ptr), get_logical_address(bp));
        PUT(PRED(bp), get_logical_address(ptr));
        PUT(SUCC(bp), 0);
    }
    return bp;
}
/*
 * 从空闲链表中删除bp;
 */
static void del_free_block(void *bp)
{

    PUT(SUCC(get_pred_blkp(bp)), SUCC_BLKP(bp));

    if (SUCC_BLKP(bp) != 0)
        PUT(PRED(get_succ_blkp(bp)), PRED_BLKP(bp));
}
/*
 * 给定空闲块bp,合并bp前后的空闲块;
 */
static void *coalesce(void *bp)
{

    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));

    unsigned int size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    { /* Case 1 */
        return bp;
    }

    else if (prev_alloc && !next_alloc)
    { /* Case 2 */

        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));

        del_free_block(NEXT_BLKP(bp));

        PUT(HDRP(bp), PACK(size, GET_PREV_ALLOC(HDRP(bp))));
        PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc)
    { /* Case 3 */

        size += GET_SIZE(HDRP(PREV_BLKP(bp)));

        del_free_block(PREV_BLKP(bp));

        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)),
            PACK(size, GET_PREV_ALLOC(HDRP(PREV_BLKP(bp)))));
        bp = PREV_BLKP(bp);
    }

    else
    { /* Case 4 */

        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));

        del_free_block(PREV_BLKP(bp));
        del_free_block(NEXT_BLKP(bp));

        PUT(HDRP(PREV_BLKP(bp)),
            PACK(size, GET_PREV_ALLOC(HDRP(PREV_BLKP(bp)))));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    PREV_FREE(NEXT_BLKP(bp));
    return bp;
}
/*
 * 扩大堆空间;
 */
static void *extend_heap(size_t words)
{

    char *bp;
    unsigned int size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((bp = mem_sbrk(size)) == (void *)-1)
        return NULL;

    PUT(HDRP(bp), PACK(size, GET_PREV_ALLOC(HDRP(bp))));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    PUT(PRED(bp), 0);
    PUT(SUCC(bp), 0);

    bp = coalesce(bp);

    bp = insert_free_block(bp);
    return bp;
}

/* 
 *  用空闲链表中的块bp从头部进行内存分配;如果剩余的块满足最小块要求则留
 *  空闲链表中，否则全部分配;
 */
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    del_free_block(bp);

    if ((csize - asize) >= (2 * DSIZE))
    {

        PUT(HDRP(bp), PACK(asize, (GET_PREV_ALLOC(HDRP(bp)) | 1)));

        bp = NEXT_BLKP(bp);

        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));

        PREV_ALLOC(bp);
        PREV_FREE(NEXT_BLKP(bp));

        bp = insert_free_block(bp);
    }
    else
    {
        PUT(HDRP(bp), PACK(csize, (GET_PREV_ALLOC(HDRP(bp)) | 1)));
        PREV_ALLOC(NEXT_BLKP(bp));
    }
}

/*
 * 寻找可以分配的空闲块,采用首次适配;
 */
static void *find_fit(size_t asize)
{
    /* First-fit search */
    void *bp;

    for (int idx = get_size_index(asize); idx <= MAXIDX; ++idx)
    {

        bp = heap_listp_head + idx * WSIZE;
        while (SUCC_BLKP(bp) && (bp = get_succ_blkp(bp)))
        {
            if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
            {

                return bp;
            }
        }
    }

    return NULL; /* No fit */
}

/*
 * 初始化堆,设置好空闲链表的根节点;
 */

int mm_init(void)
{
    if ((heap_listp = mem_sbrk(12 * WSIZE)) == (void *)-1)
    {
        fprintf(stderr, "mm_init errno\n");
        return -1;
    }
    PUT(heap_listp, 0);                             //{16-31};
    PUT(heap_listp + (1 * WSIZE), 0);               //{32-63};
    PUT(heap_listp + (2 * WSIZE), 0);               //{64-127};
    PUT(heap_listp + (3 * WSIZE), 0);               //{128-255};
    PUT(heap_listp + (4 * WSIZE), 0);               //{256-511};
    PUT(heap_listp + (5 * WSIZE), 0);               //{512-1023};
    PUT(heap_listp + (6 * WSIZE), 0);               //{1024-2047};
    PUT(heap_listp + (7 * WSIZE), 0);               //{2048-4095};
    PUT(heap_listp + (8 * WSIZE), 0);               //{4096-无穷};
    PUT(heap_listp + (9 * WSIZE), PACK(DSIZE, 1));  //Prologue header;
    PUT(heap_listp + (10 * WSIZE), PACK(DSIZE, 1)); //Prologue footer;
    PUT(heap_listp + (11 * WSIZE), PACK(0, 1));     //Epilogue header;

    heap_listp_head = heap_listp; //设置基准地址;
    heap_listp += 10 * WSIZE;

    PREV_ALLOC(NEXT_BLKP(heap_listp));

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
    {
        fprintf(stderr, "extend_heap errno\n");
        return -1;
    }
    return 0;
}

/*
 * 分配一个块,最小块16字节,已分配的块不需要footer;
 */
void *malloc(size_t size)
{
    unsigned int asize;      /* Adjusted block size */
    unsigned int extendsize; /* Amount to extend heap if no fit */

    char *bp;

    if (heap_listp == 0)
    {
        mm_init();
    }
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
    {
        if ((size + WSIZE) % DSIZE == 0)
            asize = size + WSIZE;
        else
            asize = DSIZE * ((size + WSIZE) / DSIZE + 1);
    }

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;

    place(bp, asize);
    return bp;
}

/*
 * 释放一个已分配的块,释放后立即合并;
 */
void free(void *ptr)
{
    if (ptr == 0)
        return;

    unsigned int size = GET_SIZE(HDRP(ptr));

    if (heap_listp == 0)
    {
        mm_init();
    }

    PUT(HDRP(ptr), PACK(size, GET_PREV_ALLOC(HDRP(ptr))));
    PUT(FTRP(ptr), PACK(size, 0));
    PREV_FREE(NEXT_BLKP(ptr));

    ptr = coalesce(ptr);
    ptr = insert_free_block(ptr);
}

/*
 * 分配一个新的块，将原来的块中数据复制到新块并释放原块; 
 */
void *realloc(void *oldptr, size_t size)
{
    unsigned int oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0)
    {
        free(oldptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (oldptr == NULL)
    {
        return malloc(size);
    }

    newptr = malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if (!newptr)
    {
        return 0;
    }

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(oldptr));
    if (size < oldsize)
        oldsize = size;
    memcpy(newptr, oldptr, oldsize);

    /* Free the old block. */
    free(oldptr);

    return newptr;
}

/*
 * 分配一个块并将其数据初始化为0;
 */
void *calloc(size_t nmemb, size_t size)
{
    void *bp = malloc(nmemb * size);
    memset(bp, 0, nmemb * size);
    return bp;
}

/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void *p)
{
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static int aligned(const void *p)
{
    return (size_t)ALIGN(p) == (size_t)p;
}

/*
 * 检查堆中块是否有对齐错误、空闲块的头部和脚部是否一致、
 * 是否有未合并的块、空闲链表大小和空闲块数量是否一致、
 * 序言块和结尾块是否正确、空闲链表中空闲块是否在堆中、
 * prev_alloc位是否正确、空闲链表中是否有已分配的块、
 * 空闲链表中块大小是否在其正确的大小类中;
 */
void mm_checkheap(int lineno)
{
    void *bp = heap_listp;
    int free_num = 0;
    unsigned int prev_alloc = 1;
    //检查序言块;
    if (GET_SIZE(HDRP(bp)) != DSIZE || !GET_ALLOC(HDRP(bp)))
    {
        printf("prologue error\n");
        exit(1);
    }
    if (!aligned(bp))
    {
        printf("prologue is not aligned\n");
        exit(1);
    }

    bp = NEXT_BLKP(bp);
    for (; in_heap(bp); bp = NEXT_BLKP(bp))
    {
        if (!aligned(bp))
        {
            printf("block %p is not aligned\n", bp);
            exit(1);
        }

        if ((GET_PREV_ALLOC(HDRP(bp)) >> 1) != prev_alloc)
        {
            printf("block %p prev_alloc bit error,%u\n", bp, GET_PREV_ALLOC(HDRP(bp)));
            exit(1);
        }

        if (!GET_ALLOC(HDRP(bp)))
        {
            if (!prev_alloc)
            {
                printf("block %p is not coalesced\n", bp);
                exit(1);
            }

            prev_alloc = 0;
            ++free_num;

            unsigned int bp_header = GET(HDRP(bp)) & ~0x2;
            unsigned int bp_footer = GET(FTRP(bp));
            //已分配块没有脚部,只需检查空闲块;
            if (bp_header != bp_footer)
            {
                printf("block %p is not the header and footer identical\n", bp);
                exit(1);
            }

            unsigned int pred_succ = SUCC_BLKP(get_pred_blkp(bp));
            if (pred_succ != get_logical_address(bp))
            {
                printf("the pred_succ of block %p is not itself\n", bp);
                exit(1);
            }
        }
        else
            prev_alloc = 1;
    }
    //检查结尾块;
    if (!aligned(bp))
    {
        printf("epilogue is not aligned\n");
        exit(1);
    }
    if (GET_SIZE(HDRP(bp)) != 0 || !GET_ALLOC(HDRP(bp)))
    {
        printf("epilogue error\n");
        exit(1);
    }

    //检查空闲链表;
    void *p, *root;
    for (int i = 0; i <= MAXIDX; ++i)
    {
        root = heap_listp_head + i * WSIZE;
        p = root;
        while (SUCC_BLKP(p))
        {
            --free_num;
            p = get_succ_blkp(p);

            if (!in_heap(p))
            {
                printf("free block %p is not in heap\n", p);
                exit(1);
            }

            if (GET_ALLOC(HDRP(p)))
            {
                printf("free block %p is allocated\n", p);
                exit(1);
            }

            unsigned int size = GET_SIZE(HDRP(p));
            if (get_size_index(size) != i)
            {
                printf("free block %p should in %d,but actually in %d\n",
                       p, get_size_index(size), i);
                exit(1);
            }
        }
    }
    if (free_num != 0)
    {
        printf("free blocks does not equal to the size of free list\n");
        exit(1);
    }
}
