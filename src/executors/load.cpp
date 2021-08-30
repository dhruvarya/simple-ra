#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if (tokenizedQuery.size() == 2)
    {
        parsedQuery.queryType = LOAD;
        parsedQuery.loadRelationName = tokenizedQuery[1];
        return true;
    } 
    if (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") 
    {
        parsedQuery.queryType = LOAD_MATRIX;
        parsedQuery.loadRelationName = tokenizedQuery[2];
        return true;
    }
    cout << "SYNTAX ERROR" << endl;
    return false;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (parsedQuery.queryType == LOAD && tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }
    if (parsedQuery.queryType == LOAD_MATRIX && matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }
    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    if(parsedQuery.queryType == LOAD) 
    {
        Table *table = new Table(parsedQuery.loadRelationName);
        if (table->load())
        {
            tableCatalogue.insertTable(table);
            cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
        }
    } 
    else if(parsedQuery.queryType == LOAD_MATRIX)
    {
        Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
        if (matrix->load())
        {
            matrixCatalogue.insertMatrix(matrix);
            cout << "Loaded Matrix. Matrix Size: " << matrix->matrixSize << endl;
        }
    }
    return;
}