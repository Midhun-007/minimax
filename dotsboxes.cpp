#include <bits/stdc++.h>
using namespace std;

int boardSize;
vector<vector<int>> board;

bool isInsideBoard(int r,int c){
    return r>=0&&r<boardSize&&c>=0&&c<boardSize;
}
pair<int, int> boxLabelToCoords(const string &label)
{
    int c = label[0] - 'A';
    int row = stoi(label.substr(1));
    int r = boardSize - row;
    return {r, c};
}

string coordsToBoxLabel(int r, int c)
{
    string s;
    s.push_back(char('A' + c));
    s += to_string(boardSize - r);
    return s;
}
struct MoveCandidate
{
    int r, c, bit;
    int completed;
    bool makes3;
};
/*vector<MoveCandidate> AvailableMoves(vector<vector<int>> &board){
    vector<MoveCandidate> moves;
    for(int r=0;r<boardSize;r++){
        for(int c=0;c<boardSize;c++){
            int s=board[r][c];
            if((s&1)==0){
                moves.push_back({r,c,1,0,0});
            }
            if((s&2)==0){
                moves.push_back({r,c,2,0,0});
            }
            if((s&4)==0){
                moves.push_back({r,c,4,0,0});
            }if((s&8)==0){
                moves.push_back({r,c,8,0,0});
            }
            
        }
    }
    return moves;
}*/
// Generate available moves (free edges only)
vector<MoveCandidate> AvailableMoves(vector<vector<int>> &board){
    vector<MoveCandidate> moves;
    for(int r=0;r<boardSize;r++){
        for(int c=0;c<boardSize;c++){
            int s = board[r][c]; // 0 = free, 1 = taken
            if((s & 1) == 0) moves.push_back({r, c, 1, 0, 0}); // B
            if((s & 2) == 0) moves.push_back({r, c, 2, 0, 0}); // R
            if((s & 4) == 0) moves.push_back({r, c, 4, 0, 0}); // T
            if((s & 8) == 0) moves.push_back({r, c, 8, 0, 0}); // L
        }
    }
    return moves;
}
bool makesThree(int s, int edgeMask) {
    int newState = s | edgeMask;
    int edges = __builtin_popcount(newState); // counts 1-bits in int
    return (edges == 3);
}

bool completesBox(int s, int edgeMask) {
    int newState = s | edgeMask;
    int edges = __builtin_popcount(newState);
    return (edges == 4);
}
void SimulateMove(vector<vector<int>> &board,MoveCandidate& move){
    if(move.bit==1){
        if(completesBox(board[move.r][move.c],1)) move.completed++;
        if(isInsideBoard(move.r+1,move.c)){
            if(completesBox(board[move.r+1][move.c],4)) move.completed++;
        }
        if(makesThree(board[move.r][move.c],1)) move.makes3=true;
        if(isInsideBoard(move.r+1,move.c)){
            if(makesThree(board[move.r+1][move.c],4)) move.makes3=true;
        }

    }
    else if(move.bit==2){
        if(completesBox(board[move.r][move.c],2)) move.completed++;
        if(isInsideBoard(move.r,move.c+1)){
            if(completesBox(board[move.r][move.c+1],8)) move.completed++;
        }
        if(makesThree(board[move.r][move.c],2)) move.makes3=true;
        if(isInsideBoard(move.r,move.c+1)){
            if(makesThree(board[move.r][move.c+1],8)) move.makes3=true;
        }
    }
    else if(move.bit==4){
        if(completesBox(board[move.r][move.c],4)) move.completed++;
        if(isInsideBoard(move.r-1,move.c)){
            if(completesBox(board[move.r-1][move.c],1)) move.completed++;
        }
        if(makesThree(board[move.r][move.c],4)) move.makes3=true;
        if(isInsideBoard(move.r-1,move.c)){
            if(makesThree(board[move.r-1][move.c],1)) move.makes3=true;
        }
    }
    else if(move.bit==8){
        if(completesBox(board[move.r][move.c],8)) move.completed++;
        if(isInsideBoard(move.r,move.c-1)){
            if(completesBox(board[move.r][move.c-1],2)) move.completed++;
        }
        if(makesThree(board[move.r][move.c],8)) move.makes3=true;
        if(isInsideBoard(move.r,move.c-1)){
            if(makesThree(board[move.r][move.c-1],2)) move.makes3=true;
        }
    }

    
}
int evaluateMove(const vector<vector<int>> &board, MoveCandidate &move) {
    // Simulate the move to get completed boxes and 3-sided box info
    SimulateMove(const_cast<vector<vector<int>>&>(board), move);

    int score = 0;

    // 1. Reward completed boxes heavily
    score += move.completed * 1000;

    // 2. Penalty for creating 3-sided boxes (dangerous for opponent)
    if (move.makes3) score -= 500;

    // 3. Slight bonus for safe moves (doesn’t create 3-sided boxes and doesn’t complete a box)
    if (!move.makes3 && move.completed == 0) score += 50;

    // 4. Neighboring 2-sided boxes: potential to create chains
    int r = move.r, c = move.c;
    int neighborBonus = 0;
    vector<pair<int,int>> dirs = {{-1,0},{1,0},{0,-1},{0,1}}; // up, down, left, right
    for (auto [dr,dc] : dirs) {
        int nr = r + dr, nc = c + dc;
        if (nr >= 0 && nr < boardSize && nc >= 0 && nc < boardSize) {
            int edges = __builtin_popcount(board[nr][nc]);
            if (edges == 2) neighborBonus += 30; // potential to start chain
            if (edges == 1) neighborBonus += 10; // slightly better than isolated box
        }
    }
    score += neighborBonus;

    // 5. Optional: small bonus for edges not at corners (more flexible play)
    if ((r > 0 && r < boardSize-1) && (c > 0 && c < boardSize-1)) score += 5;

    return score;
}


