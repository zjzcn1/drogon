#include <drogon/utils/Utilities.h>
#include <drogon/config.h>
#include <trantor/utils/Logger.h>
#include <string.h>
#include <zlib.h>
#include <uuid.h>
#include <stdio.h>
#include <stdarg.h>
namespace drogon
{
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

bool isInteger(const std::string &str)
{
    for (auto c : str)
    {
        if (c > '9' || c < '0')
            return false;
    }
    return true;
}
std::string genRandomString(int length)
{
    int i;
    char str[length + 1];

    timespec tp;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
    //LOG_INFO<<"time: "<<tp.tv_nsec;
    srand(static_cast<unsigned int>(tp.tv_nsec));
    std::string char_space = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (i = 0; i < length; i++)
    {
        str[i] = char_space[rand() % char_space.length()];
    }
    return std::string(str);
}

std::string stringToHex(unsigned char *ptr, long long length)
{
    std::string idString;
    for (long long i = 0; i < length; i++)
    {
        int value = (ptr[i] & 0xf0) >> 4;
        if (value < 10)
        {
            idString.append(1, char(value + 48));
        }
        else
        {
            idString.append(1, char(value + 55));
        }

        value = (ptr[i] & 0x0f);
        if (value < 10)
        {
            idString.append(1, char(value + 48));
        }
        else
        {
            idString.append(1, char(value + 55));
        }
    }
    return idString;
}
std::vector<std::string> splitString(const std::string &str, const std::string &separator)
{
    std::vector<std::string> ret;
    std::string::size_type pos1, pos2;
    pos2 = 0;
    pos1 = str.find(separator);
    while (pos1 != std::string::npos)
    {
        if (pos1 != 0)
        {
            std::string item = str.substr(pos2, pos1 - pos2);
            ret.push_back(item);
        }
        pos2 = pos1 + separator.length();
        while (pos2 < str.length() && str.substr(pos2, separator.length()) == separator)
            pos2 += separator.length();
        pos1 = str.find(separator, pos2);
    }
    if (pos2 < str.length())
        ret.push_back(str.substr(pos2));
    return ret;
}
std::string getuuid()
{
    uuid_t uu;
    uuid_generate(uu);
    return stringToHex(uu, 16);
}

std::string base64Encode(unsigned char const *bytes_to_encode, unsigned int in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

std::string base64Decode(std::string const &encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}

std::string urlDecode(const std::string &szToDecode)
{
    std::string result;
    result.reserve(szToDecode.length());
    int hex = 0;
    for (size_t i = 0; i < szToDecode.length(); ++i)
    {
        switch (szToDecode[i])
        {
        case '+':
            result += ' ';
            break;
        case '%':
            if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
            {
                std::string hexStr = szToDecode.substr(i + 1, 2);
                hex = strtol(hexStr.c_str(), 0, 16);
                //字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
                //可以不经过编码直接用于URL
                if (!((hex >= 48 && hex <= 57) ||  //0-9
                      (hex >= 97 && hex <= 122) || //a-z
                      (hex >= 65 && hex <= 90) ||  //A-Z
                      //一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
                      hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29 || hex == 0x2a || hex == 0x2b || hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f || hex == 0x3A || hex == 0x3B || hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f))
                {
                    result += char(hex);
                    i += 2;
                }
                else
                    result += '%';
            }
            else
            {
                result += '%';
            }
            break;
        default:
            result += szToDecode[i];
            break;
        }
    }
    return result;
}

/* Compress gzip data */
/* data 原数据 ndata 原数据长度 zdata 压缩后数据 nzdata 压缩后长度 */

int gzipCompress(const char *data, const size_t ndata,
                 char *zdata, size_t *nzdata)
{

    z_stream c_stream;
    int err = 0;

    if (data && ndata > 0)
    {
        c_stream.zalloc = NULL;
        c_stream.zfree = NULL;
        c_stream.opaque = NULL;
        //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
        if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                         MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            return -1;
        c_stream.next_in = (Bytef *)data;
        c_stream.avail_in = ndata;
        c_stream.next_out = (Bytef *)zdata;
        c_stream.avail_out = *nzdata;
        while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata)
        {
            if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK)
            {
                LOG_ERROR << "compress err:" << c_stream.msg;
                return -1;
            }
        }
        if (c_stream.avail_in != 0)
            return c_stream.avail_in;
        for (;;)
        {
            if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END)
                break;
            if (err != Z_OK)
                return -1;
        }
        if (deflateEnd(&c_stream) != Z_OK)
            return -1;
        *nzdata = c_stream.total_out;
        return 0;
    }
    return -1;
}

/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int gzipDecompress(const char *zdata, const size_t nzdata,
                   char *data, size_t *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = NULL;
    d_stream.zfree = NULL;
    d_stream.opaque = NULL;
    d_stream.next_in = (Bytef *)zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = (Bytef *)data;
    //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
    if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK)
        return -1;
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata)
    {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END)
            break;
        if (err != Z_OK)
        {
            if (err == Z_DATA_ERROR)
            {
                d_stream.next_in = (Bytef *)dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
                {
                    return -1;
                }
            }
            else
                return -1;
        }
    }
    if (inflateEnd(&d_stream) != Z_OK)
        return -1;
    *ndata = d_stream.total_out;
    return 0;
}
char *getHttpFullDate(const trantor::Date &date)
{
    //rfc2616-3.3.1
    //Full Date format like this:Sun, 06 Nov 1994 08:49:37 GMT
    //                           Wed, 12 Sep 2018 09:22:40 GMT
    static __thread int64_t lastSecond = 0;
    static __thread char lastTimeString[128] = {0};
    auto nowSecond = date.microSecondsSinceEpoch() / MICRO_SECONDS_PRE_SEC;
    if (nowSecond == lastSecond)
    {
        return lastTimeString;
    }
    lastSecond = nowSecond;
    date.toCustomedFormattedString("%a, %d %b %Y %T GMT", lastTimeString, sizeof(lastTimeString));
    return lastTimeString;
}
std::string formattedString(const char *format, ...)
{
    std::string strBuffer;
    strBuffer.resize(1024);
    va_list ap, backup_ap;
    va_start(ap, format);
    va_copy(backup_ap, ap);
    auto result = vsnprintf((char *)strBuffer.data(), strBuffer.size(), format, backup_ap);
    va_end(backup_ap);
    if ((result >= 0) && ((std::string::size_type)result < strBuffer.size()))
    {
        strBuffer.resize(result);
    }
    else
    {
        while (true)
        {
            if (result < 0)
            {
                // Older snprintf() behavior. Just try doubling the buffer size
                strBuffer.resize(strBuffer.size() * 2);
            }
            else
            {
                strBuffer.resize(result + 1);
            }

            va_copy(backup_ap, ap);
            auto result = vsnprintf((char *)strBuffer.data(), strBuffer.size(), format, backup_ap);
            va_end(backup_ap);

            if ((result >= 0) && ((std::string::size_type)result < strBuffer.size()))
            {
                strBuffer.resize(result);
                break;
            }
        }
    }
    va_end(ap);
    return strBuffer;
}
} // namespace drogon
