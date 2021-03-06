// https://www.cnblogs.com/lrxing/p/5535601.html
#include "base64.h"
/*base_64.cpp?ļ?*/
#include <string>
#include <cstring>
#include <basetsd.h>

std::string Base64::Encode(const unsigned char* str, size_t bytes) {
    //int num = 0, bin = 0, i;
    std::string _encode_result;
    const unsigned char* current = str;
    while (bytes > 2) {
        _encode_result += _base64_table[current[0] >> 2];
        _encode_result += _base64_table[(((size_t)current[0] & 0x03) << 4) + (current[1] >> 4)];
        _encode_result += _base64_table[(((size_t)current[1] & 0x0f) << 2) + (current[2] >> 6)];
        _encode_result += _base64_table[current[2] & 0x3f];

        current += 3;
        bytes -= 3;
    }
    if (bytes > 0)
    {
        _encode_result += _base64_table[current[0] >> 2];
        if (bytes % 3 == 1) {
            _encode_result += _base64_table[((size_t)current[0] & 0x03) << 4];
            _encode_result += "==";
        }
        else if (bytes % 3 == 2) {
            _encode_result += _base64_table[(((size_t)current[0] & 0x03) << 4) + (current[1] >> 4)];
            _encode_result += _base64_table[((size_t)current[1] & 0x0f) << 2];
            _encode_result += "=";
        }
    }
    return _encode_result;
}
std::string Base64::Decode(const char* str, size_t length) {
    //??????
    static const char DecodeTable[] {
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
        -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
        -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
    };
    size_t i = 0, pos = 0;
    std::string _decode_result;
    const char* current = str;
    signed char ch = 0;
    char bin = 0;
    while ((ch = *current++) != '\0' && length-- > 0) {
        if (ch == base64_pad) { // ??ǰһ???ַ??ǡ?=????
            /*
            ??˵??һ??????ڽ???ʱ??4???ַ?Ϊһ??????һ???ַ?ƥ?䡣
            ??????????
                1??????ĳһ??ƥ???ĵڶ????ǡ?=???ҵ??????ַ????ǡ?=????˵?????????????ַ??????Ϸ???ֱ?ӷ??ؿ?
                2????????ǰ??=?????ǵڶ????ַ????Һ??????ַ?ֻ?????հ׷?????˵???????????????Ϸ??????Լ?????
            */
            if (*current != '=' && (i % 4) == 1) {
                return "";
            }
            continue;
        }
        ch = DecodeTable[ch];
        //????????Ҫ?????????????в??Ϸ????ַ?
        if (ch < 0) { /* a space or some other separator character, we simply skip over */
            continue;
        }
        switch (i % 4)
        {
        case 0:
            bin = (size_t)ch << 2;
            break;
        case 1:
            bin |= ch >> 4;
            _decode_result += bin;
            bin = size_t(ch & 0x0f) << 4;
            break;
        case 2:
            bin |= ch >> 2;
            _decode_result += bin;
            bin = size_t(ch & 0x03) << 6;
            break;
        case 3:
            bin |= ch;
            _decode_result += bin;
            break;
        }
        i++;
    }
    return _decode_result;
}
