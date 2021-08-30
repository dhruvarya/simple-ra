#include "hash.h"

Block::Block() {
    overflow = NULL;
    records.clear();
}

int Block::add(pair<int, int> record) {
    if(records.size() < 20) {
        records.push_back(record);
        return 0;
    }
    int ret = 0;
    if(overflow == NULL) {
        overflow = new Block();
        ret = 1;
    }
    ret |= overflow->add(record);
    return ret;
}

void Block::clearBlock(vector<pair<int, int>> &v) {
    for(auto record : records) {
        v.push_back(record);
    }
    records.clear();
    if(overflow) {
        overflow->clearBlock(v);
        delete overflow;
        overflow = NULL;
    }
}

pair<int, int> Block::getRecord(int key) {
    for(auto record : records) {
        if(record.first == key) {
            return record;
        }
    }
    if(overflow) {
        return overflow->getRecord(key);
    }
    return {-1, -1};
}

void Block::printBlock() {
    for(auto record : records) {
        cout << record.first << ":" << record.second << " ";
    }
    if(overflow) {
        cout << "-> ";
        overflow->printBlock();
    }
}

HashTable::HashTable(int bucketCount) 
{
    numBuckets = bucketCount;
    bitCount = 0;
    for(int i = 0; i < numBuckets; i++) {
        blocks.push_back(new Block());
    }
}

int HashTable::hash(int x)
{
    int mod = numBuckets * (1 << bitCount);
    return (x + mod)%mod;
}

void HashTable::insert(pair<int, int> record)
{
    int k = hash(record.first);
    if(k >= blocks.size()) {
        k -= (numBuckets * (1 << (bitCount - 1)));
    }
    int ret = blocks[k]->add(record);

    if(ret == 1) {
        blocks.push_back(new Block());
        bitCount = ceil(log2((double) blocks.size()/numBuckets));
        // cout << bitCount << "\n";
        k = (blocks.size() - 1) - (numBuckets * (1 << (bitCount - 1)));

        // cout << "splitting level : " << bitCount << " and block " << k << "\n";


        vector<pair<int, int>> v;
        blocks[k]->clearBlock(v);

        for(int i = 0; i < v.size(); i++) {
            blocks[hash(v[i].first)]->add(v[i]);
        } 
    }
}

void HashTable::printTable()
{
    cout << "HashTable block size = " << blocks.size() << "\n";
    for(int i = 0; i < (int)blocks.size(); i++) {
        cout << "Block " << i << ": ";
        blocks[i]->printBlock();
        cout << "\n";
    }  
}

pair<int, int> HashTable::getRecord(int key)
{
    int k = hash(key);
    if(k >= blocks.size()) {
        k -= (numBuckets * (1 << (bitCount - 1)));
    }
    return blocks[k]->getRecord(key);
}