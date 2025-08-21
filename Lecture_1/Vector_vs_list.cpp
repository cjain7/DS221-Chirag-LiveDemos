#include <chrono>
#include <iostream>
#include <list>
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
    (5) Modern CPUs use write-back caches, the newly appended number is written into cache only (not RAM). 
        After writing into cache, the cache line is marked dirty. 
        The data is written back to RAM later, when the cache line is evicted. 
        Therefore, when we use array (vector), writes to RAM happen in batches after a cache line eviction.
    (6) In linked list, each new node leads to a cache miss due to poor memory locality
    */
}