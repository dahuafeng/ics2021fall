//凤大骅 2000012965

#include <stdio.h>
#include <stdbool.h>
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct
{
    bool valid;  //有效位;
    int tag;     //标记位;
    int LRU_cnt; //LRU，需要替换时选值最小的一行;
} Cache_line;
typedef struct
{
    Cache_line *cache_line;
} Cache_set;
typedef struct
{
    int S; //组数;
    int E; //行数;
    int B; //块大小;
    Cache_set *cache_set;
} Cache;
//需要输出的结果及改变方法;
int hits = 0, misses = 0, evictions = 0;
//命中;
void Hit(bool verbose)
{
    if (verbose)
        printf("hit ");
    ++hits;
}
//不命中;
void Miss(bool verbose)
{
    if (verbose)
        printf("miss ");
    ++misses;
}
//替换;
void Eviction(bool verbose)
{
    if (verbose)
        printf("eviction ");
    ++evictions;
}
//初始化cache;
Cache _Init_(int s, int e, int b)
{
    Cache cache;
    cache.S = s;
    cache.E = e;
    cache.B = b;
    int SetNum = 1 << s;
    cache.cache_set = malloc(SetNum * sizeof(Cache_set));
    for (int i = 0; i < SetNum; ++i)
    {
        cache.cache_set[i].cache_line = malloc(SetNum * sizeof(Cache_line));
    }
    return cache;
}
//析构cache;
void _Destruct_(Cache cache)
{
    int SetNum = 1 << cache.S;
    for (int i = 0; i < SetNum; ++i)
        free(cache.cache_set[i].cache_line);
    free(cache.cache_set);
}
//-v指令打印操作地址和大小;
void Verbose(char op, unsigned long long address, int size)
{
    printf("%c %llx %d", op, address, size);
}
//-h指令打印帮助;
void Help()
{
    printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
    printf("-h: Optional help flag that prints usage info\n");
}
//L操作;
void LoadData(Cache_set TarSet, int Tag, int e, bool verbose)
{
    bool bHit = false;
    for (int i = 0; i < e; ++i)
    {
        //hit;
        if (TarSet.cache_line[i].valid && TarSet.cache_line[i].tag == Tag)
        {
            bHit = true;
            Hit(verbose);
            ++TarSet.cache_line[i].LRU_cnt;
            break;
        }
    }
    //miss;
    if (!bHit)
    {
        Miss(verbose);
        //evict;
        int MaxLRUCnt = TarSet.cache_line[0].LRU_cnt;
        int MinLRUCnt = TarSet.cache_line[0].LRU_cnt;
        int EvictLine = 0;
        for (int i = 0; i < e; ++i)
        {
            if (TarSet.cache_line[i].LRU_cnt > MaxLRUCnt)
                MaxLRUCnt = TarSet.cache_line[i].LRU_cnt;
            if (TarSet.cache_line[i].LRU_cnt < MinLRUCnt)
            {
                EvictLine = i;
                MinLRUCnt = TarSet.cache_line[i].LRU_cnt;
            }
        }
        TarSet.cache_line[EvictLine].LRU_cnt = MaxLRUCnt + 1;
        TarSet.cache_line[EvictLine].tag = Tag;
        if (TarSet.cache_line[EvictLine].valid)
            Eviction(verbose);
        else
            TarSet.cache_line[EvictLine].valid = 1;
    }
}
//S操作;
void StoreData(Cache_set TarSet, int Tag, int e, bool verbose)
{
    LoadData(TarSet, Tag, e, verbose);
}
//M操作;
void ModifyData(Cache_set TarSet, int Tag, int e, bool verbose)
{
    LoadData(TarSet, Tag, e, verbose);
    StoreData(TarSet, Tag, e, verbose);
}
//模拟cache运行;
void Running(FILE *fp, Cache cache, bool verbose)
{
    unsigned long long address;
    char op;
    int size;
    while (EOF != fscanf(fp, "%c %llx %d", &op, &address, &size))
    {
        if (op == 'I')
            continue;
        //进行索引;
        else
        {
            int SetIdx = (address >> cache.B) & ((1 << cache.S) - 1);
            int Tag = (address >> cache.B) >> cache.S;
            Cache_set TarSet = cache.cache_set[SetIdx];
            if (op == 'L')
            {
                if (verbose)
                    Verbose(op, address, size);
                LoadData(TarSet, Tag, cache.E, verbose);
            }
            else if (op == 'S')
            {
                if (verbose)
                    Verbose(op, address, size);
                StoreData(TarSet, Tag, cache.E, verbose);
            }
            else if (op == 'M')
            {
                if (verbose)
                    Verbose(op, address, size);
                ModifyData(TarSet, Tag, cache.E, verbose);
            }
        }
    }
}
//main函数;
int main(int argc, char *argv[])
{
    hits = misses = evictions = 0;
    Cache cache;
    cache.S = cache.E = cache.B = 0;
    cache.cache_set = NULL;
    int s = 0, e = 0, b = 0;
    bool verbose = 0;
    char op;
    FILE *TraceFile;
    while (-1 != (op = getopt(argc, argv, "hvs:E:b:t:")))
    {
        switch (op)
        {
        case 'h':
            Help();
            break;
        case 'v':
            verbose = true;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            e = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            TraceFile = fopen(optarg, "r");
            break;
        default:
            break;
        }
    }
    if (s && e && b && TraceFile)
    {
        cache = _Init_(s, e, b);
        Running(TraceFile, cache, verbose);
        printSummary(hits, misses, evictions);
        _Destruct_(cache);
    }
    return 0;
}