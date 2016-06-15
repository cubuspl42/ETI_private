#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <cassert>
#include <iostream>
#include <stack>
#include <thread>
#include <vector>
#include <chrono>

const int channelId = 1338;
const int enterType = 1;
const int waitType = 2;

struct Message {
    long mtype;
    int n;
};

#define eassert(e) \
if (!(e)) perror(nullptr); \
assert(e);

thread_local int ti = 0;
thread_local std::string op;

struct Stack {
    std::stack<int> s;
    int c = -1;
    int n = 0;

    Stack() {
        c = msgget(channelId, IPC_CREAT | 0666);
        eassert(c > -1);

        {
            Message m;
            while(msgrcv(c, (struct msgbuf *)&m, sizeof(m), enterType, IPC_NOWAIT) > -1) {}
            while(msgrcv(c, (struct msgbuf *)&m, sizeof(m), waitType, IPC_NOWAIT) > -1) {}
        }

        Message m {enterType, 0};
        int r = msgsnd(c, (struct msgbuf *)&m, sizeof(m), IPC_NOWAIT);
        eassert(r > -1);
    }

    int enter() {
        Message m;
        std::cout << op + "+enter:" + std::to_string(ti) + "(" + std::to_string(n) + ")" << std::endl;
        int r = msgrcv(c, (struct msgbuf *)&m, sizeof(m), enterType, 0);
        assert(m.mtype == enterType);
        std::cout << op + "-enter:" + std::to_string(ti) + "[" + std::to_string(m.n) + "]" << std::endl;
        eassert(r > -1);
        eassert(m.n >= 0);
        return m.n;
    }

    void exit(int n) {
        Message m {enterType, n};
        int r = msgsnd(c, (struct msgbuf *)&m, sizeof(m), IPC_NOWAIT);
        eassert(r > -1);
    }

    int wait(int n) {
        exit(n + 1);

        Message m;
        std::cout << op + "+wait:" + std::to_string(ti) + "(" + std::to_string(n) + ")" << std::endl;
        int r = msgrcv(c, (struct msgbuf *)&m, sizeof(m), waitType, 0);
        assert(m.mtype == waitType);
        std::cout << op + "-wait:" + std::to_string(ti) + "[" + std::to_string(m.n) + "]" << std::endl;
        eassert(r > -1);

        return m.n - 1;
    }

    void notify(int n) {
        std::cout << op + "_notify(" + std::to_string(n) + ")" << std::endl;
        eassert(n >= 0);
        if (n > 0) {
            Message m {waitType, n};
            int r = msgsnd(c, (struct msgbuf *)&m, sizeof(m), IPC_NOWAIT);
            eassert(r > -1);
        } else {
            exit(0);
        }
    }

    // CONDITION VARIABLE PREDICATE
    bool isNotEmpty() {
        return s.size() > 0;
    }

    void push(int i) {
        op = "push";
        int n = enter();
        { // CRITICAL SECTION
            s.push(i);
        }
        notify(n);
    }

    int pop() {
        op = "pop";
        int n = enter();
        int r = 0;
        { // CRITICAL SECTION
            while(!isNotEmpty()) {
                n = wait(n);
            }
            { // CRITICAL SUB-SECTION WITH PRECONDITION: isNotEmpty()
                eassert(isNotEmpty());
                r = s.top();
                s.pop();
            }
        }
        exit(n);
        return r;
    }
};

Stack s;
int T = 100;
int X = 45;

void t(int i) {
    ti = i;
    std::cout << "Thread started..." << std::endl;
    while(true) {
        int r = rand();
        if (i < X) {
            s.push(r);
            std::cout << "s.push(" + std::to_string(r) + ")" << std::endl;
        } else {
            int a = s.pop();
            std::cout << "s.pop() == " + std::to_string(a) << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::cout << "Starting program..." << std::endl;

    std::vector<std::thread> v;
    for (int i = 0; i < T; ++i) {
        std::cout << "Starting thread..." << std::endl;
        v.emplace_back(t, i);
    }

    for (auto &t : v) {
        std::cout << "Joining..." << std::endl;
        t.join();
    }
}
