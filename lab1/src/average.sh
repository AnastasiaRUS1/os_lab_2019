 #!/bin/bash

   # Проверка наличия аргументов
   if [ "$#" -eq 0 ]; then
       echo "Нет входных аргументов"
       exit 1
   fi

   # Подсчет суммы и количества
   sum=0
   count=0
   for arg in "$@"; do
       sum=$(echo "$sum + $arg" | bc)
       count=$((count + 1))
   done

   # Вывод результата
   average=$(echo "scale=2; $sum / $count" | bc)
   echo "Количество: $count"
   echo "Среднее арифметическое: $average"
