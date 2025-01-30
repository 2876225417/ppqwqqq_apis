
#ifndef IMAGE_RESOLVER_HPP
#define IMAGE_RESOLVER_HPP

#include "image.h"
#include <unordered_map>
#include <iostream>
#include <fstream>

image_format get_format_from_extension(std::string extension) {
    std::transform( extension.begin()
                  , extension.end()
                  , extension.begin()
                  , [](unsigned char c) { 
                    return std::tolower(c);
                  } );

    static const std::unordered_map<std::string, image_format> format_map = {
        {"jpg",  image_format::JPEG},
        {"jpeg", image_format::JPEG},
        {"png",  image_format::PNG},
        {"gif",  image_format::GIF},
        {"webp", image_format::WEBP}
    };
    auto it = format_map.find(extension);
    return (it != format_map.end()) ? it->second : image_format::UNKNOWN;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



image load_image_from_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file) 
        throw std::runtime_error("Failed to load image file");
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size))
        throw std::runtime_error("Failed to load image file");

    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory( reinterpret_cast<unsigned char*>(buffer.data())
                                                     , buffer.size()
                                                     , &width
                                                     , &height
                                                     , &channels
                                                     , 0
                                                     ) ;

    if (image_data == nullptr)
        throw std::runtime_error("Failed to decode image data");

    size_t last_slash = filepath.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? filepath : filepath.substr(last_slash + 1);
    size_t dot_pos = filename.find_last_of('.');
    std::string extension = (dot_pos == std::string::npos) ? "" : filename.substr(dot_pos + 1);

    image_format format = get_format_from_extension(extension);

    image img(filename, format, std::move(buffer));
    img.set_dimensions(width, height);

    stbi_image_free(image_data);

    return img;
}



#endif
