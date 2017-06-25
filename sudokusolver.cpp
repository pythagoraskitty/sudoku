#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>

using namespace std;

struct Puzzle {
    size_t base, square;
    vector<vector<size_t>> grid;
    vector<vector<set<int>>> possible;
    map<size_t, set<pair<size_t, size_t>>> pairsWithSize;
    
    Puzzle() : base(3) { init(); }
    Puzzle(size_t b) : base(b) { init(); }
    
    void printGrid() {
        for(size_t i = 0; i < square; i++) {
            cout << endl;
            for(size_t j = 0; j < square; j++) {
                printf("%2lu ", grid[i][j]);
            } 
        }
        cout << endl;
    }
    
    void printPossible(size_t row, size_t col) {
        set<int>::iterator it;
        for(it = possible[row][col].begin(); it != possible[row][col].end(); it++) cout << *it << " ";
        cout << endl;
    }
    
    void printPairsWithSize(size_t size) {
        set<pair<size_t, size_t>>::iterator it;
        for(it = pairsWithSize[size].begin(); it != pairsWithSize[size].end(); it++) cout << "(" << it->first << ", " << it->second << ") ";
        cout << endl;
    }
    
    void printPairMap() {
        cout << endl;
        for(size_t s = 0; s <= square + 1; s++) {
            cout << "pairs with set size " << s << ": ";
            printPairsWithSize(s);
        }
        cout << endl;
    }
    
private:    
    void init() {
        square = base * base;
        vector<size_t> row;
        row.assign(square, 0);
        grid.assign(square, row);
        set<int> empty;
        vector<set<int>> possibleRow;
        possibleRow.assign(square, empty);
        possible.assign(square, possibleRow);
    } 
};

class Solver {
public:
    size_t base, square;
    Puzzle board;
    
    Solver(vector<pair<size_t, pair<size_t, size_t>>> valsAtPos) : base(3) {
        if(!init()) cerr << "Error: puzzle board must have base > 0" << endl;
        loadValues(valsAtPos);
    }
    Solver(size_t b, vector<pair<size_t, pair<size_t, size_t>>> valsAtPos) : base(b) { 
        if(!init()) cerr << "Error: puzzle board must have base > 0" << endl;
        loadValues(valsAtPos);
    }
    
    void print() {
        board.printGrid();
    }
    
