# DoubleHashTable.py
import sys
import os

# Добавляем корень проекта в путь для импортов
_current_dir = os.path.dirname(os.path.abspath(__file__))
if _current_dir not in sys.path:
    sys.path.insert(0, _current_dir)

from HashCell import HashCell
from Utils import calculate_V


class DoubleHashTable:
    def __init__(self, capacity: int = 20, base_address: int = 0):
        self.H = capacity
        self.B = base_address
        self.table = [HashCell() for _ in range(self.H)]
        self.size = 0
        self.collision_count = 0
        self.chain_count = 0

    def _h1(self, key: str) -> int:
        return (calculate_V(key) + self.B) % self.H

    def _h2(self, key: str) -> int:
        return 1 + (calculate_V(key) % (self.H - 1))

    def _find_position(self, key: str) -> tuple:
        """Возвращает (status, idx, cell, steps, first_tombstone)"""
        h1, h2 = self._h1(key), self._h2(key)
        first_tombstone = None

        for i in range(self.H):
            idx = (h1 + i * h2) % self.H
            cell = self.table[idx]

            if cell.U == 0 and cell.D == 0:
                return "empty", idx, cell, i, first_tombstone
            if cell.ID == key.upper() and cell.D == 0:
                return "found", idx, cell, i, None
            if cell.D == 1 and first_tombstone is None:
                first_tombstone = (idx, cell, i)

        return "full", -1, None, 0, first_tombstone

    def insert(self, key: str, data: str) -> bool:
        """CREATE: Вставка записи"""
        status, idx, cell, i, tombstone = self._find_position(key)
        if status == "found":
            return False
        if status == "full" and tombstone is None:
            return False

        target, final_i = (tombstone[1], tombstone[2]) if tombstone else (cell, i)

        target.ID = key.upper()
        target.Data = data
        target.U = 1
        target.D = 0
        target.T = 1
        target.L = 0
        target.Po = -1
        target.C = 1 if final_i > 0 else 0

        self.size += 1
        if final_i > 0:
            self.collision_count += 1
            self.chain_count += 1
        return True

    def search(self, key: str) -> tuple | None:
        """READ: Поиск по ключу"""
        status, idx, cell, _, _ = self._find_position(key)
        return (idx, cell) if status == "found" else None

    def update(self, key: str, new_data: str) -> bool:
        """UPDATE: Изменение данных"""
        res = self.search(key)
        if res is None:
            return False
        _, cell = res
        cell.Data = new_data
        return True

    def delete(self, key: str) -> bool:
        """DELETE: Логическое удаление (методичка)"""
        res = self.search(key)
        if res is None:
            return False
        _, cell = res
        cell.D = 1
        cell.U = 0
        self.size -= 1
        return True

    def get_load_factor(self) -> float:
        return self.size / self.H if self.H > 0 else 0.0

    def display(self) -> None:
        """Вывод таблицы в формате методички"""
        header = f"{'№':<4} | {'Ключ':<12} | {'V':<5} | {'h':<4} || {'ID':<10} | {'C':<2} | {'U':<2} | {'T':<2} | {'L':<2} | {'D':<2} | {'Po':<3} | {'Data'}"
        print("\n" + "=" * 105)
        print("ЛЕВАЯ ЧАСТЬ (Расчётные) | ПРАВАЯ ЧАСТЬ (Состояние ТХ)")
        print(header)
        print("-" * 105)
        for i, cell in enumerate(self.table):
            if cell.U == 1 and cell.D == 0:
                v = calculate_V(cell.ID)
                h = self._h1(cell.ID)
                print(
                    f"{i:<4} | {cell.ID:<12} | {v:<5} | {h:<4} || {cell.ID:<10} | {cell.C:<2} | {cell.U:<2} | {cell.T:<2} | {cell.L:<2} | {cell.D:<2} | {cell.Po:<3} | {cell.Data}")
            elif cell.D == 1:
                # Исправленная строка — убрана лишняя }
                print(
                    f"{i:<4} | {'[УДАЛ]':<12} | {'-':<5} | {'-':<4} || {'[УДАЛ]':<10} | 0 | 0 | 0 | 0 | 1 | -1 | [могила]")
            else:
                print(f"{i:<4} | {'[ПУСТО]':<12} | {'-':<5} | {'-':<4} || {'[ПУСТО]':<10} | - | 0 | - | - | 0 | -1 | -")
        print("=" * 105)
        print(
            f"📊 Заполнение: {self.get_load_factor():.2%} | 💥 Коллизии: {self.collision_count} |  Цепочки: {self.chain_count}")
