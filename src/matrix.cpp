#include "global.h"

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
    this->maxRowsPerBlock = (uint)(sqrt(BLOCK_SIZE)*16);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates Matrix
 * statistics.
 *
 * @return true if the Matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line, word;
    uint count = 0;
    if (getline(fin, line))
    {
        fin.close();
        stringstream lineStream(line);
        while(getline(lineStream, line, ','))
        {
            count++;
        }
        this->matrixSize = count;
        return this->blockify();
    }
    fin.close();
    return false;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->matrixSize, 0);
    vector<vector<int>> tempRows(this->maxRowsPerBlock, row);
    int rowCounter = 0;
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->matrixSize; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            // cout << stoi(word) << "\n";
            row[columnCounter] = stoi(word);
            tempRows[rowCounter][columnCounter] = row[columnCounter];
        }
        rowCounter++;
        if (rowCounter == this->maxRowsPerBlock)
        {
            int BlocksPerRow = (this->matrixSize + this->maxRowsPerBlock - 1)/(this->maxRowsPerBlock);
            for(int i = 0; i < BlocksPerRow; i++)
            {
                vector<vector<int>> rowsInPage;
                for(int j = 0; j < rowCounter; j++) {
                    int startcol = i*this->maxRowsPerBlock;
                    vector<int> temp;
                    for(int k = 0; k < this->maxRowsPerBlock; k++) {
                        // cout << i << " " << j << " " << startcol + k << "\n";
                        if(startcol + k == this->matrixSize) {
                            break;
                        }
                        temp.emplace_back(tempRows[j][startcol + k]);
                    }
                    rowsInPage.emplace_back(temp);
                }
                bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, rowCounter);
                this->blockCount++;
            }
            rowCounter = 0;
        }
    }
    if (rowCounter)
    {
        // cout << rowCounter << "\n";
        int BlocksPerRow = (this->matrixSize + this->maxRowsPerBlock - 1)/(this->maxRowsPerBlock);
        for(int i = 0; i < BlocksPerRow; i++)
        {
            vector<vector<int>> rowsInPage;
            for(int j = 0; j < rowCounter; j++) {
                int startcol = i*this->maxRowsPerBlock;
                vector<int> temp;
                for(int k = 0; k < this->maxRowsPerBlock; k++) {
                    // cout << i << " " << j << " " << startcol + k << "\n";
                    if(startcol + k == this->matrixSize) {
                        break;
                    }
                    temp.emplace_back(tempRows[j][startcol + k]);
                }
                rowsInPage.emplace_back(temp);
            }
            bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, rowCounter);
            this->blockCount++;
        }
    }
    return true;
}


/**
 * @brief Function prints the first few rows of the Matrix. If the Matrix contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    
    vector<int> row(this->matrixSize, 0);
    vector<vector<int>> tempRows(this->maxRowsPerBlock, row);
    int BlocksPerRow = (this->matrixSize + this->maxRowsPerBlock - 1)/(this->maxRowsPerBlock);
    for(int pageCounter = 0; pageCounter < this->blockCount; pageCounter++) {
        int startcol = (pageCounter%BlocksPerRow)*this->maxRowsPerBlock;
        vector<vector<int>> pgRow = bufferManager.getPage(this->matrixName, pageCounter).getRows();
        for(int i = 0; i < pgRow.size(); i++) {
            for(int j = 0; j < pgRow[0].size(); j++) {
                tempRows[i][startcol + j] = pgRow[i][j]; 
            }
        }
        if(pageCounter % BlocksPerRow == BlocksPerRow - 1) {
            for(int i = 0; i < pgRow.size(); i++) {
                this->writeRow(tempRows[i], cout);
            }
        }
    }
    
    // printRowCount(this->rowCount);
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    vector<int> row(this->matrixSize, 0);
    vector<vector<int>> tempRows(this->maxRowsPerBlock, row);
    int BlocksPerRow = (this->matrixSize + this->maxRowsPerBlock - 1)/(this->maxRowsPerBlock);
    // cout << BlocksPerRow << " " << this->blockCount << "\n";
    for(int pageCounter = 0; pageCounter < this->blockCount; pageCounter++) {
        int startcol = (pageCounter%BlocksPerRow)*this->maxRowsPerBlock;
        vector<vector<int>> pgRow = bufferManager.getPage(this->matrixName, pageCounter).getRows();
        for(int i = 0; i < pgRow.size(); i++) {
            for(int j = 0; j < pgRow[0].size(); j++) {
                // cout << i << " " << startcol + j << "\n";
                tempRows[i][startcol + j] = pgRow[i][j]; 
            }
        }
        if(pageCounter % BlocksPerRow == BlocksPerRow - 1) {
            for(int i = 0; i < pgRow.size(); i++) {
                this->writeRow(tempRows[i], fout);
            }
        }
    }
    fout.close();
}

/**
 * @brief Function to check if Matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the Matrix from the database by deleting
 * all temporary files created as part of this Matrix
 *
 */
void Matrix::unload(){
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

bool Matrix::transpose()
{
    logger.log("Matrix::transpose");
    int BlocksPerRow = (this->matrixSize + this->maxRowsPerBlock - 1)/(this->maxRowsPerBlock);
    for(int pageIndex = 0; pageIndex < this->blockCount; pageIndex++)
    {
        bufferManager.getPage(this->matrixName, pageIndex).transposePage();
        bufferManager.deleteFromPool("../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex));
    }
    for(int pageIndex = 0; pageIndex < this->blockCount; pageIndex++)
    {
        int rowNum = (pageIndex/BlocksPerRow);
        int colNum = (pageIndex % BlocksPerRow);
        if(rowNum > colNum) 
        {
            int swapIndex = colNum*BlocksPerRow + rowNum;
            Page pg1 = bufferManager.getPage(this->matrixName, pageIndex);
            Page pg2 = bufferManager.getPage(this->matrixName, swapIndex);
            rename(pg1.pageName.c_str(), "temp.csv");
            rename(pg2.pageName.c_str(), pg1.pageName.c_str());
            rename("temp.csv", pg2.pageName.c_str());
            swap(pg1.pageName, pg2.pageName);
        }
    } 
}

int Matrix::getColumnSize(int pageIndex)
{
    int BlocksPerRow = (this->matrixSize + this->maxRowsPerBlock - 1)/(this->maxRowsPerBlock);
    if(pageIndex % BlocksPerRow == BlocksPerRow - 1) {
        return this->matrixSize - (BlocksPerRow - 1)*this->maxRowsPerBlock;
    } else {
        return this->maxRowsPerBlock;
    }
}

int Matrix::getRowSize(int pageIndex)
{
    int BlocksPerRow = (this->matrixSize + this->maxRowsPerBlock - 1)/(this->maxRowsPerBlock);
    if(pageIndex / BlocksPerRow == BlocksPerRow - 1) {
        return this->matrixSize - (BlocksPerRow - 1)*this->maxRowsPerBlock;
    } else {
        return this->maxRowsPerBlock;
    }
}