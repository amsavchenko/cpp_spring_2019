#include <iostream>
#include <vector>
#include <algorithm>
#include "numbers.dat"



int binarySearchInData(const int Data[], int num_to_search, int first, int last)
{
    if (first >= last)
        return -1;
    int mid = first + (last - first) * 0.5;
    int mid_element = Data[mid];
    if (Data[first] == num_to_search)
        return first;
    if (mid_element == num_to_search)
    {
        if (mid == first+1)
            return mid;
        else
            return binarySearchInData(Data, num_to_search, first, mid+1);
    }
    else if (mid_element > num_to_search)
        return binarySearchInData(Data, num_to_search, first, mid);
    else return binarySearchInData(Data, num_to_search, mid+1, last);
}

int binarySearchInDataLast(const int Data[], int num_to_search, int first, int last)
{
    if (first >= last)
        return -1;
    int mid = first + (last - first) * 0.5;
    int mid_element = Data[mid];
    if (Data[last-1] == num_to_search)
        return last-1;
    if (mid_element == num_to_search)
    {
        if (mid == last-2)
            return mid;
        else
            return binarySearchInDataLast(Data, num_to_search, mid, last-1);
    }
    else if (mid_element > num_to_search)
        return binarySearchInDataLast(Data, num_to_search, first, mid);
    else return binarySearchInDataLast(Data, num_to_search, mid+1, last);
}

std::vector<bool> sieveErato (int max_element)
{
    std::vector<bool> sieve (max_element, true);
    sieve[0] = sieve[1] = false;
    for(long long i = 2; i <= max_element; i++)
    {
        if(sieve[i])
        {
            for(long long j = i*i; j <= max_element; j += i)
                sieve[j] = false;
        }
    }
    return sieve;
}


int main(int args, char* argv[])
{
    if (args == 1 || args % 2 == 0)
        return -1;
    
    std::vector<int> borders;
    
    for (int i = 1; i < args; ++i)
    {
        int v = std::stoi(argv[i]);
        borders.push_back(v);
    }
    
    int max_in_pairs = *std::max_element(std::begin(borders), std::end(borders));
    std::vector<bool> sieve = sieveErato(max_in_pairs);
    
    for (int pair = 0; pair < args - 1; pair += 2)
    {
        int counter = 0;
        
        int left_border = binarySearchInData(Data, borders[pair], 0, Size);
        int right_border = binarySearchInDataLast(Data, borders[pair + 1], 0, Size);
        
        if (left_border == -1 || right_border == -1)
            return -1;
        
        if (left_border > right_border)
        {
            std::cout << 0 << std::endl;
            return 0;
        }
        
        for (int i = left_border; i <= right_border; i++)
        {
            if (sieve[Data[i]])
                counter++;
        }
        std::cout << counter << std::endl;
    }
    
    return 0;
}

