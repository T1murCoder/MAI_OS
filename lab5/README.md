# Лабораторная работа №5

## Strace

strace это утилита, которая отслеживает системные вызовы, совершаемые программой. С её помощью можно отлаживать использование ресурсов системы

## Какие системные вызовы использовались в каждой из лаб?

### Во всех

**openat(dirfd, pathname, flags, [mode])**

Открывает или создает файл. Аргументы:
- dirfd — файловый дескриптор директории (AT_FDCWD указывает на текущую директорию)
- pathname — путь к файлу
- flags — флаги открытия (O_RDONLY — только чтение, O_CLOEXEC — закрыть дескриптор при выполнении)
- mode — права доступа (опционально)
Возвращает файловый дескриптор

Пример: openat(AT_FDCWD, "/usr/local/lib64/libstdc++.so.6", O_RDONLY|O_CLOEXEC) = 3

**read(fd, buf, count)**

Читает данные из файла. Аргументы:
- fd — файловый дескриптор
- buf — буфер, в который читаются данные
- count — количество байт для чтения
Возвращает количество прочитанных байт

Пример: read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832

**write(fd, buf, count)**

Записывает данные в файл или поток. Аргументы: 
- fd — файловый дескриптор (1 — стандартный вывод)
- buf — буфер с данными для записи
- count — количество байт для записи

Возвращает количество записанных байт

Пример: write(1, "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \320\270\320\274\321\217 \321\204\320\260\320\271\320\273\320\260"..., 40) = 40

**close(fd)**

Закрывает файловый дескриптор. Аргументы: 
- fd — файловый дескриптор

Возвращает 0 при успехе

Пример: close(3) = 0

### Лаба №1

**pipe2(pipefd, flags)**

Создает канал (pipe) для обмена данными между процессами. Аргументы: 
- pipefd — массив из двух файловых дескрипторов (первый для чтения, второй для записи)
- flags — флаги (0 означает стандартное поведение)

Возвращает 0 при успехе

Пример: pipe2([3, 4], 0) = 0

**clone(flags, child_stack, ptid, tls, ctid)**

Создает новый процесс (дочерний). Аргументы:
- flags — флаги управления поведением (CLONE_CHILD_CLEARTID — очистить tid при выходе, CLONE_CHILD_SETTID — установить tid, SIGCHLD — отправить сигнал при завершении)
- child_stack — стек дочернего процесса (NULL для стандартного)
- ptid — указатель на parent_tid
- tls — адрес TLS (thread local storage)
- ctid — указатель на child_tidptr

Возвращает PID дочернего процесса

Пример: clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f0668f36750) = 20891

**execve(filename, argv, envp)**

Загружает новую программу в адресное пространство процесса. Аргументы: 
- filename — путь к исполняемому файлу
- argv — массив аргументов командной строки
- envp — массив переменных окружения

Не возвращает при успехе (процесс заменяется новой программой), возвращает ошибку при неудаче

Пример: execve("./child_exe", ["./child", "7"], 0x7fff0d3efc78 /* 29 vars */) = 0

**wait4(pid, wstatus, options, rusage)**

Родительский процесс ожидает завершения дочернего.  Аргументы:
- pid — PID ожидаемого процесса
- wstatus — указатель на переменную для статуса завершения (NULL — статус не требуется)
- options — опции ожидания (0 — стандартное ожидание)
- rusage — указатель на структуру для получения ресурсов (NULL — не требуется)

Возвращает PID завершившегося процесса

Пример: wait4(20892, NULL, 0, NULL) = 20892

### Лаба №2

**clone3(cl_args, size)**

Современная версия clone() для создания потоков. Аргументы: 
- cl_args — структура с параметрами (flags, child_tid, parent_tid, exit_signal, stack, stack_size, tls)
  - flags:  CLONE_VM (общее адресное пространство), CLONE_FS (общая файловая система), CLONE_FILES (общие дескрипторы), CLONE_SIGHAND (общие обработчики сигналов), CLONE_THREAD (создание потока), CLONE_SYSVSEM (общие SysV семафоры), CLONE_SETTLS (установка TLS), CLONE_PARENT_SETTID (установка parent_tid), CLONE_CHILD_CLEARTID (очистка child_tid)
