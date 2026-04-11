
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from DoubleHashTable import DoubleHashTable


def print_help():
    """Вывод справки по командам"""
    print("\n ДОСТУПНЫЕ КОМАНДЫ:")
    print("  insert <ключ> <данные>  — добавить запись (CREATE)")
    print("  search <ключ>           — найти запись (READ)")
    print("  update <ключ> <данные>  — обновить запись (UPDATE)")
    print("  delete <ключ>           — удалить запись (DELETE)")
    print("  display                 — показать всю таблицу")
    print("  stats                   — показать статистику")
    print("  help                    — эта справка")
    print("  exit / quit             — выход из программы")
    print("-" * 60)


def main():
    print("=" * 60)
    print("1"
          " ЛАБОРАТОРНАЯ РАБОТА №4: Хеш-таблицы")
    print(" Метод: Двойное хеширование")
    print(" Размер таблицы: 20 ячеек, начальный адрес: 0")
    print("=" * 60)

    ht = DoubleHashTable(capacity=20, base_address=0)

    print_help()


    while True:
        try:
            cmd = input("\n>>> ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n Завершение работы.")
            break

        if not cmd:
            continue

        parts = cmd.split(maxsplit=2)
        action = parts[0].lower()

     
        if action in ("exit", "quit", "q"):
            print(" До свидания!")
            break

        elif action == "help":
            print_help()


        elif action == "insert":
            if len(parts) < 3:
                print(" Формат: insert <ключ> <данные>")
                continue
            key, data = parts[1], parts[2]
            if ht.insert(key, data):
                print(f" Вставлено: '{key}' → '{data}'")
            else:
                print(f" Ошибка: ключ '{key}' уже существует или таблица заполнена")

   
        elif action == "search":
            if len(parts) < 2:
                print(" Формат: search <ключ>")
                continue
            key = parts[1]
            res = ht.search(key)
            if res:
                idx, cell = res
                print(f" Найдено: индекс={idx}, ключ='{cell.ID}', данные='{cell.Data}'")
            else:
                print(f" Ключ '{key}' не найден")

    
        elif action == "update":
            if len(parts) < 3:
                print(" Формат: update <ключ> <новые_данные>")
                continue
            key, new_data = parts[1], parts[2]
            if ht.update(key, new_data):
                print(f" Обновлено: '{key}' → '{new_data}'")
            else:
                print(f" Ключ '{key}' не найден")

  
        elif action == "delete":
            if len(parts) < 2:
                print(" Формат: delete <ключ>")
                continue
            key = parts[1]
            if ht.delete(key):
                print(f" Удалено: '{key}'")
            else:
                print(f" Ключ '{key}' не найден")

        # === DISPLAY ===
        elif action == "display":
            ht.display()


        elif action == "stats":
            print("\n СТАТИСТИКА ХЕШ-ТАБЛИЦЫ:")
            print(f"  Размер таблицы (H): {ht.H}")
            print(f"  Начальный адрес (B): {ht.B}")
            print(f"  Записей: {ht.size}")
            print(f"  Коэффициент заполнения: {ht.get_load_factor():.2%}")
            print(f"  Коллизий: {ht.collision_count}")
            print(f"  Записей в цепочках пробинга: {ht.chain_count}")
            print("-" * 60)
           
            checks = [
                ("H ≥ 20", ht.H >= 20),
                ("Записей ≥ 10", ht.size >= 10),
                ("Коллизий ≥ 2", ht.collision_count >= 2),
                ("Цепочек ≥ 3", ht.chain_count >= 3),
            ]
            print("📋 ПРОВЕРКА ТРЕБОВАНИЙ:")
            for name, passed in checks:
                print(f"  {'прошли' if passed else 'не прошли'} {name}")

      
        else:
            print(f" Неизвестная команда: '{action}'. Введите 'help' для справки.")


if __name__ == "__main__":
    main()
