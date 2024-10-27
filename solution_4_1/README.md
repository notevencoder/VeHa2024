# Решение задачи "Дедуктивная верификация программы, относящейся к задаче проверки выполнимости булевых формул (SAT)"

Инвариант цикла построен следующим образом:

1. Первая часть содержит ограничения, заданные в предусловии (типы и диапазоны значений).
2. Вторая часть задаёт аналогичные ограничения на переменную цикла.
3. Третья часть задаёт условия, при которых возвращаемое значение становится равным посылке из постусловия. Это же значение является условием выхода из цикла.

Доказательство осуществляется автоматический системой _C-lightVer_ с использованием теорем предметной области, в частности теоремы `twosat-unsat`.

Основную сложность представляют функции и теоремы предметной области, предоставленные в готовом виде автором задания. Вынесение их в само задание сделало бы задание гораздо сложнее и потребовало бы гораздо большего погружения.