- size — размер структуры

Возвращает TID потока или ошибку

Пример:  clone3({flags=CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_SETTLS|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID, child_tid=0x7f7489200990, parent_tid=0x7f7489200990, exit_signal=0, stack=0x7f7488a00000, stack_size=0x7fff80, tls=0x7f74892006c0}, 88) = -1 ENOSYS

**clone(flags, child_stack, ptid, tls, ctid)**

Создает новый поток (если используются флаги CLONE_VM и другие). Аргументы: 
- flags:  CLONE_VM, CLONE_FS, CLONE_FILES, CLONE_SIGHAND, CLONE_THREAD, CLONE_SYSVSEM, CLONE_SETTLS, CLONE_PARENT_SETTID, CLONE_CHILD_CLEARTID
- child_stack — адрес стека потока
- ptid — указатель на parent_tid
- tls — адрес TLS
- ctid — указатель на child_tidptr

Возвращает TID потока

Пример: clone(child_stack=0x7f74891fff70, flags=CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_SETTLS|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID, parent_tid=[4455], tls=0x7f74892006c0, child_tidptr=0x7f7489200990) = 4455

**futex(uaddr, futex_op, val, [timeout], [uaddr2], [val3])**

Примитив синхронизации для работы с мьютексами и переменными условия. Аргументы: 
- uaddr — адрес переменной futex
- futex_op — операция (FUTEX_WAKE_PRIVATE — пробудить поток в приватном futex)
- val — количество потоков для пробуждения

Возвращает количество пробуженных потоков

Пример: futex(0x7ffc546d3f68, FUTEX_WAKE_PRIVATE, 1) = 1

### Лаба №3

**openat(dirfd, pathname, flags, [mode])**

Открывает или создает файл в /dev/shm (памяти). Аргументы:
- dirfd — AT_FDCWD (текущая директория)
- pathname — путь к файлу
- flags:  O_RDWR (чтение и запись), O_CREAT (создать если не существует), O_NOFOLLOW (не следовать символическим ссылкам), O_CLOEXEC (закрыть при execve)
- mode — права доступа

Возвращает файловый дескриптор

Пример: openat(AT_FDCWD, "/dev/shm/parent_child_shm_1", O_RDWR|O_CREAT|O_NOFOLLOW|O_CLOEXEC, 0666) = 5

**ftruncate(fd, length)**

Устанавливает размер файла. Аргументы:
- fd — файловый дескриптор
- length — новый размер в байтах

Возвращает 0 при успехе

Пример: ftruncate(5, 1104) = 0

**mmap(addr, length, prot, flags, fd, offset)**

Отображает файл в виртуальное адресное пространство процесса. Аргументы:
- addr — рекомендуемый адрес (NULL — ядро выберет адрес)
- length — размер отображаемого региона в байтах
- prot — защита памяти (PROT_READ — чтение, PROT_WRITE — запись)
- flags:  MAP_SHARED (разделяемое между процессами)
- fd — файловый дескриптор
- offset — смещение в файле

Возвращает адрес отображенной памяти

Пример: mmap(NULL, 1104, PROT_READ|PROT_WRITE, MAP_SHARED, 5, 0) = 0x782996b66000

**clone(flags, child_stack, ptid, tls, ctid)**

Создает новый процесс (см. описание в лабе №1)

Возвращает PID дочернего процесса

Пример: clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x782996600750) = 37221

**execve(filename, argv, envp)**

Загружает новую программу.  Аргументы:
- filename — путь к исполняемому файлу
- argv — массив аргументов командной строки
- envp — массив переменных окружения

Не возвращает при успехе

Пример: execve("./child_exe", ["./child", "3", "parent_child_shm_1"], 0x7ffe92efcdd8 /* 31 vars */) = 0

**pread64(fd, buf, count, offset)**

Позиционированное чтение из файла (не изменяет текущую позицию). Аргументы:
- fd — файловый дескриптор
- buf — буфер для данных
- count — количество байт для чтения
- offset — смещение в файле

