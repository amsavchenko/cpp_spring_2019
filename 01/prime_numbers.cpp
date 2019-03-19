#include <iostream>
#include <vector>
#include <algorithm>
#include "numbers.dat"

int binarySearch(const int Data[], int num_to_search, int first, int last, bool first_entry)
{
    /*
    бинарный поиск по Data, возвращает:
    индекс первого вхождения, если first_entry == true
    индекс последнего вхождения, если first_entry == false
    */
    
    if (first >= last)
        return -1;
    int mid = first + (last - first) * 0.5;
    int mid_element = Data[mid];
    if (mid_element == num_to_search)
    {
        if (first_entry)
        {
            while (mid_element == num_to_search)
                mid_element = Data[--mid];
            return mid + 1;
        }
        else
        {
            while (mid_element == num_to_search)
                mid_element = Data[++mid];
            return mid - 1;
        }
    }
    else if (mid_element > num_to_search)
        return binarySearch(Data, num_to_search, first, mid, first_entry);
    else return binarySearch(Data, num_to_search, mid+1, last, first_entry);
}



std::vector<bool> sieveErato (int max_element)
{
    /*
    решето Эратосфена заполняется до max_element
    true - число простое, false - составное
    */
    
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
    
    std::vector<int> borders (args - 1);
    
    for (int i = 0; i < args - 1; i++)
        borders[i] = std::atoi(argv[i + 1]);
    
    // определение max аргумента командной строки, чтобы построить решето до этого значения
    int max_in_pairs = *std::max_element(std::begin(borders), std::end(borders));
    std::vector<bool> sieve = sieveErato(max_in_pairs);
    
    for (int pair = 0; pair < args - 1; pair += 2)
    {
        int counter = 0;
        
        int left_border = binarySearch(Data, borders[pair], 0, Size, true);
        int right_border = binarySearch(Data, borders[pair + 1], 0, Size, false);
        
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

