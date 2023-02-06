class TreeNode:
    def __init__(self, value):
        self.value = value
        self.children = []
        self.num_of_children = 0
        self.parent = None
        self.path = []

    def add_child(self, value):
        node = TreeNode(value)
        self.children.append(node)
        self.num_of_children += 1
        node.parent = self
        node.path = self.path.copy() + [self.num_of_children - 1]
        return node

    def get_subtree(self, depth):
        tree = TreeNode(self.value)
        root_depth = self.value.depth
        min_allowed_depth = root_depth - depth

        nodes = (self, tree)
        q = [nodes]

        while len(q) > 0:
            current_tree_node, new_tree_node = q.pop(0)
            if current_tree_node.value.depth >= min_allowed_depth:
                for c in current_tree_node.children:
                    new_node = new_tree_node.add_child(c.value)
                    new_node.path = c.path
                    q.append((c, new_node))
        tree.path = self.path
        return tree


def update_tree(root, subtree, path):
    node = root
    for p in path:
        node = node.children[p]
    tree_node = node
    subtree_node = subtree

    q = [(tree_node, subtree_node)]

    while len(q) > 0:
        tree_node, subtree_node = q.pop()
        tree_node.value = subtree_node.value

        for i in range(subtree_node.num_of_children):
            if i + 1 > tree_node.num_of_children and subtree_node.children[i] is not None:
                tree_node.children.append(TreeNode(None))
            if subtree_node.children[i] is not None:
                q.append((tree_node.children[i], subtree_node.children[i]))


if __name__ == "__main__":
    root = TreeNode(0)

    n1 = root.add_child(1)

    n3 = n1.add_child(3)
    n4 = n1.add_child(4)

    n2 = root.add_child(2)

    s = n2.get_subtree(2)

    s.value = -1
    s.add_child(-1)
    s.add_child(-2)
    print("Subtree path")
    print(s.path)
    update_tree(root, s, [1])

    print(root.children[1].value)
    print(root.children[1].children[0].value)
    print(root.children[1].children[1].value)