// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

//taken from muduo and modified

/**
 *
 *  @file
 *  @author An Tao
 *  @section LICENSE
 *
 *  Copyright 2018, An Tao.  All rights reserved.
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  @section DESCRIPTION
 *
 */

#pragma once

#include <drogon/HttpResponse.h>
#include <trantor/utils/MsgBuffer.h>
#include <trantor/net/InetAddress.h>
#include <trantor/utils/Date.h>
#include <drogon/utils/Utilities.h>
#include <map>
#include <string>
#include <memory>

using namespace trantor;
namespace drogon
{
class HttpResponseImpl : public HttpResponse
{
    friend class HttpContext;

  public:
    explicit HttpResponseImpl()
        : _statusCode(kUnknown),
          _closeConnection(false),
          _left_body_length(0),
          _current_chunk_length(0)
    {
    }
    virtual HttpStatusCode statusCode() override
    {
        return _statusCode;
    }
    virtual void setStatusCode(HttpStatusCode code) override
    {
        _statusCode = code;
        setStatusMessage(web_response_code_to_string(code));
    }

    virtual void setStatusCode(HttpStatusCode code, const std::string &status_message) override
    {
        _statusCode = code;
        setStatusMessage(status_message);
    }

    virtual void setVersion(const Version v) override
    {
        _v = v;
    }

    virtual void setCloseConnection(bool on) override
    {
        _closeConnection = on;
    }

    virtual bool closeConnection() const override
    {
        return _closeConnection;
    }

    virtual void setContentTypeCode(uint8_t type) override
    {
        _contentType = type;
        setContentType(web_content_type_to_string(type));
    }

    virtual void setContentTypeCodeAndCharacterSet(uint8_t type, const std::string charSet = "utf-8") override
    {
        _contentType = type;
        setContentType(web_content_type_and_charset_to_string(type, charSet));
    }

    virtual uint8_t getContentTypeCode() override
    {
        return _contentType;
    }
    //        virtual uint8_t contentTypeCode() override
    //        {
    //            return _contentType;
    //        }

    virtual std::string getHeader(const std::string &key) const override
    {
        auto field = key;
        transform(field.begin(), field.end(), field.begin(), ::tolower);
        auto iter = _headers.find(field);
        if (iter == _headers.end())
        {
            return "";
        }
        else
        {
            return iter->second;
        }
    }
    virtual void addHeader(const std::string &key, const std::string &value) override
    {
        _fullHeaderString.reset();
        auto field = key;
        transform(field.begin(), field.end(), field.begin(), ::tolower);
        _headers[field] = value;
    }

    virtual void addHeader(const std::string &key, std::string &&value) override
    {
        _fullHeaderString.reset();
        auto field = key;
        transform(field.begin(), field.end(), field.begin(), ::tolower);
        _headers[field] = std::move(value);
    }

    virtual void addHeader(const char *start, const char *colon, const char *end) override
    {
        _fullHeaderString.reset();
        std::string field(start, colon);
        transform(field.begin(), field.end(), field.begin(), ::tolower);
        ++colon;
        while (colon < end && isspace(*colon))
        {
            ++colon;
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size() - 1]))
        {
            value.resize(value.size() - 1);
        }
        _headers[field] = value;

