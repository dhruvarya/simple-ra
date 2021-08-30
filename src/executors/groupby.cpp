#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- GROUP BY <attr> FROM relation_name RETURN MIN|MAX|SUM|AVG(attr)
 */
bool syntacticParseGROUP()
{
    logger.log("syntacticParseGROUP");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[3] != "BY" || tokenizedQuery[7] != "RETURN" || tokenizedQuery[5] != "FROM")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = GROUP;
    parsedQuery.groupResultRelationName = tokenizedQuery[0];
    parsedQuery.groupRelationName = tokenizedQuery[6];
    parsedQuery.groupColumnName = tokenizedQuery[4];
    parsedQuery.groupOperation = tokenizedQuery[8].substr(0, 3);
    if (parsedQuery.groupOperation != "MAX" && parsedQuery.groupOperation != "AVG" && parsedQuery.groupOperation != "MIN" && parsedQuery.groupOperation != "SUM")
    {
        cout << "SYNTAX:ERROR" << endl;
        return false;
    }
    string temp = tokenizedQuery[8];
    temp = temp.substr(4, temp.length() - 5);
    parsedQuery.groupOperationColumn = temp;
    return true;
}

bool semanticParseGROUP()
{
    logger.log("semanticParseGROUP");

    if (tableCatalogue.isTable(parsedQuery.groupResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    Table table = *tableCatalogue.getTable(parsedQuery.groupRelationName);
    if (!table.isColumn(parsedQuery.groupColumnName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation";
        return false;
    }
    if (!table.isColumn(parsedQuery.groupOperationColumn))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation";
        return false;
    }
    return true;
}

void executeGROUP()
{
    logger.log("executeGROUP");
    vector<string> columns = {parsedQuery.groupColumnName, parsedQuery.groupOperation + parsedQuery.groupOperationColumn};
    Table *resultantTable = new Table(parsedQuery.groupResultRelationName, columns);
    Table table = *tableCatalogue.getTable(parsedQuery.groupRelationName);
    table.sortTable(parsedQuery.groupColumnName, 0, 10);
    Cursor cursor = table.getCursor();

    int groupColumnIndex = table.getColumnIndex(parsedQuery.groupColumnName);
    int groupOperationIndex = table.getColumnIndex(parsedQuery.groupOperationColumn);
    vector<int> row = cursor.getNext();
    vector<int> res(2);

    int min_val = INT_MAX, max_val = INT_MIN, cnt = 0, sum = 0;
    int cur_val = -1;
    while (!row.empty())
    {
        if(cur_val != -1 && cur_val != row[groupColumnIndex]) {
            res[0] = cur_val;
            if(parsedQuery.groupOperation == "MAX") {
                res[1] = max_val;
            } else if(parsedQuery.groupOperation == "MIN") {
                res[1] = min_val;
            } else if(parsedQuery.groupOperation == "SUM") {
                res[1] = sum;
            } else {
                res[1] = sum/cnt;
            }
            resultantTable->writeRow(res);
            min_val = INT_MAX, max_val = INT_MIN, cnt = 0, sum = 0;
        }
        cur_val = row[groupColumnIndex];
        max_val = max(max_val, row[groupOperationIndex]);
        min_val = min(min_val, row[groupOperationIndex]);
        sum += row[groupOperationIndex];
        cnt++;
        row = cursor.getNext();
    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    return;
}