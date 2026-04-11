
ALPHABET = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
CHAR_MAP = {ch: i for i, ch in enumerate(ALPHABET)}

def calculate_V(key: str) -> int:
    """
    Перевод ключевого слова в числовое значение V по первым двум буквам.
    Формула: V = v1 * 33 + v2
    """
    if not key:
        return 0
    key = key.upper()
    v1 = CHAR_MAP.get(key[0], 0)
    v2 = CHAR_MAP.get(key[1], 0) if len(key) > 1 else 0
    return v1 * 33 + v2
