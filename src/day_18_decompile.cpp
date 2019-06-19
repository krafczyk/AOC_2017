#include <vector>
#include <iostream>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <sstream>
#include <cstdio>

typedef long type_t;

type_t Pow(type_t base, type_t i) {
    type_t answer = 1;
    while(i != 0) {
        answer *= base;
        i -= 1;
    }
    return answer;
}

size_t num_vals_sent = 0;
std::queue<type_t> msg_queues[2];
std::mutex msg_mutexes[2];
std::atomic<bool> waiting[2];

auto send_value = [](int this_prog, int other_prog, type_t v) {
    // Try to get a lock.
    msg_mutexes[other_prog].lock();
    std::stringstream ss;
    ss << "program " << this_prog << " sends the value " << v;
    printf("%s\n", ss.str().c_str());
    msg_queues[other_prog].push(v);
    msg_mutexes[other_prog].unlock();
    if(this_prog == 1) {
        num_vals_sent += 1;
    }
};

auto fetch_queue_value = [](int this_prog, int other_prog, type_t& v) {
    // Acquire the lock, or wait until the queue is full
    // as long as the other thread isn't waiting.
    while(true) {
        // We first need to acquire the lock.
        while(!msg_mutexes[this_prog].try_lock()) {
            // We failed to get the lock,
            // the other thread must be working.
            // Check that it isn't stalled.
            if(waiting[other_prog].load()) {
                // The other program is waiting, which means it must be done with our lock.
                if(!msg_mutexes[this_prog].try_lock()) {
                    std::cerr << "We should've been able to get the lock this time! program " << this_prog << std::endl;
                    throw;
                }
                // Check if our queue is empty.
                if(msg_queues[this_prog].size() == 0) {
                    // The other thread is stalled AND we have no messages, DEADLOCK
                    // Indicate we're waiting
                    waiting[this_prog].store(true);
                    return false;
                }
            }
        }
        // We got the lock
        if(msg_queues[this_prog].size() > 0) {
            // We can get a value!
            // Indicate we are no longer waiting.
            waiting[this_prog].store(false);
            // Get the value
            v = msg_queues[this_prog].front();
            msg_queues[this_prog].pop();
            std::stringstream ss;
            ss << "program " << this_prog << " receives the value " << v;
            printf("%s\n", ss.str().c_str());
            // Free the lock
            msg_mutexes[this_prog].unlock();
            break;
        } else {
            // There are no messages
            // Indicate we need to wait.
            waiting[this_prog].store(true);
            // Free the lock.
            msg_mutexes[this_prog].unlock();
        }
    }
    return true;
};

auto program = [](type_t this_prog) {
    type_t other_prog = (this_prog+1)%2;
    // We're program 0, generate the new values
    if(this_prog == 0) {
        type_t a = Pow(2,31)-1;
        type_t p = 826;
        // Acquire the lock
        msg_mutexes[other_prog].lock();
        for(size_t i=0; i < 127; ++i) {
            p *= 8505;
            p %= a;
            p *= 129749;
            p += 12345;
            p %= a;
            // Add the value to the other queue
            msg_queues[other_prog].push(p%10000);
        }
        // Release the lock
        msg_mutexes[other_prog].unlock();
    }

    type_t f = 0;
    type_t a = 0;
    type_t b = 0;
    type_t p = 0;
    do {
        // 23 set f 0
        // 24 set i 126
        f = 0;
        type_t i = 126;

        // 25 rcv a
        // Try to acquire the lock
        if(!fetch_queue_value(this_prog, other_prog, a)) {
            return;
        }
    
        while(i > 0) {
        // loop back point?
            // 26 rcv b
            if(!fetch_queue_value(this_prog, other_prog, b)) {
                return;
            }
            // 27 set p a
            // 28 mul p -1
            // 29 add p b
            p = a;
            p = -p;
            p += b;
            if(p == 0) {
                // 31 snd a
                send_value(this_prog, other_prog, a);
                // 32 set a b
                a = b;
            } else {
                send_value(this_prog, other_prog, b);
                // 35 set f 1
                f = 1;
            }
            // 36 add i -1
            i -= 1;
            // 37 jgz i -11 # JUMP by -11 to 26
        }
        // 38 snd a
        send_value(this_prog, other_prog, a);
        // 39 jgz f -16 
    } while (f != 0);
    if(a != 0) {
        do {
            // 21 rcv b
            if(!fetch_queue_value(this_prog, other_prog, b)) {
                return;
            }
            // 22 jgz b -1
        } while(b > 0);
        std::cerr << "Shouldn't get here I don't think.." << std::endl;
        throw;
    }
    return;
};

int main() {
    waiting[0].store(false);
    waiting[1].store(false);

    // Launch the threads
    std::thread prog0(program, 0);
    std::thread prog1(program, 1);

    // Join the threads
    prog0.join();
    prog1.join();

    std::cout << "Task 2: Program 1 sent" << num_vals_sent << " messages" << std::endl;

    return 0;
}
