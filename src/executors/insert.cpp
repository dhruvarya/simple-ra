#include "global.h"
/**
 * @brief 
 * SYNTAX: INDEX ON column_name FROM relation_name USING indexing_strategy [BUCKET <bucketsize> | FANOUT <num_pointers>]
 * indexing_strategy: ASC | DESC | NOTHING
 */
bool syntacticParseINSERT()
{
    logger.log("syntacticParseINSERT");
    if (tokenizedQuery[1] != "INTO" || tokenizedQuery[3] != "VALUES")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INSERT;
    parsedQuery.insertRelationName = tokenizedQuery[2];
    
    for(int i = 4; i < tokenizedQuery.size(); i++) {
        regex numeric("[-]?[0-9]+");
        string secondArgument = tokenizedQuery[i];
        if (!regex_match(secondArgument, numeric))
        {
            return false;
        }
        parsedQuery.insertValues.push_back(stoi(tokenizedQuery[i]));
    }
    return true;
}

bool semanticParseINSERT()
{
    logger.log("semanticParseINSERT");
    if (!tableCatalogue.isTable(parsedQuery.insertRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);
    if(tokenizedQuery.size() - 4 != table->columnCount) {
        cout << "SEMANTIC ERROR: Relation does not have enough columns" << endl;
        return false;
    }
    return true;
}

void executeINSERT()
{
    Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);
    // table->writeRow(parsedQuery.insertValues);
    Page pg = bufferManager.getPage(table->tableName, table->blockCount - 1);
    vector<vector<int>> rows = pg.getRows();

    cout << rows.size() << " " << table->maxRowsPerBlock << "\n";
    if(rows.size() < table->maxRowsPerBlock) {
        rows.push_back(parsedQuery.insertValues);
        pg.assignRows(rows);
        table->rowsPerBlockCount[table->blockCount-1]++;
        table->rowCount++;
        bufferManager.deleteFromPool(pg.pageName);
    } else {
        table->blockCount++;
        table->rowsPerBlockCount.resize(table->blockCount);
        table->rowsPerBlockCount[table->blockCount - 1] = 1;
        table->rowCount++;
        Page newPg(table->tableName, table->blockCount - 1);
        vector<vector<int>> tmprows;
        tmprows.push_back(parsedQuery.insertValues);
        newPg.assignRows(tmprows);
    }
}