/*int minimax(vector<vector<int>> &board, int depth, bool isMaximizingPlayer) {
    if (depth == 0) {
        // Leaf node evaluation
        return 0;
    }

    vector<MoveCandidate> moves = AvailableMoves(board);
    if (moves.empty()) {
        return 0; // No moves left → terminal state
    }

    int bestScore = isMaximizingPlayer ? INT_MIN : INT_MAX;

    for (auto move : moves) {
        // Simulate this move
        SimulateMove(board, move);

        // Apply move to board temporarily
        board[move.r][move.c] |= move.bit;
        if (move.bit == 1 && move.r + 1 < boardSize) board[move.r+1][move.c] |= 4;
        if (move.bit == 2 && move.c + 1 < boardSize) board[move.r][move.c+1] |= 8;
        if (move.bit == 4 && move.r - 1 >= 0) board[move.r-1][move.c] |= 1;
        if (move.bit == 8 && move.c - 1 >= 0) board[move.r][move.c-1] |= 2;

        // Decide whose turn next
        bool samePlayer = (move.completed > 0); // If we completed a box, same player goes again
        int score = evaluateMove(board, move);

        int result;
        if (samePlayer) {
            result = score + minimax(board, depth - 1, isMaximizingPlayer);
        } else {
            result = score + minimax(board, depth - 1, !isMaximizingPlayer);
        }

        // Undo move
        board[move.r][move.c] ^= move.bit;
        if (move.bit == 1 && move.r + 1 < boardSize) board[move.r+1][move.c] ^= 4;
        if (move.bit == 2 && move.c + 1 < boardSize) board[move.r][move.c+1] ^= 8;
        if (move.bit == 4 && move.r - 1 >= 0) board[move.r-1][move.c] ^= 1;
        if (move.bit == 8 && move.c - 1 >= 0) board[move.r][move.c-1] ^= 2;

        // Update best score
        if (isMaximizingPlayer) {
            bestScore = max(bestScore, result);
        } else {
            bestScore = min(bestScore, result);
        }
    }

    return bestScore;
}*/
int minimax(const vector<vector<int>> &board, int depth, bool isMaximizingPlayer, int alpha, int beta) {
    if (depth == 0) return 0;

    vector<MoveCandidate> moves = AvailableMoves(const_cast<vector<vector<int>>&>(board));
    if (moves.empty()) return 0;

    int bestScore = isMaximizingPlayer ? INT_MIN : INT_MAX;

    for (auto move : moves) {
        // Copy the board to simulate the move safely
        vector<vector<int>> tempBoard = board;

        // Apply move to tempBoard
        tempBoard[move.r][move.c] |= move.bit;
        if (move.bit == 1 && move.r + 1 < boardSize) tempBoard[move.r+1][move.c] |= 4;
        if (move.bit == 2 && move.c + 1 < boardSize) tempBoard[move.r][move.c+1] |= 8;
        if (move.bit == 4 && move.r - 1 >= 0) tempBoard[move.r-1][move.c] |= 1;
        if (move.bit == 8 && move.c - 1 >= 0) tempBoard[move.r][move.c-1] |= 2;

        // Simulate move for scoring
        SimulateMove(tempBoard, move);
        int score = evaluateMove(tempBoard, move);

        // Decide whose turn next
        bool samePlayer = (move.completed > 0);

        int result;
        if (samePlayer) {
            result = score + minimax(tempBoard, depth - 1, isMaximizingPlayer, alpha, beta);
        } else {
            result = score + minimax(tempBoard, depth - 1, !isMaximizingPlayer, alpha, beta);
        }

        // Update bestScore and alpha/beta
        if (isMaximizingPlayer) {
            bestScore = max(bestScore, result);
            alpha = max(alpha, bestScore);
            if (beta <= alpha) break; // Beta cutoff
        } else {
            bestScore = min(bestScore, result);
            beta = min(beta, bestScore);
            if (beta <= alpha) break; // Alpha cutoff
        }
    }

    return bestScore;
}



