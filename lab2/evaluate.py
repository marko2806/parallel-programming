from move import Move
from game import check_win, print_state
from tree import TreeNode
from game import move_possible, perform_move
from values import *


def evaluiraj_list(move: Move):
    if move.value.status == ZAVRSENO_RACUNANJE:
        return
    pobjeda = check_win(move.value.state)
    if pobjeda == 1:
        move.value.status = ZAVRSENO_RACUNANJE
        move.value.value = 1.0
        return
    elif pobjeda == -1:
        move.value.status = ZAVRSENO_RACUNANJE
        move.value.value = -1.0
        return

    if move.value.depth == 1:
        move.value.status = ZAVRSENO_RACUNANJE
        move.value.value = 0.0
        return

    move.value.status = POTREBAN_NOVI_ZADATAK
    move.value.value = -20.0
    return


def evaluiraj_podstalo(subtree: TreeNode):
    q = []
    node = subtree
    q.append(node)

    # dopunjavanje stabla ukoliko stablo nije do kraja stvoreno
    while len(q) > 0 and node.value.subtree_depth > 1:
        node = q.pop(0)
        if node.value.subtree_depth > 1:
            for i in range(NUM_OF_COLS):
                if (i + 1) > node.num_of_children:
                    if move_possible(node.value.state, i):
                        state = perform_move(node.value.state, i, node.value.made_by_player ^ -2)
                        child = node.add_child(Move(state, node.value.made_by_player ^ -2, node.value.depth - 1,
                                                    node.value.subtree_depth - 1))
                        q.append(child)
                    else:
                        child = node.add_child(Move(None, None, node.value.depth - 1, node.value.subtree_depth - 1))
                        child.value.status = NEMOGUCE_STANJE
                        child.value.value = -10.0
    #print("Dovrsio podstablo", flush=True)
    q.clear()
    q.append(subtree)

    while len(q) > 0:
        #print(f"Q len {len(q)}")
        node = q.pop(0)
        node.value.status = ZAPOCETO_RACUNANJE
        # ako je cvor list
        if node.value.subtree_depth == 1:
            evaluiraj_list(node)
        # inace
        else:
            # projeri jesu li sva djeca evaluirana i je li djeca zahtjevaju novi zadatak
            all_evaluated = True
            new_task_required = False
            for c in node.children:
                if c.value.status == POTREBAN_NOVI_ZADATAK:
                    new_task_required = True
                    all_evaluated = False
                    break
                elif c.value.status != ZAVRSENO_RACUNANJE:
                    all_evaluated = False
                    q.append(c)

            if all_evaluated and not new_task_required:
                all_win = True
                all_lose = True
                node_calculated = False
                total = 0.0
                for c in node.children:
                    if c.value.value > -1:
                        all_lose = False
                    if c.value.value != 1:
                        all_win = False

                    if c.value.value == 1.0 and c.value.made_by_player == CPU:
                        node.value.status = ZAVRSENO_RACUNANJE
                        node.value.value = 1.0
                        node_calculated = True
                        break

                    if c.value.value == -1.0 and c.value.made_by_player == HUMAN:
                        node.value.status = ZAVRSENO_RACUNANJE
                        node.value.value = -1.0
                        node_calculated = True
                        break
                    total += c.value.value

                if all_win and not node_calculated:
                    node.value.status = ZAVRSENO_RACUNANJE
                    node.value.value = 1.0
                elif all_lose and not node_calculated:
                    node.value.status = ZAVRSENO_RACUNANJE
                    node.value.value = -1.0
                elif not node_calculated:
                    node.value.status = ZAVRSENO_RACUNANJE
                    node.value.value = total / float(node.num_of_children)
            elif not new_task_required:
                q.append(node)
    #print("Dovrsio evaluaciju podstablo", flush=True)
    return subtree