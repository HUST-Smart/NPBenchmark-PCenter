////////////////////////////////
/// usage : 1.	data format converters.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_P_CENTER_PB_READER_H
#define SMART_SZX_P_CENTER_PB_READER_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#pragma warning(push, 0)
#include "google/protobuf/util/json_util.h"
#pragma warning(pop)


namespace pb {

template<typename T>
std::string protobufToJson(const T &obj, bool pretty = true) {
    std::string data;

    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = pretty;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;

    google::protobuf::util::MessageToJsonString(obj, &data, options);

    return data;
}

template<typename T>
void jsonToProtobuf(const std::string &data, T &obj) {
    google::protobuf::util::JsonParseOptions options;
    google::protobuf::util::JsonStringToMessage(data, &obj, options);
}

template<typename T>
static bool load(const std::string &path, T &obj) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) { return false; }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    jsonToProtobuf(oss.str(), obj);
    return true;
}

template<typename T>
static bool save(const std::string &path, const T &obj) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) { return false; }
    ofs << protobufToJson(obj);
    return true;
}

}


#endif // SMART_SZX_P_CENTER_PB_READER_H