    bool loadValues(vector<pair<size_t, pair<size_t, size_t>>> valsAtPos) {
        if(base == 0) {
            cerr << "Error: puzzle board must have base > 0" << endl;
            return false;
        }
        
        vector<set<size_t>> rowPoss, colPoss;
        vector<vector<set<size_t>>> boxPoss;
        set<size_t> values;
        for(size_t i = 0; i < square; i++) values.insert(i + 1);
        rowPoss.assign(square, values);
        colPoss = rowPoss;
        vector<set<size_t>> boxPossRow;
        boxPossRow.assign(base, values);
        boxPoss.assign(base, boxPossRow);
        set<int> valFilled;
        for(size_t i = 0; i < square + 1; i++) valFilled.insert(-(i + 1));
        
        if(valsAtPos.empty()) {
            for(size_t i = 0; i < square; i++) {
                for(size_t j = 0; j < square; j++) {
                    board.possible[i][j].insert(values.begin(), values.end());
                    board.pairsWithSize[square].insert(make_pair(i, j));
                } 
            }
            return true;
        }
        
        for(size_t i = 0; i < valsAtPos.size(); i++) {
            size_t value = valsAtPos[i].first, row = valsAtPos[i].second.first, col = valsAtPos[i].second.second;
            if(value == 0 || value > square || row >= square || col >= square) {
                cerr << "Error: something out of range for value " << value << " at (" << row << ", " << col << ")";
                cerr << "; must have 1 <= value <= " << square << " and 0 <= row, col < " << square << endl;
                return false;
            }
            if(board.grid[row][col] != 0 && board.grid[row][col] != value) {
                cerr << "Error: position (" << row << ", " << col << ") already has value " << board.grid[row][col];
                cerr << ", but tried to assign new value " << value << endl;
                return false;
            }
            if(rowPoss[row].find(value) == rowPoss[row].end() || colPoss[col].find(value) == colPoss[col].end() || boxPoss[row/base][col/base].find(value) == boxPoss[row/base][col/base].end()) {
                cerr << "Invalid board: value " << value << " not possible at (" << row << ", " << col << ")";
                cerr << boolalpha << "; possible in row: " << (rowPoss[row].find(value) != rowPoss[row].end());
                cerr << boolalpha << "; possible in col: " << (colPoss[col].find(value) != colPoss[col].end());
                cerr << boolalpha << "; possible in box: " << (boxPoss[row/base][col/base].find(value) != boxPoss[row/base][col/base].end());
                cerr << endl;
                return false;
            }
            
            board.grid[row][col] = value;
            rowPoss[row].erase(value);
            colPoss[col].erase(value);
            boxPoss[row/base][col/base].erase(value);
        }
        
        for(size_t i = 0; i < square; i++) {
            for(size_t j = 0; j < square; j++) {
                if(board.grid[i][j] == 0){
                    vector<size_t> rowVals, colVals, boxVals, intersection1, intersection2;
                    rowVals.insert(rowVals.begin(), rowPoss[i].begin(), rowPoss[i].end());
                    colVals.insert(colVals.begin(), colPoss[j].begin(), colPoss[j].end());
                    boxVals.insert(boxVals.begin(), boxPoss[i/base][j/base].begin(), boxPoss[i/base][j/base].end());
                    
                    vector<size_t>::iterator it;
                    intersection1.assign(square, 0);
                    intersection2.assign(square, 0);
                    it = set_intersection(rowVals.begin(), rowVals.end(), colVals.begin(), colVals.end(), intersection1.begin());
                    intersection1.resize(it - intersection1.begin());
                    it = set_intersection(intersection1.begin(), intersection1.end(), boxVals.begin(), boxVals.end(), intersection2.begin());
                    intersection2.resize(it - intersection2.begin());
                    board.possible[i][j].insert(intersection2.begin(), intersection2.end()); 
                    
                    if(intersection2.empty()) {
                        cerr << "Invalid board: no possible values left at (" << i << ", " << j << ")" << endl;
                        return false;
                    }  
                } else {
                    board.possible[i][j] = valFilled;
                }
                
                board.pairsWithSize[board.possible[i][j].size()].insert(make_pair(i, j));
            }
        }
        
        return true;
    }
    
    bool solveBoard() {
        pair<bool, Puzzle> result = solveNext(board);
        if(result.first) {
            cout << endl << "*** Puzzle solved! ***" << endl;
            board = result.second;
        }
        return result.first;
    }
    
    void solveAndPrint() {
        bool solved = solveBoard();
        cout << endl << "Puzzle has solution: "<< boolalpha << solved << endl;
        if(solved) {
            cout << "Note: solution may not be unique" << endl;
            cout << endl << "Solved puzzle: " << endl;
            print();
            
        }
    }
    
private:
    bool init() {
        if(base == 0) return false;
        
        square = base * base;
        board = Puzzle(base);
        
        return true;
    }
    
    pair<bool, Puzzle> insertValueAtInPuzzle(size_t val, size_t row, size_t col, Puzzle puzz) {
        if(val == 0 || val > square || row >= square || col >= square) {
            cerr << "Error: out of range; value " << val << " at (" << row << ", " << col << ")" << endl;
            return make_pair(false, puzz);
        }
        if(puzz.grid[row][col] != 0) {
            cerr << "Error: trying to insert new value at (" << row << ", " << col << ")" << endl;
            return make_pair(false, puzz);
        } 
        if(puzz.possible[row][col].find(val) == puzz.possible[row][col].end()) {
            cerr << "Error: trying to insert unavailable value at (" << row << ", " << col << ")" << endl;
            return make_pair(false, puzz);
        }
        
        set<int> valFilled;
        for(size_t i = 0; i < square + 1; i++) valFilled.insert(-(i + 1));
        
        puzz.grid[row][col] = val;
        puzz.pairsWithSize[puzz.possible[row][col].size()].erase(make_pair(row, col));
        puzz.possible[row][col] = valFilled;
        puzz.pairsWithSize[puzz.possible[row][col].size()].insert(make_pair(row, col));
        
        for(size_t i = 0; i < square; i++) {
            if(puzz.grid[row][i] == 0 && puzz.possible[row][i].find(val) != puzz.possible[row][i].end()) {
                puzz.pairsWithSize[puzz.possible[row][i].size()].erase(make_pair(row, i));
                puzz.possible[row][i].erase(val);
                puzz.pairsWithSize[puzz.possible[row][i].size()].insert(make_pair(row, i));
            }
            if(puzz.grid[i][col] == 0 && puzz.possible[i][col].find(val) != puzz.possible[i][col].end()) {
                puzz.pairsWithSize[puzz.possible[i][col].size()].erase(make_pair(i, col));
                puzz.possible[i][col].erase(val);
                puzz.pairsWithSize[puzz.possible[i][col].size()].insert(make_pair(i, col));
            }
        }
        for(size_t i = 0; i < base; i++) {
            for(size_t j = 0; j < base; j++) {
                size_t r = base * (row/base) + i, c = base * (col/base) + j;
                if(puzz.grid[r][c] != 0 && puzz.possible[r][c].find(val) != puzz.possible[r][c].end()) {
                    puzz.pairsWithSize[puzz.possible[r][c].size()].erase(make_pair(r, c));
                    puzz.possible[r][c].erase(val);
                    puzz.pairsWithSize[puzz.possible[r][c].size()].insert(make_pair(r, c));
                }
            }
        }
        
        return make_pair(true, puzz);
    }
    
