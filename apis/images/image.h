#ifndef IMAGE_H
#define IMAGE_H


enum class image_format {
    JPEG,
    PNG,
    GIF,
    WEBP,
    UNKNOWN
};

#include <string>
#include <vector>
#include "image_resolver.hpp"

struct image {
    std::string name;
    image_format format;
    std::vector<char> data;
    int width;
    int height;

    image( const std::string& name
        , image_format format
        , const std::vector<char>& data
        ):name(name)
        , format(format)
        , data(std::move(data))
        , width(0)
        , height(0) { }
    
        void set_dimensions(int w, int h) {
            width = w;
            height = h;
        }

        std::string get_MIME_type() const {
            switch (format) {
                case image_format::JPEG: return "image/jpeg";
                case image_format::PNG:  return "image/png";
                case image_format::GIF:  return "image/gif";
                case image_format::WEBP: return "image/webp";
                default:                 return "application/octet-stream";
            }
        }
};



#endif