Возвращает количество прочитанных байт

Пример: pread64(5, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784

**munmap(addr, length)**

Удаляет отображение памяти из адресного пространства. Аргументы:
- addr — адрес начала региона
- length — размер региона в байтах

Возвращает 0 при успехе

Пример: munmap(0x782996b66000, 1104) = 0

**unlink(pathname)**

Удаляет файл.  Аргументы:
- pathname — путь к файлу

Возвращает 0 при успехе

Пример: unlink("/dev/shm/parent_child_shm_1") = 0

### Лаба №4

**openat(dirfd, pathname, flags, [mode])**

Открывает динамическую библиотеку. Аргументы:
- dirfd — AT_FDCWD
- pathname — путь к файлу библиотеки
- flags:  O_RDONLY (только чтение), O_CLOEXEC

Возвращает файловый дескриптор

Пример: openat(AT_FDCWD, "./my_lib_1. so", O_RDONLY|O_CLOEXEC) = 3

**read(fd, buf, count)**

Читает заголовок ELF-файла. Аргументы:
- fd — файловый дескриптор
- buf — буфер
- count — количество байт для чтения

Возвращает количество прочитанных байт

Пример: read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832

**mmap(addr, length, prot, flags, fd, offset)** (первый вызов)

Отображает весь файл библиотеки.  Аргументы:
- addr — NULL
- length — размер файла в байтах
- prot:  PROT_READ (только чтение)
- flags:  MAP_PRIVATE (приватная копия), MAP_DENYWRITE (запретить запись в исходный файл)
- fd — файловый дескриптор
- offset — смещение в файле

Возвращает адрес отображенной памяти

Пример: mmap(NULL, 16408, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7578b3704000

**mmap(addr, length, prot, flags, fd, offset)** (второй вызов)

Отображает секцию кода. Аргументы: 
- addr — фиксированный адрес
- length — размер секции в байтах
- prot:  PROT_READ, PROT_EXEC (выполнение кода)
- flags: MAP_PRIVATE, MAP_FIXED (использовать указанный адрес), MAP_DENYWRITE
- fd — файловый дескриптор
- offset — смещение в файле

Возвращает адрес отображенной памяти

Пример: mmap(0x7578b3705000, 4096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1000) = 0x7578b3705000

**mmap(addr, length, prot, flags, fd, offset)** (третий вызов)

Отображает секцию данных только для чтения. Аргументы:
- addr — фиксированный адрес
- length — размер секции в байтах
- prot:  PROT_READ
- flags: MAP_PRIVATE, MAP_FIXED, MAP_DENYWRITE
- fd — файловый дескриптор
- offset — смещение в файле

Возвращает адрес отображенной памяти

Пример: mmap(0x7578b3706000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0x7578b3706000

**mmap(addr, length, prot, flags, fd, offset)** (четвертый вызов)

Отображает секцию инициализированных данных с правом записи. Аргументы: 
- addr — фиксированный адрес
- length — размер секции в байтах
- prot: PROT_READ, PROT_WRITE (чтение и запись)
- flags: MAP_PRIVATE, MAP_FIXED, MAP_DENYWRITE
- fd — файловый дескриптор
- offset — смещение в файле

Возвращает адрес отображенной памяти

Пример: mmap(0x7578b3707000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0x7578b3707000

**close(fd)**

Закрывает дескриптор. Аргументы:
- fd — файловый дескриптор

Возвращает 0 при успехе

Пример: close(3) = 0

**munmap(addr, length)**

Удаляет отображение памяти из адресного пространства. Аргументы:
- addr — адрес начала региона
- length — размер региона в байтах

Возвращает 0 при успехе

Пример: munmap(0x7578b3704000, 16408) = 0

## Выводы

strace полезный инструмент для отладки программы. Просто дебаггером не всегда можно продебажить, когда работаешь с системными вызовами, как например, в многопоточке или при использовании shared memory. А в strace сразу видно, например, те же самые файловые дескрипторы и в каких системных вызовах они используются.