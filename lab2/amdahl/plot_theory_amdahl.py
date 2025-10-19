import matplotlib.pyplot as plt

def amdahl(p, alpha):
    """Закон Амдала: S(p) = 1 / (α + (1-α)/p)"""
    return 1.0 / (alpha + (1.0 - alpha)/p)

alpha_estimated = 0.5  # примерная оценка 50% кода параллелится

print("=" * 70)
print("Теоретический график закона Амдала")
print("=" * 70 + "\n")
print(f"Оценка параллельной доли: α = {alpha_estimated:.2f} ({alpha_estimated*100:.0f}%)")
print(f"Последовательная доля: {(1-alpha_estimated)*100:.0f}%")
print(f"Максимальное теоретическое ускорение: {1/(1-alpha_estimated):.1f}x\n")

# Данные для графика
threads_list = [1, 2, 3, 4, 6, 8, 12, 16]
speedup_values = [amdahl(p, alpha_estimated) for p in threads_list]

print("Теоретические значения:")
print("-" * 70)
print(f"{'Потоки':<10} {'Ускорение':<12} {'Эффективность':<15}")
print("-" * 70)
for i, p in enumerate(threads_list):
    eff = (speedup_values[i] / p) * 100
    print(f"{p:<10} {speedup_values[i]:<12.2f} {eff:<15.1f}%")

# Построение графика
threads_smooth = [i/10.0 for i in range(10, 161)]
speedup_smooth = [amdahl(p, alpha_estimated) for p in threads_smooth]

plt.figure(figsize=(12, 8))

# Теоретическая кривая
plt.plot(threads_smooth, speedup_smooth, 'b-', linewidth=3, 
         label=f'Закон Амдала для программы (α={alpha_estimated:.2f})')

# Точки
plt.plot(threads_list, speedup_values, 'ro', markersize=10, 
         label='Теоретические значения')

# Идеальное ускорение
plt.plot(threads_smooth, threads_smooth, 'g--', linewidth=2, 
         alpha=0.5, label='Идеальное (линейное)')

# Максимальное ускорение (асимптота)
max_speedup = 1 / (1 - alpha_estimated)
plt.axhline(y=max_speedup, color='red', linestyle=':', linewidth=2, 
            alpha=0.7, label=f'Максимум = {max_speedup:.1f}x')

# Подписи к точкам
for i, p in enumerate(threads_list):
    plt.annotate(f'{speedup_values[i]:.2f}x', 
                xy=(p, speedup_values[i]), 
                xytext=(5, 5), 
                textcoords='offset points',
                fontsize=10,
                bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.7))

plt.xlabel('Количество потоков (p)', fontsize=14, fontweight='bold')
plt.ylabel('Ускорение S(p)', fontsize=14, fontweight='bold')
plt.title('Теоретический график закона Амдала', 
          fontsize=15, fontweight='bold')
plt.legend(fontsize=11, loc='upper left')
plt.grid(True, alpha=0.3, linestyle='--')
plt.xlim(1, 16)
plt.ylim(1, max_speedup * 1.2)

plt.tight_layout()
plt.savefig('gauss_theory_amdahl.png', dpi=300, bbox_inches='tight')
plt.show()

# График эффективности
plt.figure(figsize=(10, 6))
efficiency = [(speedup_values[i] / threads_list[i]) * 100 for i in range(len(threads_list))]

plt.plot(threads_list, efficiency, 'mo-', markersize=10, linewidth=2.5)

for i, p in enumerate(threads_list):
    plt.annotate(f'{efficiency[i]:.1f}%', 
                xy=(p, efficiency[i]), 
                xytext=(5, 5), 
                textcoords='offset points',
                fontsize=10)

plt.xlabel('Количество потоков', fontsize=14, fontweight='bold')
plt.ylabel('Эффективность (%)', fontsize=14, fontweight='bold')
plt.title('Теоретическая эффективность использования потоков', 
          fontsize=15, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.xlim(1, 16)
plt.ylim(0, 110)

plt.tight_layout()
plt.savefig('gauss_theory_efficiency.png', dpi=300, bbox_inches='tight')
plt.show()