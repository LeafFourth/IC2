#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include <string>

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

    JsonObject(cJSON *j);
    JsonObject(int value);
    JsonObject(double value);
    JsonObject(const std::string &value);
    JsonObject(bool value);

    JsonObject(const JsonObject &obj);
    JsonObject(JsonObject &&obj);


    JsonObject(const JsonArray &obj);
    JsonObject(JsonArray &&obj);

    void operator=(const JsonObject &obj);
    void operator=(JsonObject &&obj);

    static JsonObject Parse(const std::string &jsonStr);
    std::string toJson() const;

public:
    JsonObject operator[](const std::string &key);

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

    JsonArray transferToArray();

private:
    void clean();

private:
    cJSON *json_;
    //Type type_;
};

class JsonArray {
public:
    JsonArray();

    JsonArray(cJSON *json);

public:
    std::string toJson() const;

    int getSize();

    JsonObject operator[](int);

    JsonObject toObject() const;
    JsonObject transferToObject();
private:
    cJSON *json_;
};

#endif // JSONOBJECT_H