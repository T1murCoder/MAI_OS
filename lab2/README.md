### Debug
`g++ -std=c++11 -pthread -g -O0 main.cpp src/thread_pool.cpp -Iinclude -o gauss_debug`
`gdb ./gauss_debug`
```
break main
break gauss
break normalize_row_task
break subtract_from_task
break ThreadPool:worker
```
`run < inputdata.txt`

`tui enable`

### Run Benchmark
`g++ -std=c++11 -pthread -O2 -Iinclude benchmark.cpp src/gaussian.cpp src/thread_pool.cpp -o benchmark`

---

## Отчёт

### Теоретический график закона Амдала (доля параллельного кода α=0.5)
![Amdahl Theory](assets/gauss_theory_amdahl.png)

### График бенчмарка для матрицы 10000 на 10000
![Benchmark 10k*10k](assets/benchmark_amdahl_10000.png)
Из графика видно, что на матрицах размера 10000 на 10000, оптимальное количество потоков - это 3. Далее, чем больше потоков, тем меньше ускорение и эффективность использования потоков.

На бОльших матрицах график будет больше приближен к теоретическому.

<details>
<summary>График времени исполнения от количества потоков</summary>
<img src="assets/benchmark_time_10000.png">
</details>


### График загруженности потоков в бенчмарке (2 прогона для каждого теста)
![Docker](assets/docker_thread_usage_10000.png)
Как мы видим, уже на 6 потоках график становится очень рваным - потоки конкурируют за задачи и выигрыш от многопоточности перекрывается накладными расходами на управление потоками

---

### Выводы
Многопоточность позволяет:
- повысить производительность процесса за счёт разделения алгоритма на независимые подзадачи
- упростить паралелльное взаимодействие за счёт использования общего адресного пространства (в отличии от взаимодействия процессов)

Но в тоже время несёт в себе проблемы синхронизации, а также отладки и поиска ошибок в программе.

<details>
<summary>⚠️Spoiler warning⚠️</summary>
<img src="assets/multi-threading.jpg">
</details>
