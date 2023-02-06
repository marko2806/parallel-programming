from tree import TreeNode
from values import *


class Task:
    def __init__(self, move_node: TreeNode):
        self.move_node = move_node


def create_new_tasks(tasks: list, subtree: TreeNode):
    q = []
    node = subtree
    q.append(node)
    ctr = 0
    while len(q) > 0:
        node = q.pop()

        if node.value.subtree_depth == 1 and node.value.status != ZAVRSENO_RACUNANJE:
            ctr += 1
            tasks.append(Task(node))
        else:
            for c in node.children:
                q.append(c)

    if subtree.value.status != ZAVRSENO_RACUNANJE:
        tasks.append(Task(subtree))
