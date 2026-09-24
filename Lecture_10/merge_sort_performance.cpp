#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

// ---------------- 2-Way Merge Sort ----------------
void merge2(vector<int>& arr, vector<int>& temp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= mid)
        temp[k++] = arr[i++];
    while (j <= right)
        temp[k++] = arr[j++];

    for (int i = left; i <= right; ++i)
        arr[i] = temp[i];
}

void mergeSort2(vector<int>& arr, int left, int right, vector<int>& temp) {
    if(left < right) {
        int mid = left + (right - left)/2;
        mergeSort2(arr, left, mid, temp);
        mergeSort2(arr, mid+1, right, temp);
        merge2(arr, temp, left, mid, right); 
    }
}

// ---------------- 10-Way Merge Sort ----------------
void merge10(vector<int>& arr, vector<int>& temp, int left, int right, vector<int>& indices) {
    int n = right - left + 1;
    int k = left;
    vector<int> pos(10, 0);
    for(int i = 0; i < 10; ++i) pos[i] = indices[i];

    while(true) {
        int minVal = INT_MAX, minIdx = -1;
        for(int i = 0; i < 10; ++i) { //find the minimum element
            if(pos[i] <= indices[i+1]-1 && pos[i] <= right) {
                if(arr[pos[i]] < minVal) {
                    minVal = arr[pos[i]];
                    minIdx = i;
                }
            }
        }
        if(minIdx == -1) break;
        temp[k++] = minVal;  //put the minimum element in sorted array
        pos[minIdx]++; //advance the pointer
    }
    for(int i = left; i <= right; ++i) arr[i] = temp[i];
}

void mergeSort10(vector<int>& arr, int left, int right, vector<int>& temp) {

    //base case
    if(right - left < 10) {
        sort(arr.begin() + left, arr.begin() + right + 1);
        return;
    }

    //split into 10 subarrays
    int n = right - left + 1;
    vector<int> indices(11);
    for(int i = 0; i <= 10; ++i) 
        indices[i] = left + i * (n/10);
    indices[10] = right + 1;

    for(int i = 0; i < 10; ++i)
        mergeSort10(arr, indices[i], indices[i+1]-1, temp);

    merge10(arr, temp, left, right, indices);
}

// ---------------- 100-Way Merge Sort ----------------
void merge100(vector<int>& arr, vector<int>& temp, int left, int right, vector<int>& indices) {
    int n = right - left + 1;
    int k = left;
    vector<int> pos(100, 0);
    for(int i = 0; i < 100; ++i) pos[i] = indices[i];

    while(true) {
        int minVal = INT_MAX, minIdx = -1;
        for(int i = 0; i < 100; ++i) { //find the minimum element
            if(pos[i] <= indices[i+1]-1 && pos[i] <= right) {
                if(arr[pos[i]] < minVal) {
                    minVal = arr[pos[i]];
                    minIdx = i;
                }
            }
        }
        if(minIdx == -1) break;
        temp[k++] = minVal;  //put the minimum element in sorted array
        pos[minIdx]++; //advance the pointer
    }
    for(int i = left; i <= right; ++i) arr[i] = temp[i];
}

void mergeSort100(vector<int>& arr, int left, int right, vector<int>& temp) {

    //base case
    if(right - left < 100) {
        sort(arr.begin() + left, arr.begin() + right + 1);
        return;
    }


    //split into 100 subarrays
    int n = right - left + 1;
    vector<int> indices(101);
    for(int i = 0; i <= 100; ++i) 
        indices[i] = left + i * (n/100);
    indices[100] = right + 1;

    for(int i = 0; i < 100; ++i)
        mergeSort100(arr, indices[i], indices[i+1]-1, temp);

    merge100(arr, temp, left, right, indices);
}

// ---------------- Main Function ----------------
int main() {
    int n = 10000000;  // Array size
    vector<int> arr(n);
    for(int i = 0; i < n; ++i) arr[i] = rand();

    // --- 2-Way Merge Sort ---
    vector<int> arr2 = arr;
    vector<int> temp(n);
    auto start = high_resolution_clock::now();
    mergeSort2(arr2, 0, n-1, temp);
    auto end = high_resolution_clock::now();
    cout << "2-way merge sort time: " 
         << duration_cast<milliseconds>(end-start).count() << " ms\n";

    // --- 10-Way Merge Sort ---
    vector<int> arr10 = arr;
    vector<int> temp2(n);
    start = high_resolution_clock::now();
    mergeSort10(arr10, 0, n-1, temp2);
    end = high_resolution_clock::now();
    cout << "10-way merge sort time: " 
         << duration_cast<milliseconds>(end-start).count() << " ms\n";

    // --- 100-Way Merge Sort ---
    vector<int> arr100 = arr;
    vector<int> temp3(n);
    start = high_resolution_clock::now();
    mergeSort100(arr100, 0, n-1, temp3);
    end = high_resolution_clock::now();
    cout << "100-way merge sort time: " 
         << duration_cast<milliseconds>(end-start).count() << " ms\n";

    return 0;
}

