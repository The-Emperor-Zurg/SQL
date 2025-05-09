#include "../lib/MyDataBase.h"

#include <gtest/gtest.h>

TEST(Test1, Insert) {
    MyBD db;

    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");

    Result res = db.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(res.GetCounter(), 4);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367124);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Nazar Vakulenko");
    ASSERT_EQ(res.Get("points").GetDoubleVal(), 53.7);
    ASSERT_EQ(res.Get("budget").GetBoolVal(), true);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367111);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Petr Ivanov");
    ASSERT_TRUE(res.Get("points").IsNull());
    ASSERT_EQ(res.Get("budget").GetBoolVal(), true);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367222);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Igor Meshechkin");
    ASSERT_EQ(res.Get("points").GetDoubleVal(), 100);
    ASSERT_TRUE(res.Get("budget").IsNull());

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367121);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Danil Kashirin");
    ASSERT_TRUE(res.Get("points").IsNull());
    ASSERT_EQ(res.Get("budget").GetBoolVal(), true);

    ASSERT_FALSE(res.Next());
}

TEST(Test2, Update) {
    MyBD db;
    
    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");

    db.RequestParsing("UPDATE Students SET points = 74.01 WHERE isu = 367124");
    db.RequestParsing("UPDATE Students SET points = NULL WHERE isu = 367111");
    Result res = db.RequestQuery("SELECT * FROM Students WHERE isu = 367124 OR isu = 367111");
    ASSERT_EQ(res.GetCounter(), 2);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367124);
    ASSERT_EQ(res.Get("points").GetDoubleVal(), 74.01);
    ASSERT_EQ(res.Get("budget").GetBoolVal(), true);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367111);
    ASSERT_TRUE(res.Get("points").IsNull());
}

TEST(Test3, Select) {
    MyBD db;
    
    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");

    Result res1 = db.RequestQuery("SELECT isu, budget FROM Students WHERE name = 'Nazar Vakulenko'");
    ASSERT_EQ(res1.GetCounter(), 1);
    ASSERT_EQ(res1.Get("isu").GetIntVal(), 367124);
    ASSERT_EQ(res1.Get("budget").GetBoolVal(), true);

    Result res2 = db.RequestQuery("SELECT name FROM Students WHERE points <= 100");
    ASSERT_EQ(res2.GetCounter(), 2);
    res2.Next();
    ASSERT_EQ(res2.Get("name").GetStringVal(), "Nazar Vakulenko");
    res2.Next();
    ASSERT_EQ(res2.Get("name").GetStringVal(), "Igor Meshechkin");
    ASSERT_FALSE(res2.Next());
}

TEST(Test4, Delete) {
    MyBD db;

    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");

    db.RequestParsing("DELETE FROM Students WHERE isu <= 666");
    Result res1 = db.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(res1.GetCounter(), 4);

    db.RequestParsing("DELETE FROM Students");
    Result res2 = db.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(res2.GetCounter(), 0);
}

TEST(Test5, Drop) {
    MyBD db;

    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");

    db.RequestParsing("CREATE TABLE Test ("
                      "    ITMO BOOL"
                      ")");

    ASSERT_EQ(db.GetTableNames(), std::vector<std::string>({"Students", "Test"}));
    db.RequestParsing("DROP TABLE Students");
    ASSERT_EQ(db.GetTableNames(), std::vector<std::string>({"Test"}));
    db.RequestParsing("DROP TABLE Test");
    ASSERT_EQ(db.GetTableNames(), std::vector<std::string>());
}

TEST(TEST6, PrimaryKey) {
    MyBD db;

    db.RequestParsing("CREATE TABLE Examin ("
                      "    priority INT PRIMARY KEY,"
                      "    name VARCHAR"
                      ")");

    db.RequestParsing("INSERT INTO Examin(priority, name) VALUES (1, 'Math')");
    db.RequestParsing("INSERT INTO Examin(priority, name) VALUES (2, 'C++')");

    ASSERT_THROW(db.RequestParsing("INSERT INTO Examin(priority, name) VALUES (1, 'Math')"), std::invalid_argument);
    ASSERT_THROW(db.RequestParsing("INSERT INTO Examin(priority, name) VALUES (2, 'C++')"), std::invalid_argument);
}

