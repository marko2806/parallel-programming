import game
import copy
from game import perform_move
from values import *


class Move:
    def __init__(self, state, player, depth=MAX_DEPTH, subtree_depth=SUBTREE_DEPTH):
        self.state = state
        self.status = NIJE_ZAPOCETO_RACUNANJE
        self.made_by_player = player
        self.subtree_depth = subtree_depth
        self.depth = depth
        self.value = -15.0


def make_move_from_current(current_move, col):
    next_player = CPU if current_move.made_by_player == HUMAN else HUMAN
    move = Move(copy.deepcopy(current_move.state), next_player, current_move.depth - 1)
    if game.move_possible(current_move.state, col):
        move.state = perform_move(move.state, col, move.made_by_player)
    else:
        move.state = None
        move.value = -10.0
    move.status = NIJE_ZAPOCETO_RACUNANJE
    move.made_by_player = next_player
    move.subtree_depth = -1
    move.depth = current_move.depth - 1
    return move
