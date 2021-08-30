#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    if(tokenizedQuery.size()!= 10 || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN" || tokenizedQuery[8] != "BUFFER"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortColumnName = tokenizedQuery[5];
    parsedQuery.sortRelationName = tokenizedQuery[3];
    string sortingStrateg = tokenizedQuery[7];
    if(sortingStrateg == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if(sortingStrateg == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else{
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.sortBufferSize = stoi(tokenizedQuery[9]);
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}


void executeSORT(){
    logger.log("executeSORT");
    Table table = *tableCatalogue.getTable(parsedQuery.sortRelationName);

    Table* resultantTable = new Table(parsedQuery.sortResultRelationName, table.columns);

    Cursor cursor = table.getCursor();
    vector<int> row = cursor.getNext();

    while (!row.empty())
    {
        resultantTable->writeRow<int>(row);
        row = cursor.getNext();
    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    
    if(!resultantTable->sortTable(parsedQuery.sortColumnName, parsedQuery.sortingStrategy, parsedQuery.sortBufferSize)) {
        logger.log("Falal Error in sort");
    }

    return;
}