    pair<bool, Puzzle> solveNext(Puzzle puzz) {
        map<size_t, set<pair<size_t, size_t>>>::iterator it = puzz.pairsWithSize.begin();
        if(it->first == 0 && !it->second.empty()) {
            return make_pair(false, puzz);
        }
        while(it != puzz.pairsWithSize.end() && it->second.empty()) it++;
        if(it == puzz.pairsWithSize.end()) {
            cerr << "Error: map pairsWithSize is empty" << endl;
            return make_pair(false, puzz);
        }
        if(it->first > square + 1) {
            cerr << "Error: map continues out of range with index " << it->first << endl;
            return make_pair(false, puzz);
        }
        if(it->first == square + 1) {
            // puzzle solved!
            return make_pair(true, puzz);
        }
        
        pair<size_t, size_t> position = *(it->second.begin());
        size_t row = position.first, col = position.second;
        
        set<int>::iterator pt = puzz.possible[row][col].begin();
        pair<bool, Puzzle> next;
        bool done = false;
        
        while(pt != puzz.possible[row][col].end() && !done) {
            next = insertValueAtInPuzzle(*pt, row, col, puzz);
            if(!next.first) {
                cerr << "Error inserting value " << *pt << " at (" << row << ", " << col << ")" << endl;
                return make_pair(false, puzz);
            }
            next = solveNext(next.second);
            done = next.first;
            pt++;
        }
        
        if(done) return make_pair(true, next.second);
        return make_pair(false, puzz);
    } 
};

void findSolution(vector<pair<size_t, pair<size_t, size_t>>> valsAtPos) {
    Solver sol(valsAtPos);
    cout << "Initial puzzle: " << endl;
    sol.print();
    sol.solveAndPrint();
}

void findSolution(size_t base, vector<pair<size_t, pair<size_t, size_t>>> valsAtPos) {  
    Solver sol(base, valsAtPos);
    cout << "Initial puzzle: " << endl;
    sol.print();
    sol.solveAndPrint();
}

vector<pair<size_t, pair<size_t, size_t>>> parseInput9x9(vector<string> board) {
    vector<pair<size_t, pair<size_t, size_t>>> valsAtPos;
    for(size_t i = 0; i < board.size(); i++) {
        for(size_t j = 0; j < board[i].size(); j++) {
            if(board[i][j] >= '1' && board[i][j] <= '9') {
                valsAtPos.push_back(make_pair(board[i][j] - '0', make_pair(i, j)));
            }
        }
    }
    return valsAtPos;
}

void findSolution9x9(vector<string> board) {  
    vector<pair<size_t, pair<size_t, size_t>>> valsAtPos = parseInput9x9(board);
    Solver sol(valsAtPos);
    cout << "Initial puzzle: " << endl;
    sol.print();
    sol.solveAndPrint();
}

// Need better way to input puzzle, eg read from file

int main(int argc, char *argv[]) {
    
    vector<string> board = {
        "-76-9--2-",
        "2--7-----",
        "----4---3",
        "193----4-",
        "--7-1-8--",
        "-4----132",
        "9---8----",
        "-----4--5",
        "-8--2-31-"
    };
    
    findSolution9x9(board);
}