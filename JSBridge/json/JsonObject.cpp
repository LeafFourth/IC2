#include "JsonObject.h"

#include <vector>

#include "impl/cJSON.h"

JsonObject::JsonObject() : json_(nullptr)
{

}

JsonObject::~JsonObject()
{
    clean();
}

JsonObject::JsonObject(cJSON *j)
{
    json_ = j;
}

JsonObject::JsonObject(int value)
{
    json_ = cJSON_CreateNumber((double)value);
}

JsonObject::JsonObject(double value)
{
    json_ = cJSON_CreateNumber((double)value);
}

JsonObject::JsonObject(const std::string &value)
{
    json_ = cJSON_CreateString(value.c_str());
}

JsonObject::JsonObject(bool value)
{
    json_ = cJSON_CreateBool((cJSON_bool)value);
}

JsonObject::JsonObject(const JsonObject &obj) : json_(nullptr)
{
    *this = obj;
}

JsonObject::JsonObject(JsonObject &&obj) : json_(nullptr)
{
    *this = std::move(obj);
}

JsonObject::JsonObject(const JsonArray &obj)
{
    *this = obj.toObject();
}

JsonObject::JsonObject(JsonArray &&obj)
{
    *this = std::move(obj).transferToObject();
}

void JsonObject::operator=(const JsonObject &obj)
{
    clean();

    if (obj.json_) {
        this->json_ = cJSON_Duplicate(obj.json_, (cJSON_bool)true);
    }
}


void JsonObject::operator=(JsonObject &&obj)
{
    clean();

    this->json_ = obj.json_;
    obj.json_ = nullptr;
}

//static
JsonObject JsonObject::Parse(const std::string &jsonStr)
{
    cJSON *json = cJSON_Parse(jsonStr.c_str());
    return JsonObject(json);
}

std::string JsonObject::toJson() const
{
    if (!json_) {
        return std::string();
    }

    char *data = cJSON_Print(json_);
    std::string ret(data);

    if (data) {
        cJSON_free(data);
        data = nullptr;
    }

    return std::move(ret);
}

JsonObject JsonObject::operator[](const std::string &key)
{
    if (key.size() == 0 || !json_) {
        return JsonObject();
    }

    std::vector<std::string> strs;

    const char c = '.';
    size_t off = 0;
    size_t pos = 0;
    //不加值验证(如存在连续的. 或者开始就是一个.)
    while (std::string::npos != (pos = key.find(c, off))) {
        strs.push_back(key.substr(off, pos - off));
        off = pos + 1;
    }

    if (off < key.size()) {
        strs.push_back(key.substr(off));
    }


    cJSON *node = json_;
    for (auto it: strs) {
        node = cJSON_GetObjectItem(node, it.c_str());
        if (!node) {
            node = nullptr;
            break;
        }
    }

    if (node) {
        cJSON *inner = cJSON_Duplicate(node, true);
        return JsonObject(inner);
    }

    return JsonObject();
}

bool JsonObject::isObject() const
{
    if (!json_) {
        return false;
    }

    return (bool)cJSON_IsObject(json_);
}

bool JsonObject::isArray() const
{
    if (!json_) {
        return false;
    }

    return (bool)cJSON_IsArray(json_);
}

bool JsonObject::isInt() const
{
    if (!json_) {
        return false;
    }
    return (bool)cJSON_IsNumber(json_);
}

bool JsonObject::isDouble() const
{
    if (!json_) {
        return false;
    }

    return (bool)cJSON_IsNumber(json_);
}

bool JsonObject::isString() const
{
    if (!json_) {
        return false;
    }

    return (bool)cJSON_IsString(json_);
}

bool JsonObject::isBool() const
{
    if (!json_) {
        return false;
    }

    return (bool)cJSON_IsBool(json_);
}

std::string JsonObject::toString() const
{
    if (!isString()) {
        return std::string();
    }

    return std::string(json_->valuestring);
}

int JsonObject::toInt() const
{
    if (!isInt()) {
        return 0;
    }

    return json_->valueint;
}

double JsonObject::toDouble() const
{
    if (!isDouble()) {
        return 0;
    }

    return json_->valuedouble;
}

bool JsonObject::toBool() const
{
    if (!isBool()) {
        return false;
    }

    return (bool)json_->valueint;
}

JsonArray JsonObject::toArray() const
{
    if (!isArray()) {
        return JsonArray();
    }

    cJSON *tmp = cJSON_Duplicate(json_, cJSON_bool(true));

    return JsonArray(tmp);
}

JsonArray JsonObject::transferToArray()
{
    if (!isArray()) {
        return JsonArray();
    }

    cJSON *mid =  json_;
    json_ = nullptr;

    return JsonArray(mid);
}

void JsonObject::clean()
{
    if (json_) {
        cJSON_Delete(json_);
        json_ = nullptr;
    }
}


JsonArray::JsonArray()
{
    json_ = nullptr;
}

JsonArray::JsonArray(cJSON *json)
{
    json_ = json;
}

std::string JsonArray::toJson() const
{
    if (!json_) {
        return std::string();
    }

    char *data = cJSON_Print(json_);
    std::string ret(data);

    if (data) {
        cJSON_free(data);
        data = nullptr;
    }

    return std::move(ret);
}

int JsonArray::getSize()
{
    if (!json_) {
        return 0;
    }

    return cJSON_GetArraySize(json_);
}

JsonObject JsonArray::operator[](int index)
{
    if (!json_) {
        return JsonObject();
    }

    if (index < 0 || index >= getSize()) {
        return JsonObject();
    }

    cJSON *item = cJSON_GetArrayItem(json_, index);

    if (!item) {
        return JsonObject();
    }

    cJSON *copy = cJSON_Duplicate(item, true);
    return JsonObject(copy);
}

JsonObject JsonArray::toObject() const
{
    if (!json_) {
        return JsonObject();
    }

    cJSON *copy = cJSON_Duplicate(json_, true);
    return JsonObject(copy);
}

JsonObject JsonArray::transferToObject()
{
    if (!json_) {
        return JsonObject();
    }
    cJSON *mid = json_;
    json_ = nullptr;

    return JsonObject(mid);
}