MoveCandidate findBestMove(const vector<vector<int>> &board, int depth) {
    vector<MoveCandidate> moves = AvailableMoves(const_cast<vector<vector<int>>&>(board));

    int bestVal = INT_MIN;
    MoveCandidate bestMove;
    if (!moves.empty()) bestMove = moves[0];

    for (auto move : moves) {
        // Copy the board to simulate the move safely
        vector<vector<int>> tempBoard = board;

        // Apply move to tempBoard
        tempBoard[move.r][move.c] |= move.bit;
        if (move.bit == 1 && move.r + 1 < boardSize) tempBoard[move.r+1][move.c] |= 4;
        if (move.bit == 2 && move.c + 1 < boardSize) tempBoard[move.r][move.c+1] |= 8;
        if (move.bit == 4 && move.r - 1 >= 0) tempBoard[move.r-1][move.c] |= 1;
        if (move.bit == 8 && move.c - 1 >= 0) tempBoard[move.r][move.c-1] |= 2;

        // Simulate move for scoring
        SimulateMove(tempBoard, move);
        int score = evaluateMove(tempBoard, move);

        // Decide whose turn next
        bool samePlayer = (move.completed > 0);
        int moveVal;
        if (samePlayer) moveVal = score + minimax(tempBoard, depth - 1, true,INT_MIN, INT_MAX);
        else moveVal = score + minimax(tempBoard, depth - 1, false,INT_MIN, INT_MAX);

        if (moveVal > bestVal) {
            bestVal = moveVal;
            bestMove = move;
        }
    }

    return bestMove;
}


void ApplyMove(vector<vector<int>> &board, const MoveCandidate &move) {
    int r = move.r, c = move.c;
    int bit = move.bit;

    // Mark the edge as taken in this box
    board[r][c] |= bit;

    // Propagate to adjacent box if necessary
    if(bit == 1 && isInsideBoard(r+1, c)) board[r+1][c] |= 4; // B -> T of box below
    if(bit == 2 && isInsideBoard(r, c+1)) board[r][c+1] |= 8; // R -> L of box right
    if(bit == 4 && isInsideBoard(r-1, c)) board[r-1][c] |= 1; // T -> B of box above
    if(bit == 8 && isInsideBoard(r, c-1)) board[r][c-1] |= 2; // L -> R of box left
}

int main()
{
    int board_size; 
    cin >> boardSize; cin.ignore();
    string player_id; 
    cin >> player_id; cin.ignore();
    
    // game loop
    while (1) {
        board.assign(boardSize, vector<int>(boardSize, 15));
        int myScore; 
        int oppScore; 
        cin >> myScore >> oppScore; cin.ignore();
        int num_boxes; 
        cin >> num_boxes; cin.ignore();
        //updating board with al the locked edges
        for(int i=0;i<num_boxes;i++){
        string box, sides;
        cin >> box >> sides; cin.ignore();
        auto [r, c] = boxLabelToCoords(box);

        // Convert free edges to bitmask
        int freeMask = 0;
        for(char ch : sides){
            if(ch == 'B') freeMask |= 1;
            else if(ch == 'R') freeMask |= 2;
            else if(ch == 'T') freeMask |= 4;
            else if(ch == 'L') freeMask |= 8;
        }

        // Update board: 1 = taken, 0 = free
        int takenMask = 15 ^ freeMask; // invert free edges -> taken
        board[r][c] = takenMask;
        // Propagate taken edges to neighbors
        /*if(board[r][c] & 1 && isInsideBoard(r+1, c)) board[r+1][c] |= 4; // B -> T
        if(board[r][c] & 2 && isInsideBoard(r, c+1)) board[r][c+1] |= 8; // R -> L
        if(board[r][c] & 4 && isInsideBoard(r-1, c)) board[r-1][c] |= 1; // T -> B
        if(board[r][c] & 8 && isInsideBoard(r, c-1)) board[r][c-1] |= 2; // L -> R*/
    }
        //Ensures Consistency across all the r and c values
        /*for(int r=0;r<boardSize;r++){
            for(int c=0;c<boardSize;c++){
                int s=board[r][c];
                if(s&1){
                    if(isInsideBoard(r+1,c)){
                        board[r+1][c]|=4;
                    }
                }
                if(s&2){
                    if(isInsideBoard(r,c+1)){
                        board[r][c+1]|=8;
                    }
                }
                if(s&4){
                    if(isInsideBoard(r-1,c)){
                        board[r-1][c]|=1;
                    }
                }
                if(s&8){
                    if(isInsideBoard(r,c-1)){
                        board[r][c-1]|=2;
                    }
                }
            }
        }*/
        
        MoveCandidate move = findBestMove(board, 2);
        
        if(move.r>=0){
            ApplyMove(board,move);
            string s=coordsToBoxLabel(move.r,move.c);
            if(move.bit==1){
                s+=" B";
            }
            else if(move.bit==2) s+=" R";
            else if(move.bit==4) s+=" T";
            else s+=" L";
            cout<<s<<endl;
            cout.flush();
        }
        else{
            cout<<"A1 B"<<endl;
            cout.flush();
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        //cout << "A1 B MSG bla bla bla..." << endl; // <box> <side> [MSG Optional message]
    }
}
