import matplotlib.pyplot as plt
import pandas as pd

file_path = '/home/spectreofoblivion/CLionProjects/untitled1/cmake-build-debug/результаты.csv'
data = pd.read_csv(file_path)


# График 1: Время выполнения (Последовательное vs Параллельное)
plt.figure(figsize=(10, 6))
plt.plot(data['Размер'], data['Последовательное'], label='Последовательное', color='red', marker='o')
plt.plot(data['Размер'], data['Параллельное'], label='Параллельное', color='blue', marker='o')
plt.xlabel('Размер матрицы')
plt.ylabel('Время выполнения (сек.)')
plt.title('Сравнение времени выполнения: Последовательное vs Параллельное')
plt.legend()
plt.grid(True)
plt.savefig('execution_time_comparison.png')
plt.show()

# График 2: Ускорение параллельного алгоритма
data['Ускорение'] = data['Последовательное'] / data['Параллельное']

plt.figure(figsize=(10, 6))
plt.plot(data['Размер'], data['Ускорение'], label='Ускорение', color='green', marker='o')
plt.xlabel('Размер матрицы')
plt.ylabel('Ускорение')
plt.title('Ускорение параллельного алгоритма')
plt.grid(True)
plt.savefig('speedup_comparison.png')
plt.show()
