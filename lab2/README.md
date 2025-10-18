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

### Вывод



![Multi-threading](assets/multi-threading.jpg)