/**
 * @brief The Matrix class implements matrix. 
 *
 */
class Matrix
{
public:
    string sourceFileName = "";
    string matrixName = "";
    uint columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    uint maxRowsPerBlock = 0;
    uint matrixSize = 0;
    
    bool blockify();
    Matrix();
    Matrix(string matrixName);
    bool load();
    void print();
    void makePermanent();
    bool isPermanent();
    // void getNextPage(Cursor *cursor);
    // Cursor getCursor();
    void unload();
    bool transpose();
    int getRowSize(int pageIndex);
    int getColumnSize(int pageIndex);

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout)
    {
        logger.log("Matrix::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row 
     */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Matrix::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};