import matplotlib.pyplot as plt
import matplotlib
matplotlib.rcParams['font.size'] = 11

# Читаем данные из файла
threads = []
times = []

try:
    with open('data/benchmark_results2500.txt', 'r') as f:
        for line in f:
            if line.startswith('#'):
                continue
            parts = line.strip().split()
            if len(parts) == 2:
                threads.append(int(parts[0]))
                times.append(float(parts[1]))
except FileNotFoundError:
    print("❌ Файл benchmark_results.txt не найден!")
    print("Сначала запусти: ./benchmark")
    exit(1)

if len(threads) == 0:
    print("❌ Нет данных в файле!")
    exit(1)

# Вычисляем ускорение
t1 = times[0]  # Время с 1 потоком
speedup = [t1 / t for t in times]

print(f"\n{'Потоки':<10} {'Время (сек)':<15} {'Ускорение':<12} {'Эффективность'}")
print("-" * 70)
for i in range(len(threads)):
    eff = (speedup[i] / threads[i]) * 100
    print(f"{threads[i]:<10} {times[i]:<15.4f} {speedup[i]:<12.2f} {eff:.1f}%")

# Теоретическая параллельная доля
f_theoretical = 0.5

# Теоретическая кривая Амдала с α = 0.5
threads_theory = [i/10.0 for i in range(10, max(threads)*10 + 10)]
speedup_theory = [1 / ((1 - f_theoretical) + f_theoretical / p) for p in threads_theory]

# ============ ГРАФИК 1: УСКОРЕНИЕ ============
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

# Теоретическая кривая Амдала
ax1.plot(threads_theory, speedup_theory, 'r-', 
         label=f'Закон Амдала (α={f_theoretical:.1f})', linewidth=2.5, zorder=2, alpha=0.7)

# РЕАЛЬНЫЕ данные
ax1.plot(threads, speedup, 'bo-', label='Реальные измерения', 
         markersize=12, linewidth=2.5, zorder=3)

# Идеальное ускорение
ax1.plot(threads_theory, threads_theory, 'g--', 
         label='Идеальное (линейное)', alpha=0.5, linewidth=2, zorder=1)

# Максимальное ускорение (асимптота)
max_speedup = 1 / (1 - f_theoretical)
ax1.axhline(y=max_speedup, color='red', linestyle=':', linewidth=2, 
            alpha=0.6, label=f'Предел = {max_speedup:.1f}x', zorder=2)

ax1.set_xlabel('Количество потоков', fontsize=13, fontweight='bold')
ax1.set_ylabel('Ускорение S(p)', fontsize=13, fontweight='bold')
ax1.legend(fontsize=11, loc='upper left')
ax1.grid(True, alpha=0.3, linestyle='--')
ax1.set_xlim(0.5, max(threads) + 0.5)
ax1.set_ylim(0.5, max(max(speedup) * 1.2, max_speedup * 1.1))

# Подписи к точкам
for i in range(len(threads)):
    ax1.annotate(f'{speedup[i]:.2f}x', 
                xy=(threads[i], speedup[i]), 
                xytext=(7, 7), 
                textcoords='offset points',
                fontsize=10,
                bbox=dict(boxstyle='round,pad=0.4', facecolor='yellow', alpha=0.8),
                zorder=4)

# ГРАФИК 2: ЭФФЕКТИВНОСТЬ
efficiency = [(speedup[i] / threads[i]) * 100 for i in range(len(threads))]

ax2.plot(threads, efficiency, 'mo-', markersize=12, linewidth=2.5)
ax2.axhline(y=50, color='orange', linestyle='--', linewidth=2, 
            label='50% порог', alpha=0.7)
ax2.axhline(y=75, color='green', linestyle='--', linewidth=2, 
            label='75% хорошо', alpha=0.5)

ax2.set_xlabel('Количество потоков', fontsize=13, fontweight='bold')
ax2.set_ylabel('Эффективность (%)', fontsize=13, fontweight='bold')
ax2.set_title('Эффективность использования потоков', 
              fontsize=14, fontweight='bold')
ax2.legend(fontsize=11)
ax2.grid(True, alpha=0.3)
ax2.set_xlim(0.5, max(threads) + 0.5)
ax2.set_ylim(0, 110)

# Подписи
for i in range(len(threads)):
    color = 'green' if efficiency[i] > 75 else ('orange' if efficiency[i] > 50 else 'red')
    ax2.annotate(f'{efficiency[i]:.1f}%', 
                xy=(threads[i], efficiency[i]), 
                xytext=(7, 7), 
                textcoords='offset points',
                fontsize=10,
                bbox=dict(boxstyle='round,pad=0.4', facecolor=color, alpha=0.3))

plt.tight_layout()
plt.savefig('benchmark_amdahl.png', dpi=300, bbox_inches='tight')

# ============ ОТДЕЛЬНЫЙ ГРАФИК: ВРЕМЯ ============
plt.figure(figsize=(10, 6))
plt.plot(threads, times, 'rs-', markersize=12, linewidth=2.5)
plt.xlabel('Количество потоков', fontsize=13, fontweight='bold')
plt.ylabel('Время выполнения (сек)', fontsize=13, fontweight='bold')
plt.title('Время выполнения vs количество потоков', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)

for i in range(len(threads)):
    plt.annotate(f'{times[i]:.2f}с', 
                xy=(threads[i], times[i]), 
                xytext=(7, 7), 
                textcoords='offset points',
                fontsize=10,
                bbox=dict(boxstyle='round,pad=0.4', facecolor='lightblue', alpha=0.8))

plt.tight_layout()
plt.savefig('benchmark_time.png', dpi=300, bbox_inches='tight')

plt.show()