class HashCell:
    """Формат ячейки хеш-таблицы строго по Рисунку 1 методички."""
    def __init__(self):
        self.ID = ""     
        self.C = 0        
        self.U = 0       
        self.T = 0       
        self.L = 0        
        self.D = 0        
        self.Po = -1      
        self.Data = ""  

    def reset(self):
        """Возврат ячейки в начальное состояние."""
        self.__init__()
