#include "global.h"
/**
 * @brief 
 * SYNTAX: INDEX ON column_name FROM relation_name USING indexing_strategy [BUCKET <bucketsize> | FANOUT <num_pointers>]
 * indexing_strategy: ASC | DESC | NOTHING
 */
bool syntacticParseINDEX()
{
    logger.log("syntacticParseINDEX");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[1] != "ON" || tokenizedQuery[3] != "FROM" || tokenizedQuery[5] != "USING")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INDEX;
    parsedQuery.indexColumnName = tokenizedQuery[2];
    parsedQuery.indexRelationName = tokenizedQuery[4];
    string indexingStrategy = tokenizedQuery[6];
    if (indexingStrategy == "BTREE")
        parsedQuery.indexingStrategy = BTREE;
    else if (indexingStrategy == "HASH")
        parsedQuery.indexingStrategy = HASH;
    else if (indexingStrategy == "NOTHING")
        parsedQuery.indexingStrategy = NOTHING;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.indexBSize = stoi(tokenizedQuery[8]);
    return true;
}

bool semanticParseINDEX()
{
    logger.log("semanticParseINDEX");
    if (!tableCatalogue.isTable(parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.indexColumnName, parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.indexRelationName);
    if(table->indexed){
        cout << "SEMANTIC ERROR: Table already indexed" << endl;
        return false;
    }
    return true;
}

void executeINDEX()
{
    logger.log("executeINDEX");
    Table table = *tableCatalogue.getTable(parsedQuery.indexRelationName);
    
    logger.log("sorting the table");
    table.sortTable(parsedQuery.indexColumnName, 0, 10);
    
    vector< pair<int, int> > pushvals;
    int colIndex = table.getColumnIndex(parsedQuery.indexColumnName);
    int curr = INT_MIN;
    for(int i = 0; i < table.blockCount; i++) {
        Page pg = bufferManager.getPage(table.tableName, i);
        vector<vector<int>> rows = pg.getRows();
        for(int j = 0; j < rows.size(); j++) {
            if(rows[j][colIndex] != curr) {
                curr = rows[j][colIndex];
                pushvals.push_back({curr, i});
            }
        }
    }
    if(parsedQuery.indexingStrategy == HASH) {
        table.indexed = true;
        table.indexedColumn = colIndex;
        table.indexingStrategy = HASH;
        table.hashtable = new HashTable(parsedQuery.indexBSize);
        for(int i = 0; i < pushvals.size(); i++) {
            // cout << pushvals[i].first << " " << pushvals[i].second << "\n";
            table.hashtable->insert(pushvals[i]);
            // cout << table.hashtable->getRecord(pushvals[i].first).second << "\n";
        }
    }
    if(parsedQuery.indexingStrategy == BTREE) {
        table.indexed = true;
        table.indexedColumn = colIndex;
        table.indexingStrategy = BTREE;
        table.bptree = new BPlusTree(parsedQuery.indexBSize);
        for(int i = 0; i < pushvals.size(); i++) {
            // cout << pushvals[i].first << " " << pushvals[i].second << "\n";
            table.bptree->insert(pushvals[i]);
            // cout << table.bptree->getRecord(pushvals[i].first).second << "\n";
        }
    }   
    return;
}