#include "cursor.h"

class Block
{
    vector< pair<int, int> > records;
    Block *overflow;

    public:
    Block();

    int add(pair<int, int> record);

    void clearBlock(vector<pair<int, int>> &v);

    pair<int, int> getRecord(int key);

    void printBlock();
};

class HashTable
{
    int bitCount, numBuckets;
    vector<Block *> blocks;

    public:
    HashTable(int bucketCount);
    int hash(int x);
    void insert(pair<int, int> record);
    pair<int, int> getRecord(int key);
    void printTable();
};
