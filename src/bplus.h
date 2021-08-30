#include "hash.h"

class Node
{
    public:
    vector< pair<int, int> > records;
    vector<Node *> childNodes;
    bool isLeaf;
    int foSize;
    Node();
};

class BPlusTree
{
    public:
    int foSize;
    Node *root;
    BPlusTree(int fan_out);
    Node *getParent(Node *cur_par, Node *node);
    void insert(pair<int, int> record);
    pair<int, int> getRecord(int key);
    void splitNode(pair<int, int> record, Node *cur, Node *child);
    void display(Node *cur);
};