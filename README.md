Task 10 - модельный интерпретатор SQL

Архитектура проекта 
Проект состоит из 5 папок с файлами: sql, lib, executor, models, parser.
Связующим звеном всего проекта является папка sql, в которой содержится файл SQL.cpp,  в котором есть класс SQL c private данными  SQLExecutor и SQLParser. А также метод run, который запускает весь процесс.  
В папке lib хранится файл Table.cpp для работы с таблицами через C++.  
В папке executor содержится файл SQLExecutor.cpp , который нужен для выполнения парсера и папка poliz, с помощью которой мы вычисляем все арифметические выражения. 
В папке models хранятся все необходимые структуры данных для парсинга запросов. 
В папке parser содержится файл SQLParser.cpp  , который выполняем парсер всех запросов SQL , заполняет для каждого запроса  нужную структуру данных из models и передаёт эту структуру в SQLExecutore.cpp.

Программа запускается из main.cpp. Создаётся объект класса SQL и считывается строка из терминала. Входная строка передается в метод SQL::run , происходит парсинг выражения и само выполнение запроса. 

Для компиляции проекта используем make. ./build/sql# SQL_interpretator_c-
