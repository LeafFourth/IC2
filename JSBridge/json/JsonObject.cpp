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

    std::vector<std::string> strs = parseKey(key);

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

bool JsonObject::isEmpty() const
{
    return json_ == nullptr || cJSON_IsNull(json_);
}

bool JsonObject::isObject() const
{
    if (!json_) {
        return false;
    }

    return cJSON_IsObject(json_) != 0;
}

bool JsonObject::isArray() const
{
    if (!json_) {
        return false;
    }

    return cJSON_IsArray(json_) != 0;
}

bool JsonObject::isInt() const
{
    if (!json_) {
        return false;
    }
    return cJSON_IsNumber(json_) != 0;
}

bool JsonObject::isDouble() const
{
    if (!json_) {
        return false;
    }

    return cJSON_IsNumber(json_) != 0;
}

bool JsonObject::isString() const
{
    if (!json_) {
        return false;
    }

    return cJSON_IsString(json_) != 0;
}

bool JsonObject::isBool() const
{
    if (!json_) {
        return false;
    }

    return cJSON_IsBool(json_) != 0;
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

    return json_->valueint != 0;
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

void JsonObject::insert(const std::string &key, int val)
{
    std::vector<std::string> paths = parseKey(key);
    cJSON *parent = getInsertNode(paths);
    
    if (!parent) {
        return;
    }

    cJSON_AddNumberToObject(parent, paths[paths.size() - 1].c_str(), val);
}

void JsonObject::insert(const std::string &key, double val)
{
    std::vector<std::string> paths = parseKey(key);
    cJSON *parent = getInsertNode(paths);


    if (!parent) {
        return;
    }

    cJSON_AddNumberToObject(parent, paths[paths.size() - 1].c_str(), val);
}

void JsonObject::insert(const std::string &key, bool val)
{
    std::vector<std::string> paths = parseKey(key);
    cJSON *parent = getInsertNode(paths);


    if (!parent) {
        return;
    }
    cJSON_AddBoolToObject(parent, paths[paths.size() - 1].c_str(), cJSON_bool(val));
}

void JsonObject::insert(const std::string &key, const std::string &val)
{
    std::vector<std::string> paths = parseKey(key);
    cJSON *parent = getInsertNode(paths);


    if (!parent) {
        return;
    }
    cJSON_AddStringToObject(parent, paths[paths.size() - 1].c_str(), val.c_str());
}

void JsonObject::insert(const std::string &key, const JsonArray &val)
{
    std::vector<std::string> paths = parseKey(key);
    cJSON *parent = getInsertNode(paths);

    if (!parent) {
        return;
    }

    if (val.getSize() == 0) {
        cJSON_AddArrayToObject(parent, paths[paths.size() - 1].c_str());

        return;
    }

    JsonObject obj = val.toObject();
    cJSON_AddItemToObject(parent, paths[paths.size() - 1].c_str(), obj.take());
}

void JsonObject::insert(const std::string &key, const JsonObject &val)
{
    std::vector<std::string> paths = parseKey(key);
    cJSON *parent = getInsertNode(paths);

    if (!parent) {
        return;
    }

    if (val.isEmpty()) {
        cJSON_AddObjectToObject(parent, paths[paths.size() - 1].c_str());

        return;
    }

    cJSON_AddItemToObject(parent, paths[paths.size() - 1].c_str(), cJSON_Duplicate(val.json_, cJSON_bool(true)));
}

cJSON *JsonObject::take()
{
    cJSON *ret = json_;
    json_ = nullptr;
    return ret;
}

void JsonObject::clean()
{
    if (json_) {
        cJSON_Delete(json_);
        json_ = nullptr;
    }
}

void JsonObject::initOnNull()
{
    if (!json_) {
        json_ = cJSON_CreateObject();
    }
}

cJSON *JsonObject::getInsertNode(const std::vector<std::string> &paths)
{
    if (paths.size() == 0) {
        return nullptr;
    }

    initOnNull();

    if ((paths.size() < 2)) {
        return json_;
    }

    if (!isEmpty() && !isObject()) {
        return nullptr;
    }

    size_t pathsSize = paths.size();
    cJSON *sub = nullptr;
    cJSON *parent = json_;
    cJSON *insertNode = nullptr;
    const char* insertKey = nullptr;
    bool err = false;
    for (size_t i = 0; i < pathsSize - 1; ++i) {
        if (sub) {
            cJSON *newSub = cJSON_CreateObject();
            cJSON_AddItemToObject(parent, paths[i].c_str(), newSub);
            parent = newSub;
            continue;
        }

        cJSON *node = cJSON_GetObjectItem(parent, paths[i].c_str());

        if (node) {
            if (!cJSON_IsObject(node)) {
                err = true;
                break;
            }

            parent = node;
            
            continue;
        }

        sub = cJSON_CreateObject();
        insertKey = paths[i].c_str();
        insertNode = parent;
        parent = sub;
    }

    if (err) {
        if (sub) {
            cJSON_free(sub);
            sub = nullptr;
        }

        return nullptr;
    }
    
    if (sub) {
        cJSON_AddItemToObject(insertNode, insertKey, sub);
    }

    return parent;
}

//static
std::vector<std::string> JsonObject::parseKey(const std::string &key)
{
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

    return std::move(strs);
}


JsonArray::JsonArray()
{
    json_ = nullptr;
}

JsonArray::JsonArray(const std::string &array)
{
    json_ = cJSON_Parse(array.c_str());
    if (!cJSON_IsArray(json_)) {
        cJSON_free(json_);
        json_ = nullptr;
    }
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

int JsonArray::getSize() const
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

void JsonArray::insert(const std::string &key, const JsonObject &item)
{
    if (item.isEmpty()) {
        return;
    }

    JsonObject tmp = item;
    cJSON_AddItemToArray(json_, tmp.take());
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