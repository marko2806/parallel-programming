import copy
from values import *


def print_state(state):
    if state is not None:
        for i in range(NUM_OF_ROWS):
            s = ""
            for j in range(NUM_OF_COLS):
                character = "x" if state[i][j] == CPU else "-"
                if state[i][j] == HUMAN:
                    character = "o"
                s += str("{:3s}".format(character))
            print(s)
        print()
    else:
        print("Illegal state")


def perform_move(state, idx, player):
    row = 0
    new_state = copy.deepcopy(state)
    current = new_state[row][idx]
    while current == 0 and row < NUM_OF_ROWS:
        row += 1
        if row < NUM_OF_ROWS:
            current = new_state[row][idx]

    if row > 0:
        new_state[row - 1][idx] = player

    return new_state


def check_win(state):
    if state is None:
        return 0
    human_wins = False
    cpu_wins = False
    for i in range(NUM_OF_ROWS):
        for j in range(NUM_OF_COLS):
            # provjera vodoravno
            seq_cpu = 0
            seq_human = 0
            k = 0
            while k < 4 and k + j < NUM_OF_COLS:
                if state[i][k + j] == CPU:
                    seq_cpu += 1
                elif state[i][k+j] == HUMAN:
                    seq_human += 1
                k += 1

            if seq_human == 4:
                human_wins = True
            if seq_cpu == 4:
                cpu_wins = True


            # provjera okomito
            seq_cpu = 0
            seq_human = 0
            k = 0
            while k < 4 and k + i < NUM_OF_ROWS:
                if state[k + i][j] == CPU:
                    seq_cpu += 1
                elif state[k + i][j] == HUMAN:
                    seq_human += 1
                k += 1

            if seq_human == 4:
                human_wins = True
            if seq_cpu == 4:
                cpu_wins = True


            # provjera dijagonale prema dolje
            seq_cpu = 0
            seq_human = 0
            k = 0
            while k < 4 and k + i < NUM_OF_ROWS and k + j < NUM_OF_COLS:
                if state[k + i][k + j] == CPU:
                    seq_cpu += 1
                elif state[k + i][k + j] == HUMAN:
                    seq_human += 1
                k += 1

            if seq_human == 4:
                human_wins = True
            if seq_cpu == 4:
                cpu_wins = True


            # provjera dijagonale prema gore
            seq_cpu = 0
            seq_human = 0
            k = 0
            while k < 4 and i - k >= 0 and k + j < NUM_OF_COLS:
                if state[i - k][k + j] == CPU:
                    seq_cpu += 1
                elif state[i - k][k + j] == HUMAN:
                    seq_human += 1
                k += 1

            if seq_human == 4:
                human_wins = True

            if seq_cpu == 4:
                cpu_wins = True
    if human_wins:
        return -1
    elif cpu_wins:
        return 1
    return 0


def move_possible(state, move):
    if state is None:
        return False
    if state[0][move] == 0:
        return True
    return False


if __name__ == "__main__":
    initial_state = [[ 0,  0,  0,  0,  0,  0,  0],
                     [ 1,  0,  0,  0,  0,  0,  0],
                     [ 1,  0,  0,  0,  0,  0,  0],
                     [ 1,  0,  0,  0,  0,  0,  0],
                     [ 1,  0,  0,  0,  0,  0,  0],
                     [ -1, -1, -1, -1, 0,  0,  0]]

    print(check_win(initial_state))
