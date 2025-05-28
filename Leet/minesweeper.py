import random

class Minesweeper:
    def __init__(self, rows, cols, num_mines):
        self.rows = rows
        self.cols = cols
        self.num_mines = num_mines
        self.board = [[' ' for _ in range(cols)] for _ in range(rows)]
        self.visible = [[False for _ in range(cols)] for _ in range(rows)]
        self.mines = set()
        self.generate_mines()
        self.populate_numbers()

    def generate_mines(self):
        while len(self.mines) < self.num_mines:
            r = random.randint(0, self.rows - 1)
            c = random.randint(0, self.cols - 1)
            if (r, c) not in self.mines:
                self.mines.add((r, c))
                self.board[r][c] = '*'

    def populate_numbers(self):
        directions = [(-1, -1), (-1, 0), (-1, 1),
                      (0, -1),         (0, 1),
                      (1, -1), (1, 0), (1, 1)]
        for r in range(self.rows):
            for c in range(self.cols):
                if self.board[r][c] == '*':
                    continue
                count = 0
                for dr, dc in directions:
                    nr, nc = r + dr, c + dc
                    if 0 <= nr < self.rows and 0 <= nc < self.cols:
                        if self.board[nr][nc] == '*':
                            count += 1
                self.board[r][c] = str(count) if count > 0 else ' '

    def print_board(self):
        print("   " + " ".join(str(i) for i in range(self.cols)))
        for r in range(self.rows):
            row_display = [self.board[r][c] if self.visible[r][c] else '#' for c in range(self.cols)]
            print(f"{r:2} {' '.join(row_display)}")

    def reveal(self, r, c):
        if self.board[r][c] == '*':
            self.visible[r][c] = True
            return False  # Game over
        self.dfs_reveal(r, c)
        return True

    def dfs_reveal(self, r, c):
        if not (0 <= r < self.rows and 0 <= c < self.cols):
            return
        if self.visible[r][c]:
            return
        self.visible[r][c] = True
        if self.board[r][c] == ' ':
            for dr in [-1, 0, 1]:
                for dc in [-1, 0, 1]:
                    if dr != 0 or dc != 0:
                        self.dfs_reveal(r + dr, c + dc)

    def play(self):
        while True:
            self.print_board()
            try:
                r, c = map(int, input("Enter row and column to reveal (e.g., 1 2): ").split())
                if not (0 <= r < self.rows and 0 <= c < self.cols):
                    print("Invalid coordinates.")
                    continue
                if not self.reveal(r, c):
                    self.visible = [[True for _ in range(self.cols)] for _ in range(self.rows)]
                    self.print_board()
                    print("💥 You hit a mine! Game over.")
                    break
                if self.check_win():
                    self.print_board()
                    print("🎉 You win!")
                    break
            except ValueError:
                print("Please enter valid integers separated by space.")

    def check_win(self):
        for r in range(self.rows):
            for c in range(self.cols):
                if not self.visible[r][c] and self.board[r][c] != '*':
                    return False
        return True


# Example usage:
game = Minesweeper(rows=5, cols=5, num_mines=5)
game.play()
