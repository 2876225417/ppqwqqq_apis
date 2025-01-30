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
#include "include/image_resolver.hpp"

struct image {
    std::string name;
    image_format format;
    std::vector<char> data;
    int width;
    int height;

    image( const std::string& name
         , image_format format
         , const std::vector<char>& data
         ) ;
    
    void set_dimensions(int w, int h);

    std::string get_MIME_type() const;
};



#endif