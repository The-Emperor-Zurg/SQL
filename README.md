### Упрощенная реляционная БД с поддержкой "урезанного" sql. 
Проект выполнен в виде библиотеки, которая содержит класс MyDataBase, который обладает публичным методом для выполнения запроса, а также возможностью сохранения и загрузки текущего состояния БД в файл.

### Поддерживаемый синтаксис

Ключевые слова:

- SELECT
- FROM
- WHERE
- (LEFT|RIGHT|INNER)JOIN
- CREATE TABLE
- DROP TABLE
- AND
- OR
- IS
- NOT
- NULL
- ON
- UPDATE
- INSERT
- VALUES
- DELETE
- PRIMARY KEY
- FOREIGN KEY

Поддерживаемые типы данных:

- bool
- int
- float
- double
- varchar

Ограничения:

- вложенные подзапросы не поддерживаются
- Join только для 2 таблиц

## Реализация

Работа выполнена в виде библиотеки. Для хранение данных используется только память. Тесты также пристутсвуют.
Результат выполнения любого запроса должен иметь возможность отображения в стандартном потоке вывода.
