#include "global.h"

/**
 * @brief 
 * SYNTAX: ALTER TABLE <table_name> ADD|DELETE COLUMN <colname>
 */

bool syntacticParseALTER()
{
    logger.log("syntacticParseALTER");
    if (tokenizedQuery.size() != 6 || tokenizedQuery[1] != "TABLE" || (tokenizedQuery[3] != "DELETE" && tokenizedQuery[3] != "ADD") || tokenizedQuery[4] != "COLUMN" )
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ALTER;
    parsedQuery.alterRelationName = tokenizedQuery[2];
    parsedQuery.alterOperation = tokenizedQuery[3];
    parsedQuery.alterColumnName = tokenizedQuery[5];
    return true;
}

bool semanticParseALTER()
{
    logger.log("semanticParseALTER");

    if (!tableCatalogue.isTable(parsedQuery.alterRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    Table table = *tableCatalogue.getTable(parsedQuery.alterRelationName);
    
    if (parsedQuery.alterOperation == "ADD")
    {
        if (table.isColumn(parsedQuery.alterColumnName))
        {
            cout << "SEMANTIC ERROR: Column already exist in relation";
            return false;
        }
    }
    
    if (parsedQuery.alterOperation == "DELETE")
    {
        if (!table.isColumn(parsedQuery.alterColumnName))
        {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation";
            return false;
        }
    }
    
    return true;
}

void executeALTER()
{
    logger.log("executeALTER");
    Table *table = tableCatalogue.getTable(parsedQuery.alterRelationName);
    vector<string> columns;
    string op = parsedQuery.alterOperation;
    if(op == "ADD") {
        columns = table->columns;
        columns.push_back(parsedQuery.alterColumnName);
    } else if(op == "DELETE") {
        for(int i = 0; i < table->columnCount; i++) {
            if(table->columns[i] != parsedQuery.alterColumnName) {
                columns.push_back(table->columns[i]);
            }
        }
    }
    Table *resTable = new Table(parsedQuery.alterRelationName + "_tmp", columns);

    Cursor cursor = table->getCursor();
    vector<int> row = cursor.getNext();

    while(!row.empty()) {
        vector<int> tmprow;
        if(op == "ADD") {
            for(int i = 0; i < row.size(); i++) {
                tmprow.push_back(row[i]);
            }
            tmprow.push_back(0);
        } else if(op == "DELETE") {
            int delind = table->getColumnIndex(parsedQuery.alterColumnName);
            for(int i = 0; i < row.size(); i++) {
                if(i != delind) {
                    tmprow.push_back(row[i]);
                }
            }
        }
        resTable->writeRow(tmprow);
        row = cursor.getNext();
    }
    string filePath = table->sourceFileName;

    for(int i = 0; i < table->blockCount; i++) {
        bufferManager.deleteFile(table->tableName, i);
    }

    resTable->blockify();
    // cout << resTable->columnCount << " " << table->columnCount << "\n";
    table->columnCount = resTable->columnCount;
    // cout << table->columnCount << "\n";
    table->columns = resTable->columns;
    table->blockCount = resTable->blockCount;
    table->distinctValuesPerColumnCount = resTable->distinctValuesPerColumnCount;
    table->maxRowsPerBlock = resTable->maxRowsPerBlock;
    table->rowsPerBlockCount.resize(resTable->rowsPerBlockCount.size());
    for(int i = 0; i < resTable->blockCount; i++) {
        table->rowsPerBlockCount[i] = resTable->rowsPerBlockCount[i];
        string oldName = "../data/temp/" + resTable->tableName + "_Page" + to_string(i);
        string newName = "../data/temp/" + table->tableName + "_Page" + to_string(i);
        if(rename(oldName.c_str(), newName.c_str()) != 0) {
            cout << "Fatal Error\n";
        }
        bufferManager.deleteFromPool(newName);
    }
    resTable->unload();
}