
#ifndef IMAGE_RESOLVER_H
#define IMAGE_RESOLVER_H

#include "image.h"
#include <unordered_map>
#include <iostream>
#include <fstream>

image_format get_format_from_extension(const std::string& extension) {
    static const std::unordered_map<std::string, image_format> format_map = {
        {"jpg",  image_format::JPEG},
        {"jpeg", image_format::JPEG},
        {"png",  image_format::PNG},
        {"gif",  image_format::GIF},
        {"webp", image_format::WEBP}
    };
    auto it = format_map.find(extension);
    if (it != format_map.end())
        return it->second;
    return image_format::UNKNOWN;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


image load_image_from_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file) 
        throw std::runtime_error("Failed to load image file!");

    // 获取文件大小
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) 
        throw std::runtime_error("Failed to read image file!");

    // 使用stb_image解析图片并获取宽度和高度
    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(buffer.data()), buffer.size(), &width, &height, &channels, 0);
    
    if (image_data == nullptr) {
        throw std::runtime_error("Failed to decode image data!");
    }

    // 获取文件名和扩展名
    size_t last_slash = filepath.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? filepath : filepath.substr(last_slash + 1);
    size_t dot_pos = filename.find_last_of('.');
    std::string extension = (dot_pos == std::string::npos) ? "" : filename.substr(dot_pos + 1);

    // 确定图片格式
    image_format format = get_format_from_extension(extension);

    // 创建 image 对象
    image img(filename, format, buffer);
    img.set_demensions(width, height);  // 设置图片宽高

    // 释放stbi的内存
    stbi_image_free(image_data);

    return img;
}



#endif
