#include <chrono>
#include <iostream>
#include <list>
#include <vector>
using namespace std;

int main() {
    int N = 100000000; 

    // Two ways to store the list of numbers {0,1,2,...,100000000}: Vector (array) and Linked list
    vector<int> v;  
    list<int> s;

    // Let us compare the performance of vector and list for appending N numbers
    // Start the timer
    auto start = chrono::high_resolution_clock::now();
    
    // Insert all numbers into vector v, one by one
    for (int i = 0; i <= N; i++) {
        v.push_back(i);
    }

    // Stop the timer and print the time
    auto end = chrono::high_resolution_clock::now();
    cout << "Vector insertions took "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " milliseconds\n";

    // Restart the timer
    start = chrono::high_resolution_clock::now();
    
    // Insert all numbers at the end of list s, one by one
    for (int i = 0; i <= N; i++) {
        s.insert(s.end(), i);
    }

    // Stop the timer and print the time
    end = chrono::high_resolution_clock::now();
    cout << "List insertions took "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " milliseconds\n";

    return 0;

    /*What did we learn by running this code?
    (1) Theoretically, both vector and linked list require amortized O(1) time per append of each number
    (2) In practice, appending is faster in array (vector) than linked list by an order of magnitude
    (3) Memory reallocation in vector is rare (logarithmic number of times due to the size doubling method)
    (4) In linked list, memory allocation is done once for every new node insertion 
    (5) Modern CPUs typically use write-back caches. When a vector element is written, the corresponding cache line is modified in the cache and marked dirty rather than immediately being written to DRAM. 
    
    Since vector elements are contiguous, multiple appended elements can modify the same cache line. Dirty cache lines are eventually propagated to lower levels of the memory hierarchy and, when necessary, to DRAM. Thus, vector writes can be efficiently grouped at the cache-line level.
    */
}
