#include "utils.hpp"

#include <zlib/zlib.h>
#include <sstream>

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string urlToFilename(const std::string& url) {
    std::string filename;
    filename.reserve(url.size()); // reserve memory for efficiency
    
    // Define a whitelist of allowed characters
    const std::string whitelist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.";

    // Convert the URL to the filename, keeping only characters in the whitelist
    for (char ch : url) {
        if (whitelist.find(ch) != std::string::npos) {
            filename.push_back(ch);
        } else {
            // Replace invalid characters with underscores
            filename.push_back('_');
        }
    }
    
    return filename;
}


std::string getSubstringAfterSlash(const std::string& input) {
    size_t pos = input.find_last_of('/');
    if (pos != std::string::npos && pos != input.length() - 1) {
        return input.substr(pos + 1);
    } else {
        return input;
    }
}

std::string decompressGz(const std::vector<uint8_t>& compressed_data) {
    // Prepare output string
    std::string uncompressed_data;

    // Setup zlib stream
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = compressed_data.size();
    stream.next_in = const_cast<uint8_t*>(compressed_data.data());

    // Use zlib's inflateInit2 to specify gzip/zlib decoding
    if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK) {
        return uncompressed_data; // Error handling
    }

    // Inflate compressed data
    constexpr size_t chunk_size = 4096; // Adjust as needed
    uint8_t out[chunk_size];
    int ret;
    do {
        stream.avail_out = chunk_size;
        stream.next_out = out;
        ret = inflate(&stream, Z_NO_FLUSH);
        switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&stream);
                return uncompressed_data; // Error handling
        }
        size_t have = chunk_size - stream.avail_out;
        uncompressed_data.append(reinterpret_cast<char*>(out), have);
    } while (ret != Z_STREAM_END);

    // Clean up and return uncompressed data
    (void)inflateEnd(&stream);
    return uncompressed_data;
}