TEST(TEST7, ForeignKey) {
    MyBD db;

    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");

    db.RequestParsing("CREATE TABLE Food ("
                      "    id INT PRIMARY KEY,"
                      "    customer_id INT,"
                      "    product VARCHAR,"
                      "    FOREIGN KEY (customer_id) REFERENCES Students(isu)"
                      ")");

    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (1, 1, 'Pizza')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (2, 1, 'Soup')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (3, 2, 'Sushi')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (4, 2, 'Sashimi')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (5, 3, 'RedBull')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (6, 4, 'Coffee')");
    ASSERT_THROW(db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (7, 5, 'Cheese')"), std::invalid_argument);
    ASSERT_THROW(db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (8, 6, 'Bread')"), std::invalid_argument);
}

TEST(Test8, DownloadToFrom) {
    MyBD db;

    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");
    db.DownloadTo("Test");

    MyBD data_base;
    data_base.DownloadFrom("Test");

    Result res = data_base.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(res.GetCounter(), 4);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367124);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Nazar Vakulenko");
    ASSERT_EQ(res.Get("points").GetDoubleVal(), 53.7);
    ASSERT_EQ(res.Get("budget").GetBoolVal(), true);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367111);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Petr Ivanov");
    ASSERT_TRUE(res.Get("points").IsNull());
    ASSERT_EQ(res.Get("budget").GetBoolVal(), true);

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367222);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Igor Meshechkin");
    ASSERT_EQ(res.Get("points").GetDoubleVal(), 100);
    ASSERT_TRUE(res.Get("budget").IsNull());

    res.Next();
    ASSERT_EQ(res.Get("isu").GetIntVal(), 367121);
    ASSERT_EQ(res.Get("name").GetStringVal(), "Danil Kashirin");
    ASSERT_TRUE(res.Get("points").IsNull());
    ASSERT_EQ(res.Get("budget").GetBoolVal(), true);

    ASSERT_FALSE(res.Next());
}

TEST(Test9, Join) {
    MyBD db;

    db.RequestParsing("CREATE TABLE Students ("
                      "    isu    INT,"
                      "    name  VARCHAR,"
                      "    points   DOUBLE,"
                      "    budget  BOOL"
                      ");");

    db.RequestParsing("INSERT INTO Students(isu, name, points, budget) VALUES (367124, 'Nazar Vakulenko', 53.7, true)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367111, 'Petr Ivanov', 1)");
    db.RequestParsing("INSERT INTO Students(isu, name, points) VALUES (367222, 'Igor Meshechkin', 100)");
    db.RequestParsing("INSERT INTO Students(isu, name, budget) VALUES (367121, 'Danil Kashirin', true)");

    db.RequestParsing("CREATE TABLE Food ("
                      "    id INT PRIMARY KEY,"
                      "    customer_id INT,"
                      "    product VARCHAR,"
                      ")");

    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (1, 1, 'Pizza')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (2, 1, 'Soup')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (3, 2, 'Sushi')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (4, 2, 'Sashimi')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (5, 3, 'RedBull')");
    db.RequestParsing("INSERT INTO Food(id, customer_id, product) VALUES (6, 4, 'Coffee')");

    Result res1 = db.RequestQuery("SELECT * FROM Students INNER JOIN Food ON Food.customer_id = Students.isu");
    
    ASSERT_EQ(res1.GetCounter(), 7);
    res1.Next();
    ASSERT_EQ(res1.Get("Food.id").GetIntVal(), 1);
    ASSERT_EQ(res1.Get("Students.isu").GetIntVal(), 1);
    ASSERT_EQ(res1.Get("Students.name").GetStringVal(), "NazarVakulenko");
    ASSERT_EQ(res1.Get("Food.product").GetStringVal(), "Pizza");

    res1.Next();
    ASSERT_EQ(res1.Get("Food.id").GetIntVal(), 2);
    ASSERT_EQ(res1.Get("Students.isu").GetIntVal(), 1);
    ASSERT_EQ(res1.Get("Students.name").GetStringVal(), "Nazar Vakulenko");
    ASSERT_EQ(res1.Get("Food.product").GetStringVal(), "Soup");

    Result res2 = db.RequestQuery("SELECT * FROM Students LEFT JOIN Food ON Food.customer_id = Students.isu");

    Result res3 = db.RequestQuery("SELECT * FROM Students Right JOIN Food ON Food.customer_id = Students.isu");

}