        //FIXME:reponse cookie should be "Set-Cookie:...."
        if (field == "cookie")
        {
            //LOG_INFO<<"cookies!!!:"<<value;
            std::string::size_type pos;
            while ((pos = value.find(";")) != std::string::npos)
            {
                std::string coo = value.substr(0, pos);
                auto epos = coo.find("=");
                if (epos != std::string::npos)
                {
                    std::string cookie_name = coo.substr(0, epos);
                    std::string::size_type cpos = 0;
                    while (cpos < cookie_name.length() && isspace(cookie_name[cpos]))
                        cpos++;
                    cookie_name = cookie_name.substr(cpos);
                    std::string cookie_value = coo.substr(epos + 1);
                    _cookies.insert(std::make_pair(cookie_name, Cookie(cookie_name, cookie_value)));
                }
                value = value.substr(pos + 1);
            }
            if (value.length() > 0)
            {
                std::string &coo = value;
                auto epos = coo.find("=");
                if (epos != std::string::npos)
                {
                    std::string cookie_name = coo.substr(0, epos);
                    std::string::size_type cpos = 0;
                    while (cpos < cookie_name.length() && isspace(cookie_name[cpos]))
                        cpos++;
                    cookie_name = cookie_name.substr(cpos);
                    std::string cookie_value = coo.substr(epos + 1);
                    _cookies.insert(std::make_pair(cookie_name, Cookie(cookie_name, cookie_value)));
                }
            }
        }
    }

    virtual void addCookie(const std::string &key, const std::string &value) override
    {
        _cookies.insert(std::make_pair(key, Cookie(key, value)));
    }

    virtual void addCookie(const Cookie &cookie) override
    {
        _cookies.insert(std::make_pair(cookie.key(), cookie));
    }

    virtual void removeCookie(const std::string &key) override
    {
        _cookies.erase(key);
    }

    virtual void setBody(const std::string &body) override
    {
        _body = body;
    }
    virtual void setBody(std::string &&body) override
    {
        _body = std::move(body);
    }

    virtual void redirect(const std::string &url) override
    {
        _headers["Location"] = url;
    }
    void appendToBuffer(MsgBuffer *output) const;

    virtual void clear() override
    {
        _statusCode = kUnknown;
        _v = kHttp11;
        _statusMessage.clear();
        _fullHeaderString.reset();
        _headers.clear();
        _cookies.clear();
        _body.clear();
        _left_body_length = 0;
        _current_chunk_length = 0;
    }

    virtual void setExpiredTime(ssize_t expiredTime) override
    {
        _expriedTime = expiredTime;
    }

    virtual ssize_t expiredTime() const override { return _expriedTime; }

    //	void setReceiveTime(trantor::Date t)
    //    {
    //        receiveTime_ = t;
    //    }

    virtual const std::string &getBody() const override
    {
        return _body;
    }
    virtual std::string &getBody() override
    {
        return _body;
    }
    void swap(HttpResponseImpl &that)
    {
        _headers.swap(that._headers);
        _cookies.swap(that._cookies);
        std::swap(_statusCode, that._statusCode);
        std::swap(_v, that._v);
        _statusMessage.swap(that._statusMessage);
        std::swap(_closeConnection, that._closeConnection);
        _body.swap(that._body);
        std::swap(_left_body_length, that._left_body_length);
        std::swap(_current_chunk_length, that._current_chunk_length);
        std::swap(_contentType, that._contentType);
        _jsonPtr.swap(that._jsonPtr);
        _fullHeaderString.swap(that._fullHeaderString);
    }
    void parseJson() const
    {
        //parse json data in reponse
        _jsonPtr = std::make_shared<Json::Value>();
        Json::CharReaderBuilder builder;
        builder["collectComments"] = false;
        JSONCPP_STRING errs;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (!reader->parse(_body.data(), _body.data() + _body.size(), _jsonPtr.get(), &errs))
        {
            LOG_ERROR << errs;
            _jsonPtr.reset();
        }
    }
    virtual const std::shared_ptr<Json::Value> getJsonObject() const override
    {
        if (!_jsonPtr)
        {
            parseJson();
        }
        return _jsonPtr;
    }
    const std::string &sendfileName() const
    {
        return _sendfileName;
    }
    void setSendfile(const std::string &filename)
    {
        _sendfileName = filename;
    }
    void makeHeaderString()
    {
        trantor::MsgBuffer buf;
        makeHeaderString(&buf);
        _fullHeaderString = std::make_shared<std::string>(buf.peek(), buf.readableBytes());
    }

  protected:
    static std::string web_content_type_to_string(uint8_t contenttype);
    static const std::string web_content_type_and_charset_to_string(uint8_t contenttype,
                                                                    const std::string &charSet);

    static std::string web_response_code_to_string(int code);

    void makeHeaderString(MsgBuffer *output) const;

  private:
    std::map<std::string, std::string> _headers;
    std::map<std::string, Cookie> _cookies;
    HttpStatusCode _statusCode;
    // FIXME: add http version
    Version _v;
    std::string _statusMessage;
    bool _closeConnection;
    std::string _body;
    size_t _left_body_length;
    size_t _current_chunk_length;
    uint8_t _contentType = CT_TEXT_HTML;

    ssize_t _expriedTime = -1;
    std::string _sendfileName;
    mutable std::shared_ptr<Json::Value> _jsonPtr;

    std::shared_ptr<std::string> _fullHeaderString;
    //trantor::Date receiveTime_;

    void setContentType(const std::string &contentType)
    {
        addHeader("Content-Type", contentType);
    }
    void setContentType(std::string &&contentType)
    {
        addHeader("Content-Type", std::move(contentType));
    }
    void setStatusMessage(const std::string &message)
    {
        _statusMessage = message;
    }
    void setStatusMessage(std::string &&message)
    {
        _statusMessage = std::move(message);
    }
};
typedef std::shared_ptr<HttpResponseImpl> HttpResponseImplPtr;
} // namespace drogon
