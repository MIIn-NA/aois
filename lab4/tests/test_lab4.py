
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import pytest
from unittest.mock import patch

from DoubleHashTable import DoubleHashTable
from HashCell import HashCell
from Utils import calculate_V


def test_calculate_v_standard():
    assert calculate_V("Абаев") == 0 * 33 + 1  # А=0, Б=1
    assert calculate_V("Вяткин") == 2 * 33 + 32  # В=2, Я=32

def test_calculate_v_edge_cases():
    assert calculate_V("А") == 0
    assert calculate_V("") == 0
    assert calculate_V("test") == 0  # Латиница → 0
    assert calculate_V("аб") == calculate_V("АБ")  # Регистронезависимость


def test_cell_init_and_reset():
    c = HashCell()
    assert c.U == 0 and c.D == 0 and c.ID == ""
    c.ID = "X"; c.U = 1; c.D = 1
    c.reset()
    assert c.ID == "" and c.U == 0 and c.D == 0


@pytest.fixture
def small_table():
    """Таблица малого размера для контролируемых тестов"""
    return DoubleHashTable(capacity=5, base_address=0)

def test_insert_and_search(small_table):
    assert small_table.insert("Иванов", "Спорт") is True
    assert small_table.size == 1
    res = small_table.search("Иванов")
    assert res is not None and res[1].Data == "Спорт"
    assert small_table.search("Петров") is None

def test_insert_duplicate(small_table):
    small_table.insert("Иванов", "A")
    assert small_table.insert("Иванов", "B") is False
    assert small_table.size == 1

def test_insert_tombstone_reuse(small_table):
    small_table.insert("Андрей", "A")
    small_table.delete("Андрей")
    assert small_table.insert("Борис", "B") is True
    assert any(c.ID == "БОРИС" and c.U == 1 and c.D == 0 for c in small_table.table)

def test_insert_full(small_table):
    keys = ["Аа", "Бб", "Вв", "Гг", "Дд"]
    for k in keys:
        small_table.insert(k, "X")
    assert small_table.insert("Ее", "Y") is False

def test_update_and_delete(small_table):
    small_table.insert("Козлов", "Плавание")
    assert small_table.update("Козлов", "Бег") is True
    assert small_table.search("Козлов")[1].Data == "Бег"
    assert small_table.update("Нет", "X") is False

    assert small_table.delete("Козлов") is True
    assert small_table.size == 0
    assert small_table.search("Козлов") is None
    assert small_table.delete("Козлов") is False


def test_collision_and_chain_counting(small_table):
    small_table.insert("Аб", "1")  
    small_table.insert("Ва", "2")  

    assert small_table.collision_count >= 1, "Коллизия не зафиксирована"
    assert small_table.chain_count >= 1, "Цепочка пробинга не зафиксирована"

    
    res = small_table.search("Ва")
    assert res is not None
    idx, cell = res
    assert cell.C == 1, "Флажок коллизии C не установлен"

def test_load_factor(small_table):
    assert small_table.get_load_factor() == 0.0
    small_table.insert("Аа", "X")
    small_table.insert("Бб", "Y")
    assert small_table.get_load_factor() == pytest.approx(0.4)

def test_display_output(small_table):
    small_table.insert("Тест", "Data")
    with patch("builtins.print") as mock_print:
        small_table.display()
        assert mock_print.call_count > 10
