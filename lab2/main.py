from mpi4py import MPI
from game import *
from move import Move, make_move_from_current
from task import Task, create_new_tasks
from tree import TreeNode, update_tree
from values import *
from evaluate import evaluiraj_podstalo
import time

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
num_processes = comm.Get_size()


if rank == 0:

    initial_state = [[0, 0, 0, 0, 0, 0, 0],
                     [0, 0, 0, 0, 0, 0, 0],
                     [0, 0, 0, 0, 0, 0, 0],
                     [0, 0, 0, 0, 0, 0, 0],
                     [0, 0, 0, 0, 0, 0, 0],
                     [0, 0, 0, 0, 0, 0, 0]]

    current_state = initial_state
    
    while check_win(current_state) != -1 and check_win(current_state) != 1:
        zadaci = []

        print_state(current_state)
        indx = int(input("Unesi poziciju: "))
        indx = indx - 1
        start_time = time.time()

        if move_possible(current_state, indx):
            current_state = perform_move(current_state, indx, HUMAN)

        print_state(current_state)

        first_move = Move(current_state, HUMAN, MAX_DEPTH)

        pocetni_potez = TreeNode(current_state)

        for i in range(NUM_OF_COLS):
            new_move = make_move_from_current(first_move, i)
            pocetni_potez.add_child(new_move)
            for j in range(NUM_OF_COLS):
                child_node = pocetni_potez.children[i]
                child_node.add_child(make_move_from_current(new_move, j))
                zadaci.append(Task(child_node.children[j]))

        if num_processes > 1:
            kraj = 0
            ended_processes = 0
            active_processes = []
            while not kraj:
                s = MPI.Status()
                got_message = comm.iprobe(source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG, status=s)
                source = s.Get_source()
                if got_message:
                    if s.Get_tag() == PODSTABLO:
                        m = comm.recv(source=source, tag=s.Get_tag())
                        if source in active_processes:
                            active_processes.remove(source)
                        update_tree(pocetni_potez, m, m.path)
                        create_new_tasks(zadaci, m)
                    elif s.Get_tag() == ZAHTJEV:
                        mes = comm.recv(source=source, tag=s.Get_tag())
                        if len(zadaci) > 0:
                            t = zadaci.pop(0)
                            m = t.move_node
                            m.value.subtree_depth = SUBTREE_DEPTH
                            active_processes.append(source)
                            comm.send(m, dest=source, tag=ZAHTJEV)
                        else:
                            if source in active_processes:
                                active_processes.remove(source)
                            comm.send(None, dest=source, tag=ZAHTJEV)                        
                elif len(zadaci) == 0 and len(active_processes) == 0:
                    kraj = 1
                    break

        else:
            while len(zadaci) > 0:
                t = zadaci.pop(0)
                m = t.move_node
                m.value.subtree_depth = SUBTREE_DEPTH
                m = evaluiraj_podstalo(m)
                update_tree(pocetni_potez, m, m.path)
                create_new_tasks(zadaci, m)
        max_ind = 0
        max_val = -1
        values = []
        for i in range(NUM_OF_COLS):
            evaluiraj_podstalo(pocetni_potez.children[i])
        

        for i in range(NUM_OF_COLS):
            if max_val < pocetni_potez.children[i].value.value \
                    and move_possible(current_state, i):
                max_ind = i
                max_val = pocetni_potez.children[i].value.value
            values.append(pocetni_potez.children[i].value.value)
        print(values)
        new_move = max_ind
        current_state = perform_move(current_state, new_move, CPU)
        print("Vrijeme racunanja :" + str(time.time() - start_time))

    processes_to_end = list(range(1, num_processes))
    s = MPI.Status()
    while comm.iprobe(source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG, status=s):
        comm.recv(None, source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG)
        comm.send(None, dest=s.Get_source(), tag=KRAJ_ALGORITMA)
        if s.Get_source() in processes_to_end:
            processes_to_end.remove(s.Get_source())

    if check_win(current_state) == -1:
        print_state(current_state)
        print("Pobijeda")
    elif check_win(current_state) == 1:
        print_state(current_state)
        print("Pc je pobijedio")
    
    for p in processes_to_end:
        comm.send(None, dest=p, tag=KRAJ_ALGORITMA)

elif rank != 0:
    kraj = 0
    vec_poslao = 0
    while not kraj:
        s = MPI.Status()
        flag = comm.iprobe(source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG, status=s)
        if flag:
            if s.Get_tag() == KRAJ_ALGORITMA:
                m = comm.recv(source=s.Get_source(), tag=s.Get_tag())
                vec_poslao = 0
                break
            elif s.Get_tag() == ZAHTJEV:
                m = comm.recv(source=s.Get_source(), tag=s.Get_tag())
                if m is not None:
                    m = evaluiraj_podstalo(m)
                    comm.send(m, dest=0, tag=PODSTABLO)
                vec_poslao = 0

        if not vec_poslao:
            comm.send(rank, dest=0, tag=ZAHTJEV)
            vec_poslao = 1

    print(f"Proces{rank} izasao iz petlje", flush=True)