#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    uint maxRowCount;
    if(tableCatalogue.isTable(tableName))
    {
        Table table = *tableCatalogue.getTable(tableName);
        this->columnCount = table.columnCount;
        maxRowCount = table.maxRowsPerBlock;
        this->rowCount = table.rowsPerBlockCount[pageIndex];
    } else if(matrixCatalogue.isMatrix(tableName)) 
    {
        Matrix matrix = *matrixCatalogue.getMatrix(tableName);
        this->columnCount = matrix.getColumnSize(pageIndex);
        this->rowCount = matrix.getRowSize(pageIndex);
        maxRowCount = this->rowCount;
    }
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    // cout << columnCount << "\n";
    
    ifstream fin(pageName, ios::in);
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page2");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
    logger.log("Page::writePage end");
}

void Page::transposePage()
{
    logger.log("Page::transposePage");
    vector<vector<int>> transposeRows(this->columnCount, vector<int>(this->rowCount, 0));
    for(int rowCounter = 0; rowCounter < this->rowCount; rowCounter++) {
        for(int columnCounter = 0; columnCounter < this->columnCount; columnCounter++) {
            transposeRows[columnCounter][rowCounter] = this->rows[rowCounter][columnCounter];
        }
    }
    this->rows = transposeRows;
    this->rowCount = transposeRows.size();
    this->columnCount = transposeRows[0].size();
    this->writePage();
}

vector<vector<int>> Page::getRows()
{
    logger.log("Page::getRows");
    vector<vector<int>> retRows;
    for(int i = 0; i < this->rowCount; i++) {
        retRows.push_back(rows[i]);
    }
    return retRows;
}

void Page::assignRows(vector<vector<int>> newRows)
{
    this->rows = newRows;
    this->rowCount = newRows.size();
    this->writePage();
}