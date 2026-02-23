a) Gehen Sie davon aus, dass Sie die letzten vier Bits auf 0 setzen und die ersten vier Bits unverändert lassen wollen. Wie würden Sie dies mit einer bitweisen Operation tun?
    >> 4 -> die letzten 4 Bits werden gelöscht
    << 4 -> 4 0 werden hinten hinzugefügt

b) Wenn Sie eine 8-Bit-Zahl haben und möchten die ersten 4 Bits mit den letzten 4 Bits vertauschen (z. B. wird aus 01101010 nun 10100110). Wie können Sie dies mit den genannten bitweisen Operationen erreichen?
    (01101010 >> 4) | (01101010 << 4)
    

c) Angenommen, Sie haben eine 8-Bit-Zahl 11010100. Sie möchten das 4. und 5. Bit aus dieser Zahl extrahieren (von rechts gezählt, beginnend mit 1). Wie können Sie dies mit bitweisen Operationen erreichen?
    11010100 & 00011000 = 00010000 >> 3 = 10

d) Nehmen wir eine 8-Bit-Zahl 10011011. Sie möchten nur das 3. Bit (von rechts, beginnend mit 1) auf 1 setzen. Mit welchen der genannten bitweisen Operationen können Sie dies erreichen und wie?
    10011011 | 00000100 = 10011111
    