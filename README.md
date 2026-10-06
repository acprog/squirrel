# Squirrel plugin for Unreal engine
Squirrel script plugin for unreal engine

Code from the following repositories is used:

https://github.com/albertodemichelis/squirrel
Copyright (c) 2003-2022 Alberto Demichelis

https://github.com/matusnovak/simplesquirrel
Copyright (c) 2019 Matus Novak matusnov@gmail.com

Отличия от Squirrel 3.2:
1. operator 'in' для array работает по логике из table - для проверки наличия элемента в массиве
2. требуется local перед переменными в foreach (local ...)
3. автоматический .bindenv(this) для всех вложеных функций
4. складывание массивов + и += в дополнение к методу extend()
5. метод find() для table по аналогии с array (поиск по ключу, возращает значение) 
6. в массив добавлены методы indexof и removeat (дублирующие find и remove)
7. инициализация значений по умолчанию для полей, унаследованных от C++ класса
8. методы tostring() для таблицы и массива печатают их содержимое
9. создание вложенных классов
10. унарный оператор ! для проверки переменных
11. разрешен необязательный знак + перед положительными числами
12. разрешена передача в параметры функции int как float и float как int
13. возможность получения элементов массива с конца через отрицательные индексы [-1]
14. срез массива [:] по аналогии с python

