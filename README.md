# ♟️ C Chess Engine (Console)

A complete Chess game developed in **C**, featuring an Artificial Intelligence based on the **Minimax** algorithm with **Alpha-Beta Pruning** and heuristic board evaluation.

The project was made using **Single Responsibility** principles.

## 🧠 Artificial Intelligence

The computer makes decisions by calculating thousands of moves ahead. The "intelligence" consists of:

1.  **Minimax Algorithm:** Recursive decision tree to predict opponent moves.
2.  **Alpha-Beta Pruning:** Optimization technique that ignores tree branches that don't need to be calculated, significantly improving AI performance.
3.  **Heuristic Evaluation Function:** A comprehensive static analysis algorithm that computes the **net advantage** (AI Score minus Opponent Score) by assessing:
    * **Material Balance:** Weighted values for each piece type.
    * **Dynamic Mobility Analysis:** Real-time calculation of available moves to reward board control and flexibility.
    * **King Safety & Threat Detection:** Penalties for checks, exposed King positions, and hanging pieces (undefended units).
4.  **Move Ordering Optimization:** Before recursion, moves are sorted based on a preliminary heuristic (e.g., capturing high-value targets first). This maximizes the efficiency of Alpha-Beta Pruning by finding cut-offs earlier.

## ✨ Features

* **Single Player:** Play against the computer (Black pieces).
* **Save/Load System:** Save your match to a binary file and resume later.
* **User System:** User registration/login and win/loss statistics.
* **Full Validation:** Complete movement rules for all pieces, including Check and Checkmate detection.
* **Visualization:** Board rendered in the terminal using ANSI colors.

## 📂 Project Architecture

The code is organized into modules to facilitate maintenance and scalability:

* `main.c`: Entry point and main game loop.
* `ia.c` / `ia.h`: Minimax logic, heuristics, and move generation.
* `regras.c` / `regras.h`: Move validation, Check/Checkmate detection.
* `tabuleiro.c` / `tabuleiro.h`: Data structures and visual rendering.
* `sistema.c` / `sistema.h`: Menus, file handling, and authentication.
* `defs.h`: Global definitions and Structs.

## 🚀 How to Compile and Run

### Prerequisites
* GCC Compiler (MinGW on Windows or native GCC on Linux).

### Compiling
Since the project uses multiple files, use the following command in your terminal to compile everything together:

```bash
gcc main.c tabuleiro.c regras.c ia.c sistema.c -o chess_game
```
### Running
```bash
# Windows
.\chess_game.exe

# Linux
./chess_game
```
## 🎮 Controls
The game uses standard algebraic notation coordinates.

* **To move**: Type the origin square followed by the destination square.

  * Example: e2 e4 (Moves piece from E2 to E4).

### Commands:

* `salvar`: Saves the current game state.

* `sair`: Exits the application.

Developed by Guilherme Henrique.
