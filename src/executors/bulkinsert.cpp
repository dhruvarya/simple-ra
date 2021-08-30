#include "global.h"
/**
 * @brief 
 * SYNTAX: INDEX ON column_name FROM relation_name USING indexing_strategy [BUCKET <bucketsize> | FANOUT <num_pointers>]
 * indexing_strategy: ASC | DESC | NOTHING
 */
bool syntacticParseBULKINSERT()
{
    logger.log("syntacticParseBULKINSERT");
    if (tokenizedQuery.size() != 4 || tokenizedQuery[2] != "INTO")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = BULK_INSERT;
    parsedQuery.insertRelationName = tokenizedQuery[3];
    parsedQuery.loadRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseBULKINSERT()
{
    logger.log("semanticParseBULKINSERT");
    if (!tableCatalogue.isTable(parsedQuery.insertRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    Table *newTable = new Table(parsedQuery.loadRelationName);
    Table *table = tableCatalogue.getTable(parsedQuery.insertRelationName);
    if (newTable->load())
    {
        tableCatalogue.insertTable(newTable);
        // cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    } else {
        return false;
    }
    if(newTable->columnCount != table->columnCount) {
        cout << "SEMANTIC ERROR: Columns does not match\n";
        return false;
    }
    for(int i = 0; i < table->columnCount; i++) {
        if(table->columns[i] != newTable->columns[i]) {
            cout << "SEMANTIC ERROR: Columns does not match\n";
            return false;
        }
    }
    return true;
}

void executeBULKINSERT()
{
    Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);
    Table* intable = tableCatalogue.getTable(parsedQuery.loadRelationName);


    // table->writeRow(parsedQuery.insertValues);
    Page pg = bufferManager.getPage(table->tableName, table->blockCount - 1);
    
    vector<vector<int>> rows = pg.getRows();

    // cout << rows.size() << " " << table->maxRowsPerBlock << "\n";
    
    Cursor cursor = intable->getCursor();
    vector<int> inrows = cursor.getNext();
    int cnt = 0, tmp = 0;
    if(rows.size() == table->maxRowsPerBlock) {
        rows.clear();
        tmp = 1;
    }
    while(!inrows.empty()) {    
        rows.push_back(inrows);
        cnt++;
        // cout << rows.size() << "\n";
        inrows = cursor.getNext();
        if(rows.size() == table->maxRowsPerBlock) {
            if(tmp) table->blockCount++;
            table->rowsPerBlockCount.resize(table->blockCount);
            table->rowsPerBlockCount[table->blockCount - 1] = table->maxRowsPerBlock;
            table->rowCount += cnt;
            Page newPg(table->tableName, table->blockCount - 1);
            newPg.assignRows(rows);
            rows.clear();
            cnt = 0;
            tmp = 1;
        }
    } 
    if(rows.size() != table->maxRowsPerBlock) {
        table->blockCount++;
        table->rowsPerBlockCount.resize(table->blockCount);
        table->rowsPerBlockCount[table->blockCount - 1] = cnt;
        table->rowCount+=cnt;
        Page newPg(table->tableName, table->blockCount - 1);
        newPg.assignRows(rows);
        rows.clear();
    }
}