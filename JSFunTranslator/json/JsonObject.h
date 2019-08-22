#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include <string>
#include <vector>

struct cJSON;

class JsonArray;

class JsonObject {
public:
    //enum Type {
    //    TYPE_NULL = 0,
    //    TYPE_OBJECT,
    //    TYPE_ARRAY,
    //    TYPE_INT,
    //    TYPE_DOUBLE,
    //    TYPE_STRING,
    //    TYPE_BOOL
    //};

public:
    JsonObject();
    ~JsonObject();

    explicit JsonObject(cJSON *j);
    explicit JsonObject(int value);
    explicit JsonObject(double value);
    JsonObject(const std::string &value);
    explicit JsonObject(bool value);

    JsonObject(const JsonObject &obj);
    JsonObject(JsonObject &&obj);


    JsonObject(const JsonArray &obj);
    explicit JsonObject(JsonArray &&obj);

    void operator=(const JsonObject &obj);
    void operator=(JsonObject &&obj);

    static JsonObject Parse(const std::string &jsonStr);
    std::string toJson() const;

public:
    JsonObject operator[](const std::string &key);

    bool isEmpty() const;
    bool isObject() const;
    bool isArray() const;
    bool isInt() const;
    bool isDouble() const;
    bool isString() const;
    bool isBool() const;

    std::string toString() const;
    int toInt() const;
    double toDouble() const;
    bool toBool() const;

    JsonArray toArray() const;

    //move
    JsonArray transferToArray();

    void insert(const std::string &key, int val);
    void insert(const std::string &key, double val);
    void insert(const std::string &key, bool val);
    void insert(const std::string &key, const std::string &val);
    
    void insert(const std::string &key, const JsonArray &val);
    void insert(const std::string &key, const JsonObject &val);

    cJSON *take();

private:
    void clean();
    void initOnNull();
    cJSON *getInsertNode(const std::vector<std::string> &paths);

    static std::vector<std::string> parseKey(const std::string &key);

private:
    cJSON *json_;
    //Type type_;
};

class JsonArray {
public:
    JsonArray();
    JsonArray(const std::string &array);

    explicit JsonArray(cJSON *json);
    JsonArray(const JsonArray& obj);
    JsonArray(JsonArray&& obj);
    ~JsonArray();

public:
    JsonArray& operator=(const JsonArray& obj);
    JsonArray &operator=(JsonArray&& obj);

    std::string toJson() const;

    int getSize() const;

    JsonObject operator[](int);

    JsonObject toObject() const;

    void insert(const std::string &key, const JsonObject &item);

    //move
    JsonObject transferToObject();

private:
    void clean();

private:
    cJSON *json_;
};

#endif // JSONOBJECT_H
