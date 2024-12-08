#include <iostream>    // Для вывода на экран (std::cout, std::cerr)
#include <vector>      // Для работы с векторами (std::vector)
#include <random>      // Для генерации случайных чисел (std::random_device, std::mt19937, std::uniform_int_distribution)
#include <chrono>      // Для измерения времени (std::chrono::high_resolution_clock)
#include <iomanip>     // Для форматированного вывода
#include <fstream>     // Для работы с файлами (std::ofstream)
#include <omp.h>       // Для использования OpenMP (параллельные вычисления)

// Функция для генерации случайной матрицы
// size - размер матрицы (количество строк и столбцов),
// minVal и maxVal - минимальное и максимальное значение для генерации случайных чисел.
std::vector<std::vector<int>> generateMatrix(int size, int minVal, int maxVal) {
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));  // Создаем двумерный вектор (матрицу)

    // Инициализация генератора случайных чисел
    std::random_device rd;        // Используется для генерации случайных чисел
    std::mt19937 gen(rd());       // Мерсенн Твистер - генератор случайных чисел
    std::uniform_int_distribution<> dis(minVal, maxVal);  // Определяем диапазон случайных чисел

    // Заполнение матрицы случайными числами
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = dis(gen);  // Записываем случайное число в ячейку матрицы
        }
    }

    return matrix;  // Возвращаем сгенерированную матрицу
}

// Функция для последовательного умножения матриц
// Принимает две матрицы A и B, возвращает результат их умножения
std::vector<std::vector<int>> multiplySequential(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B) {
    int size = A.size();  // Получаем размер матриц (предполагаем, что они квадратные)
    std::vector<std::vector<int>> result(size, std::vector<int>(size, 0));  // Инициализируем результат (матрица всех нулей)

    // Три вложенных цикла для умножения матриц
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                result[i][j] += A[i][k] * B[k][j];  // Стандартная формула для умножения матриц
            }
        }
    }

    return result;  // Возвращаем результат умножения
}

// Функция для параллельного умножения матриц с использованием OpenMP
std::vector<std::vector<int>> multiplyParallel(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B) {
    int size = A.size();  // Получаем размер матриц
    std::vector<std::vector<int>> result(size, std::vector<int>(size, 0));  // Инициализируем результат

    // Директива OpenMP для распараллеливания внешних двух циклов
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                #pragma omp atomic  // Обеспечиваем атомарность операции (чтобы избежать гонок данных)
                result[i][j] += A[i][k] * B[k][j];  // Умножаем и записываем результат
            }
        }
    }

    return result;  // Возвращаем результат параллельного умножения
}

// Функция для сравнения двух матриц на равенство
bool compareMatrices(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B) {
    // Проходим по всем элементам матриц и сравниваем их
    for (size_t i = 0; i < A.size(); ++i) {
        for (size_t j = 0; j < A[i].size(); ++j) {
            if (A[i][j] != B[i][j]) {  // Если элементы не равны, возвращаем false
                return false;
            }
        }
    }
    return true;  // Если все элементы равны, возвращаем true
}

// Функция для записи данных в файл в формате CSV
// filename - имя файла для записи, results - вектор с результатами
void writeToExcel(const std::string& filename, const std::vector<std::tuple<int, double, double>>& results) {
    std::ofstream file(filename);  // Открываем файл для записи

    if (!file.is_open()) {  // Если не удалось открыть файл
        std::cerr << "Ошибка: не удалось открыть файл для записи." << std::endl;
        return;  // Выход из функции
    }

    // Записываем заголовок в файл
    file << "Размер,Последовательное,Параллельное\n";

    // Записываем результаты (размер матрицы и времена для последовательного и параллельного умножения)
    for (const auto& [size, seqTime, parTime] : results) {
        file << size << "," << seqTime << "," << parTime << "\n";  // Форматируем данные для CSV
    }

    std::cout << "Данные успешно сохранены в файл: " << filename << std::endl;
}

// Основная функция
int main() {
    std::vector<std::tuple<int, double, double>> results;  // Вектор для хранения результатов (размер, время последовательного, время параллельного)

    std::cout << "Размер\tПоследовательное(с)\tПараллельное(с)" << std::endl;  // Заголовок для вывода

    // Цикл для тестирования различных размеров матриц
    for (int size = 100; size <= 800; size += 100) {
        double sequentialTime = 0, parallelTime = 0;  // Переменные для накопления времени

        // Запуск 10 тестов для каждого размера матрицы
        for (int t = 0; t < 10; ++t) {
            auto A = generateMatrix(size, -10000000, 10000000);  // Генерация случайных матриц A и B
            auto B = generateMatrix(size, -10000000, 10000000);

            // Замер времени для последовательного умножения
            auto start = std::chrono::high_resolution_clock::now();
            auto seqResult = multiplySequential(A, B);  // Умножение матриц
            auto end = std::chrono::high_resolution_clock::now();
            sequentialTime += std::chrono::duration<double>(end - start).count();  // Накопление времени

            // Замер времени для параллельного умножения
            start = std::chrono::high_resolution_clock::now();
            auto parResult = multiplyParallel(A, B);  // Параллельное умножение матриц
            end = std::chrono::high_resolution_clock::now();
            parallelTime += std::chrono::duration<double>(end - start).count();  // Накопление времени

            // Проверка, совпадают ли результаты умножения
            if (!compareMatrices(seqResult, parResult)) {
                std::cerr << "Ошибка: результаты матриц не совпадают!" << std::endl;
                return -1;  // Выход с ошибкой, если результаты не совпадают
            }
        }

        // Вычисление среднего времени для каждого типа умножения
        double avgSequential = sequentialTime / 10.0;
        double avgParallel = parallelTime / 10.0;

        // Вывод результатов для текущего размера матрицы
        std::cout << size << "\t"
                  << avgSequential << "\t"
                  << avgParallel << std::endl;

        // Сохраняем результаты для последующей записи в файл
        results.emplace_back(size, avgSequential, avgParallel);
    }

    // Записываем результаты в файл
    writeToExcel("результаты.csv", results);

    return 0;  // Возвращаем 0, что означает успешное завершение программы
}