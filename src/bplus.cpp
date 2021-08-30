#include "bplus.h"

Node::Node() 
{
    records.clear();
    childNodes.clear();
    foSize = 0;
}

BPlusTree::BPlusTree(int fan_out) 
{
    root = NULL;
    foSize = fan_out;
}

Node* BPlusTree::getParent(Node *cur_par, Node *node) 
{
    if(cur_par->isLeaf ||  cur_par->childNodes[0]->isLeaf) {
        return NULL;
    }
    for(int i = 0; i < cur_par->childNodes.size(); i++) {
        if(cur_par->childNodes[i] == node) {
            return cur_par;
        } else {
            Node *par = getParent(cur_par->childNodes[i], node);
            if(par) {
                return par;
            }
        }
    }
    return NULL;
}

void BPlusTree::splitNode(pair<int, int> record, Node *cur, Node *child)
{
    int br = -1; 
    for(int i = 0; i < cur->records.size(); i++) {
        if(record.first < cur->records[i].first) {
            br = i;
            break;
        }
        if(i == cur->records.size() - 1) {
            br = i+1;
        }
    }
    vector<pair<int, int>> tmprecords;
    vector<Node *> tmpchilds;
    for(int i = 0; i < cur->records.size(); i++) {
        if(i == br) {
            tmprecords.push_back(record);
        }
        tmprecords.push_back(cur->records[i]);
    }
    if(br == cur->records.size()) {
        tmprecords.push_back(record);
    }
    br++;
    for(int i = 0; i < cur->childNodes.size(); i++) {
        if(i == br) {
            tmpchilds.push_back(child);
        }
        tmpchilds.push_back(cur->childNodes[i]);
    }
    if(br == cur->childNodes.size()) {
        tmpchilds.push_back(child);
    }

    if(cur->records.size() < foSize) {
        cur->records = tmprecords;
        cur->childNodes = tmpchilds;
        return;
    }

    Node *newIntvl = new Node();
    newIntvl->isLeaf = false;

    int ltrecsize = (foSize + 1)/2;
    int rtrecsize = foSize - ltrecsize;

    for(int i = ltrecsize + 1; i < foSize + 1; i++) {
        newIntvl->records.push_back(tmprecords[i]);
    }
    for(int i = ltrecsize + 1; i < foSize + 2; i++) {
        newIntvl->childNodes.push_back(tmpchilds[i]);
    }

    vector<pair<int, int>> tmpCurRecords;
    vector<Node *> tmpCurNodes;

    for(int i = 0; i <= ltrecsize; i++) {
        tmpCurRecords.push_back(tmprecords[i]);
        tmpCurNodes.push_back(tmpchilds[i]);
    }
    tmpCurNodes.push_back(tmpchilds[ltrecsize+1]);

    cur->records = tmpCurRecords;
    cur->childNodes = tmpCurNodes;

    if(cur == root) {
        Node *newRoot = new Node();
        newRoot->records.push_back(newIntvl->records[0]);
        newRoot->childNodes.push_back(cur);
        newRoot->childNodes.push_back(newIntvl);
        newRoot->isLeaf = false;
        root = newRoot;
        return;
    }

    splitNode(newIntvl->records[0], getParent(root, cur), newIntvl);
}

void BPlusTree::insert(pair<int, int> record)
{
    if (root == NULL) { 
		root = new Node(); 
		root->records.push_back(record); 
		root->isLeaf = true;  
        return;
	} 
    Node *cur = root;
    Node *parent;

    while(cur->isLeaf == false) {
        parent = cur;
        for(int i = 0; i < cur->records.size(); i++) {
            if(record.first < cur->records[i].first) {
                cur = cur->childNodes[i];
                break;
            }
            if(i == cur->records.size() - 1) {
                cur = cur->childNodes[i+1];
                break;
            }
        }
    }

    int br = -1; 
    for(int i = 0; i < cur->records.size(); i++) {
        if(record.first < cur->records[i].first) {
            br = i;
            break;
        }
        if(i == cur->records.size() - 1) {
            br = i+1;
        }
    }
    vector<pair<int, int>> tmprecords;
    for(int i = 0; i < cur->records.size(); i++) {
        if(i == br) {
            tmprecords.push_back(record);
        }
        tmprecords.push_back(cur->records[i]);
    }
    if(br == cur->records.size()) {
        tmprecords.push_back(record);
    }
    br++;
    if(cur->records.size() < foSize) {
        cur->records = tmprecords;
        return;
    }

    Node *newIntvl = new Node();
    newIntvl->isLeaf = true;

    int ltrecsize = (foSize + 1)/2;
    int rtrecsize = foSize - ltrecsize;

    for(int i = ltrecsize + 1; i < foSize + 1; i++) {
        newIntvl->records.push_back(tmprecords[i]);
    }

    vector<pair<int, int>> tmpCurRecords;

    for(int i = 0; i <= ltrecsize; i++) {
        tmpCurRecords.push_back(tmprecords[i]);
    }
    cur->records = tmpCurRecords;

    if(cur == root) {
        Node *newRoot = new Node();
        newRoot->records.push_back(newIntvl->records[0]);
        newRoot->childNodes.push_back(cur);
        newRoot->childNodes.push_back(newIntvl);
        newRoot->isLeaf = false;
        root = newRoot;
        return;
    }
    splitNode(newIntvl->records[0], parent, newIntvl);
}

pair<int, int> BPlusTree::getRecord(int key) 
{
    if(root == NULL) {
        return {-1, -1};
    }
    Node *cur = root;
    while(cur->isLeaf == false) {
        for(int i = 0; i < cur->records.size(); i++) {
            if(key < cur->records[i].first) {
                cur = cur->childNodes[i];
                break;
            }
            if(i == cur->records.size() - 1) {
                cur = cur->childNodes[i+1];
                break;
            }
        }
    }

    for(int i = 0; i < cur->records.size(); i++) {
        if(cur->records[i].first == key) {
            return cur->records[i];
        }
    }
    return {-1, -1};
}

void BPlusTree::display(Node *cur)
{
    cout << "Node : ";
    for(int i = 0; i < cur->records.size(); i++) {
        cout << cur->records[i].first << "," << cur->records[i].second << " ";
    }
    cout << "\n";
    if(cur->isLeaf == false) {
        for(int i = 0; i < cur->childNodes.size(); i++) {
            display(cur->childNodes[i]);
        }
    }
}