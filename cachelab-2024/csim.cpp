// student id: 2023202128
// please change the above line to your student id

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

struct CacheLine {
    bool valid;         // 有效位
    unsigned long tag;  // tag 位
    int lastUsed;       // LRU 值
};

class Cache {
   private:
    int S, s;                         // Set 数 S = 2 ^ s
    int E;                            // 每组行数
    int B, b;                         // 块大小 B = s ^ b
    vector<vector<CacheLine>> cache;  // 模拟的缓存
    int hits, misses, evictions;      // 命中次数、缺失次数、替换次数
    int timer;                        // 模拟时间，用于 LRU

   public:
    Cache(int s, int e, int b)
        : S(1 << s), E(e), B(1 << b), hits(0), misses(0), s(s), evictions(0), timer(0), b(b) {
        cache.resize(S, vector<CacheLine>(E, {false, 0, 0}));
    }

    int getHits() {
        return hits;
    }

    int getMisses() {
        return misses;
    }

    int getEvictions() {
        return evictions;
    }

    void access(unsigned long address) {
        timer++;                                                   // 增加时间计数器
        unsigned long blockOffset = address & ((1 << b) - 1);      // 块内偏移
        unsigned long setIndex = (address >> b) & ((1 << s) - 1);  // 所属 Set
        unsigned long tag = address >> (b + s);                    // tag 位
        auto& set = cache[setIndex];
        bool hit = false;

        // 命中
        for (auto& line : set) {
            if (line.valid && line.tag == tag) {
                hit = true;
                line.lastUsed = timer;  // 更新上次使用时间
                hits++;
                break;
            }
        }

        // 不命中
        if (!hit) {
            misses++;
            // 寻找代替换的行
            CacheLine* lruLine = nullptr;
            for (auto& line : set) {
                if (!line.valid) {
                    lruLine = &line;
                    break;
                }
                if (!lruLine || line.lastUsed < lruLine->lastUsed) {
                    lruLine = &line;
                }
            }
            if (lruLine->valid)
                evictions++;

            // 执行替换
            lruLine->valid = true;
            lruLine->tag = tag;
            lruLine->lastUsed = timer;
        }
    }
};

void printSummary(int hits, int misses, int evictions) {
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

void printHelp(const char* name) {
    printf(
        "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n"
        "Options:\n"
        "  -h         Print this help message.\n"
        "  -v         Optional verbose flag.\n"
        "  -s <num>   Number of set index bits.\n"
        "  -E <num>   Number of lines per set.\n"
        "  -b <num>   Number of block offset bits.\n"
        "  -t <file>  Trace file.\n\n"
        "Examples:\n"
        "  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n"
        "  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n",
        name, name, name);
}

int main(int argc, char* argv[]) {
    // printHelp(argv[0]);
    // printSummary(0, 0, 0);
    int s = 0, E = 0, b = 0;
    char* tracefile = NULL;
    if (argc < 9) {
        printHelp(argv[0]);
        return 1;
    } else {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                switch (argv[i][1]) {
                    case 's':
                        s = atoi(argv[++i]);
                        break;
                    case 'E':
                        E = atoi(argv[++i]);
                        break;
                    case 'b':
                        b = atoi(argv[++i]);
                        break;
                    case 't':
                        tracefile = argv[++i];
                        break;
                    case 'v':
                        // todo
                        break;
                    case 'h':
                        printHelp(argv[0]);
                        return 0;
                    default:
                        printHelp(argv[0]);
                        return 1;
                }
            }
        }
        if (s == 0 || E == 0 || b == 0 || tracefile == NULL) {
            printHelp(argv[0]);
            return 1;
        }
    }

    FILE* fp = fopen(tracefile, "r");
    int hits = 0, misses = 0, evictions = 0, size, regs;
    char operation;
    unsigned long address;
    unsigned int blockOffset, setIndex, tag;
    Cache cache(s, E, b);

    while (fscanf(fp, " %c %lx,%d %d", &operation, &address, &size, &regs) != EOF)
        cache.access(address);

    printSummary(cache.getHits(), cache.getMisses(), cache.getEvictions());
    return 0;
}
