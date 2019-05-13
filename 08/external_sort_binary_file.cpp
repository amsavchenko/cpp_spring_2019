#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <thread>

std::ifstream::pos_type filesize(std::string filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

template<class T>
void generateBinFile(const std::string& path, size_t size)
{
    srand(static_cast<unsigned int>(time(0)));
    std::ofstream out(path, std::ios::binary);
    for (size_t i = 0; i < size / sizeof(T); i++)
    {
        T x = rand() % 100000;
        out.write((char *) &x, sizeof(x));
    }
    out.close();
}

template<class T>
void printBinFile(const std::string& path)
{
    std::ifstream in(path, std::ios::binary);
    for (size_t i = 0; i < filesize(path) / sizeof(T); i++)
    {
        T x;
        in.read((char *) &x, sizeof(x));
        std::cout << x << std::endl;
    }
    in.close();
}

template<class T>
void splitBinFileTo2Files(const std::string& old_path, const std::string& path1, const std::string& path2)
{
    std::ifstream old_file(old_path, std::ios::binary);
    std::ofstream new_file(path1, std::ios::binary);
    for (size_t i = 0; i < filesize(old_path) / (2 * sizeof(T)); i++)
    {
        T x;
        old_file.read((char *) &x, sizeof(x));
        new_file.write((char *) &x, sizeof(x));
    }
    new_file.close();
    
    std::ofstream new_file2(path2, std::ios::binary);
    for (size_t i = 0; i < filesize(old_path) / (2 * sizeof(T)); i++)
    {
        T x;
        old_file.read((char *) &x, sizeof(x));
        new_file2.write((char *) &x, sizeof(x));
    }
    new_file2.close();
    old_file.close();
}

template<class T>
void mergeHalfs(std::vector<T> &chunk, size_t s, size_t q, size_t r)
{
    std::vector<T> *chunk2 = new std::vector<T>(q - s + 1);
    auto it2 = chunk2->begin(),
    it1 = chunk.begin() + q + 1,
    it = chunk.begin() + s;
    
    std::copy(it, it1, it2);
    while (it2 != chunk2->end() && it1 != chunk.begin() + r + 1)
    {
        if (*it1 > *it2)
        {
            *it = *it2;
            ++it2;
        } else
        {
            *it = *it1;
            ++it1;
        }
        ++it;
    }
    if (it1 == chunk.begin() + r + 1)
        std::copy(it2, chunk2->end(), it);
    else
        std::copy(it1, chunk.begin() + r + 1, it);
    delete chunk2;
}

template<class T>
const size_t fillZeroes(const std::string& filename, size_t memory)
{
    std::fstream data(filename, std::ios::in | std::ios::out | std::ios::binary);
    const uint64_t file_size = filesize(filename);
    const size_t type_size = sizeof(T);
    const size_t chunk_size = memory / type_size;
    const size_t size = (file_size % memory) / type_size;
    const size_t zeroes = size == 0 ? 0 : chunk_size - size;;
    auto chunk = new std::vector<T>(zeroes);
    const size_t chunk_byte_size = chunk_size * type_size;
    const uint64_t chunk_number = file_size / memory;
    
    data.seekp(chunk_byte_size * chunk_number + size * type_size);
    data.write((char*) chunk->data(), type_size * zeroes);
    data.close();
    
    return zeroes;
}

template<class T>
void externalSort(const std::string& filename, size_t memory)
{
    const size_t type_size = sizeof(T);
    const uint64_t file_size = filesize(filename);
    
    std::fstream data(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    const uint64_t chunk_number = ceilf(static_cast<float>(file_size) / static_cast<float>(memory));
    const size_t chunk_size = memory / type_size - (memory / type_size) % 2,
    chunk_byte_size = chunk_size * type_size,
    half_chunk_byte_size = chunk_byte_size / 2,
    half_chunk_size = chunk_size / 2;
    
    std::vector<T> *chunk = new std::vector<T>(chunk_size);
    
    for (uint64_t i = 0; i < chunk_number; ++i)
    {
        data.seekg(chunk_byte_size * i);
        data.read((char *) chunk->data(), chunk_byte_size);
        
        std::sort(chunk->begin(), chunk->end());
        
        data.seekp(chunk_byte_size * i);
        data.write((char *) chunk->data(), chunk_byte_size);
    }
    
    int64_t s = chunk_number, start = 0;
    while (s > 0)
    {
        
        // проход вправо
        data.seekp(half_chunk_byte_size * start);
        data.read((char *) chunk->data(), half_chunk_byte_size);
        
        for (int64_t q = 1; q < s; ++q)
        {
            data.seekg(half_chunk_byte_size * start + chunk_byte_size * q);
            data.read((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
            
            mergeHalfs<T>(*chunk, 0, half_chunk_size - 1, chunk_size - 1);
            
            data.seekp(half_chunk_byte_size * start + chunk_byte_size * q);
            data.write((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
        }
        
        data.seekp(half_chunk_byte_size * start);
        data.write((char *) chunk->data(), half_chunk_byte_size);
        
        // проход влево
        data.seekp(half_chunk_byte_size * start + chunk_byte_size * s - half_chunk_byte_size);
        data.read((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
        for (int64_t q = s - 2; q >= 0; --q)
        {
            data.seekg(half_chunk_byte_size * (start + 1) + chunk_byte_size * q);
            data.read((char *) chunk->data(), half_chunk_byte_size);
            
            mergeHalfs(*chunk, 0, half_chunk_size - 1, chunk_size - 1);
            
            data.seekp(half_chunk_byte_size * (start + 1) + chunk_byte_size * q);
            data.write((char *) chunk->data(), half_chunk_byte_size);
        }
        
        data.seekg(half_chunk_byte_size * start + chunk_byte_size * s - half_chunk_byte_size);
        data.write((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
        
        --s;
        ++start;
        
        // перераспределение блоков
        for (int64_t p = 0; p < s; ++p)
        {
            data.seekp(half_chunk_byte_size * start + chunk_byte_size * p);
            data.read((char *) chunk->data(), chunk_byte_size);
            
            mergeHalfs(*chunk, 0, half_chunk_size - 1, chunk_size - 1);
            
            data.seekg(half_chunk_byte_size * start + chunk_byte_size * p);
            data.write((char *) chunk->data(), chunk_byte_size);
        }
    }
    
    delete chunk;
}

template <class T>
void parallelSort(const std::string& name1, const std::string& name2, size_t memory)
{
    std::thread t1(externalSort<T>, name1, memory / 2);
    std::thread t2(externalSort<T>, name2, memory / 2);
    t1.join();
    t2.join();
}

template <class T>
void merge2Files(const std::string& filename1, const std::string& filename2,
                const std::string& filename_final, const size_t memory)
{
    const size_t type_size = sizeof(uint64_t);
    const uint64_t file_size1 = filesize(filename1);
    const uint64_t file_size2 = filesize(filename2);
    
    std::ifstream data1(filename1, std::ios::binary);
    std::ifstream data2(filename2, std::ios::binary);
    std::ofstream data3(filename_final, std::ios::binary);
    
    const uint64_t chunk_number1 = ceilf(static_cast<float>(file_size1) / static_cast<float>(memory));
    const uint64_t chunk_number2 = ceilf(static_cast<float>(file_size2) / static_cast<float>(memory));
    const size_t chunk_size = memory / type_size - (memory / type_size) % 2,
    chunk_byte_size = chunk_size * type_size;
    
    std::vector<uint64_t>* chunk = new std::vector<uint64_t>(2 * chunk_size);
    uint64_t i = 0, k = 0;
    data1.seekg(chunk_byte_size * i);
    data1.read((char*) chunk->data(), chunk_byte_size);
    
    data2.seekg(chunk_byte_size * k);
    data2.read((char*) (chunk->data() + chunk_size), chunk_byte_size);
    
    auto it1 = chunk->begin(),
    it2 = chunk->begin() + chunk_size;
    
    while (i < chunk_number1 && k < chunk_number2)
    {
        while (it1 != (chunk->begin() + chunk_size) && it2 != (chunk->begin() + chunk_size * 2))
        {
            if (*it1 > *it2)
            {
                data3.write((char*) &(*it2), sizeof(uint64_t));
                ++it2;
            }
            else
            {
                data3.write((char*) &(*it1), sizeof(uint64_t));
                ++it1;
            }
        }
        if (it1 == (chunk->begin() + chunk_size))
        {
            ++i;
            if (i < chunk_number1)
            {
                data1.seekg(chunk_byte_size * i);
                data1.read((char*) chunk->data(), chunk_byte_size);
                it1 = chunk->begin();
            }
        }
        else
        {
            ++k;
            if (k < chunk_number2)
            {
                data2.seekg(chunk_byte_size * k);
                data2.read((char*) (chunk->data() + chunk_size), chunk_byte_size);
                it2 = chunk->begin() + chunk_size;
            }
        }
    }
    if (i == chunk_number1)
    {
        while (it2 != (chunk->begin() + chunk_size * 2))
        {
            data3.write((char*) &(*it2), sizeof(uint64_t));
            ++it2;
        }
        ++k;
        
        for (uint64_t j = k; j < chunk_number2; j++)
        {
            data2.seekg(chunk_byte_size * j);
            data2.read((char*) (chunk->data() + chunk_size), chunk_byte_size);
            data3.write((char*) (chunk->data() + chunk_size), chunk_byte_size);
        }
    }
    else
    {
        while (it1 != chunk->begin() + chunk_size)
        {
            data3.write((char*) &(*it1), sizeof(uint64_t));
            ++it1;
        }
        ++i;
        for (uint64_t j = i; j < chunk_number1; j++)
        {
            data2.seekg(chunk_byte_size * j);
            data2.read((char*) (chunk->data() + chunk_size), chunk_byte_size);
            data3.write((char*) (chunk->data() + chunk_size), chunk_byte_size);
        }
    }
    data1.close();
    data2.close();
    data3.close();
    delete chunk;
}

template<class T>
std::string deleteZeroes(const std::string& filename, const size_t zeroes)
{
    if (zeroes == 0)
        return filename;
    const std::string new_name = "./new.txt";
    std::ifstream old(filename, std::ios::binary);
    std::ofstream new_file(new_name, std::ios::binary);
    old.seekg(zeroes * sizeof(T));
    for (size_t i = 0; i < (filesize(filename) / sizeof(T)) - zeroes; i ++)
    {
        T x;
        old.read((char *) &x, sizeof(x));
        new_file.write((char *) &x, sizeof(x));
    }
    old.close();
    new_file.close();
    return new_name;
}

template <class T>
std::string externalSortBinFile (unsigned long memory, unsigned long chunk_size)
{
    std::string file_name = "./generated_file.txt";
    std::string first_half = "./first_half.txt";
    std::string second_half = "./second_half.txt";
    std::string result = "./result.txt";
    
    generateBinFile<T>(file_name, memory); // создание бинарного файла размера memory
    splitBinFileTo2Files<T>(file_name, first_half, second_half); // разделение бин файла на 2 части
    remove(file_name.c_str());
    const size_t zeroes = fillZeroes<T>(second_half, chunk_size); //если последний из блоков во 2ом файле
                                                                //заполнен не до конца, заполняем его нулями
    parallelSort<T>(first_half, second_half, chunk_size); // параллельная внешняя сортировка 2ух файлов
    merge2Files<T>(first_half, second_half, result, chunk_size); // слияние 2ух файлов в 1 отсортированный
    remove(first_half.c_str());
    remove(second_half.c_str());

    return deleteZeroes<T>(result, zeroes); // если надо - удаление нулей
}

int main()
{
    unsigned long memory = 16777216; // 16 MB
    unsigned long chunk_size = 4194304; // 4 MB
    
    auto new_name = externalSortBinFile<uint64_t>(memory, chunk_size);
    printBinFile<uint64_t>(new_name);

    return 0;
}