
#include "image.h"

image::image( const std::string& name
            , image_format format
            , const std::vector<char>& data
            ):name(name)
            , format(format)
            , data(std::move(data))
            , width(0)
            , height(0) { }





void image::set_dimensions(int w, int h) {
    width = w;
    height = h;
}
        
std::string image::get_MIME_type() const {
    switch (format) {
        case image_format::JPEG: return "image/jpeg";
        case image_format::PNG:  return "image/png";
        case image_format::GIF:  return "image/gif";
        case image_format::WEBP: return "image/webp";
        default:                 return "application/octet-stream";
    }
}