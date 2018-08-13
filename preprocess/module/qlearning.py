import numpy as np
import numpy as geek
import random
import sys
import glob
import csv
import os

def create_qtable(state, action):
        return np.zeros(shape=(state, action))   # state => RSSI / action =>sector
        # | sector 1 | sector 2 | sector 3 . . .
        # | RSSI     | RSSI     | RSSI     . . .
        # | .
        # | .
        # | .

class Q_learning:
    def __init__(self, table, state, action):
        # self.qtable = create_qtable(state, action)
        self.qtable = table
        self.alpha = 0.5    # learning rate
        self.gamma = 0.5
        self.epsilon_threshold = 0.99

    def set_alpha(self, alpha):
        self.alpha = alpha

    def set_gamma(self, gamma):
        self.gamma = gamma

    # TD : Temporal-difference
    def update_qtable_TD(self, now_state, now_action, next_state, reward):
        self.qtable[now_state][now_action] = (1 - self.alpha)*float(self.qtable[now_state][now_action]) + \
                          self.alpha * (reward + self.gamma * float(max(self.qtable[next_state])))

    # MC : Monte Carlo
    def update_qtable_MC(self, now_state, now_action, reward):
        # if(abs(self.qtable[now_state][now_action]) == 0):
        #     self.qtable[now_state][now_action] = -1000
        # else:
        self.qtable[now_state][now_action] = (1 - self.alpha)*float(self.qtable[now_state][now_action]) + \
                          self.alpha * float(reward)

    def get_action(self, state):
        epsilon = random.random()
        return self.qtable[state].tolist().index(max(self.qtable[state].tolist())) \
               # if (epsilon < self.epsilon_threshold) \                        <------- challenge!
               # else random.randint(0,len(self.qtable[state].tolist())-1)

    def get_table(self):
        return self.qtable

    def print_qtable(self):
        print self.qtable

    def save_module(self, filename,  state_info, ap_counts):
        with open(filename, "w") as myfile:
            # for state_stack in state_info:
            #     state_stack = state_stack.split(",")
            #     for i in range(2, ap_counts + 2):
            #         # print(str(state_stack[i]).rstrip())
            #         myfile.write(str(state_stack[i]).rstrip() + ",")
            for states in self.qtable:
                for action in states:
                    myfile.write(str(action) + ",")

print("\n")

def openfile(filename):
    return open(filename, 'r')

# python qlearning.py 1

def csv_to_list(data_set, file):
    array = file.readlines()[0].split(",")
    array = array[:-1]
    # print(array)
    data_set = np.reshape(array, (10, 20))
    # print(data_set)

ap_stack = []
fd_ap = openfile("C:/Users/user/Desktop/preprocess/data/ap_state/output/ap_state.csv")
next(fd_ap)
for line in fd_ap:
    ap_stack.append(line)

rx_range = 10
tx_range = 20

ap_range = 2

decline_rate = 0      # set to zero, cause' there are no relationship between states
learning_rate = 0.3     # Q(s1, a1) - alpha * ( recent_data - Q(s1, a1) ), the speed of convergence

data_set = geek.zeros([rx_range, tx_range])

table_filename = 'C:/Users/user/Desktop/preprocess/module/table' + str(sys.argv[1]) + '.csv'

fd = openfile("C:/Users/user/Desktop/preprocess/module/temp.csv")

if(os.path.isfile(table_filename)):
    f = openfile(table_filename)
    csv_to_list(data_set, f)

else:
    data_set = geek.zeros([rx_range, tx_range])

# print(np.size(data_set))
qlearning = Q_learning(data_set, rx_range, tx_range)
qlearning.set_alpha(learning_rate)
qlearning.set_gamma(decline_rate)

rx = 0
tx = 0
for rows in fd:
    # print(rows.split(","))
    for col in rows.split(","):
        if col.strip() is not "":
            # print("#" + str(tx + 1) + " : " + str(col.strip()))
            qlearning.update_qtable_MC(rx, tx, col)
            tx = tx + 1
    # print("#" + str(rx + 1))
    rx = rx + 1
    tx = 0

print("[ ap state " + str(sys.argv[1]) + " ]\n")

print("[ q table ]\n")
qlearning.print_qtable()
print("\n")

qlearning.save_module(table_filename, ap_stack, ap_range)









#
