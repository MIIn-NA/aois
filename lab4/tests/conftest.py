# tests/conftest.py
import sys
import os

# Добавляем корень проекта (родительскую папку tests/) в sys.path
# Это позволяет импортировать модули как: from DoubleHashTable import ...
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))