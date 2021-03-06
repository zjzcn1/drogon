//this file is generated by program automatically,don't modify it!
#include <drogon/NotFound.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>
using namespace std;
HttpResponsePtr NotFound::genHttpResponse(const HttpViewData &NotFound_view_data)
{
	std::stringstream NotFound_tmp_stream;
	NotFound_tmp_stream << "<html>\n";
	NotFound_tmp_stream << "<head><title>404 Not Found</title></head>\n";
	NotFound_tmp_stream << "<body bgcolor=\"white\">\n";
	NotFound_tmp_stream << "<center><h1>404 Not Found</h1></center>\n";
	NotFound_tmp_stream << "<hr><center>drogon/";
	NotFound_tmp_stream << NotFound_view_data.get<std::string>("version");
	NotFound_tmp_stream << "</center>\n";
	NotFound_tmp_stream << "</body>\n";
	NotFound_tmp_stream << "</html>\n";
	NotFound_tmp_stream << "<!-- a padding to disable MSIE and Chrome friendly error page -->\n";
	NotFound_tmp_stream << "<!-- a padding to disable MSIE and Chrome friendly error page -->\n";
	NotFound_tmp_stream << "<!-- a padding to disable MSIE and Chrome friendly error page -->\n";
	NotFound_tmp_stream << "<!-- a padding to disable MSIE and Chrome friendly error page -->\n";
	NotFound_tmp_stream << "<!-- a padding to disable MSIE and Chrome friendly error page -->\n";
	NotFound_tmp_stream << "<!-- a padding to disable MSIE and Chrome friendly error page -->\n";
	auto res = HttpResponse::newHttpResponse();
	res->setStatusCode(HttpResponse::k200OK);
#ifdef CONTENT_TYPE
	res->setContentTypeCode(CONTENT_TYPE);
#else
	res->setContentTypeCode(CT_TEXT_HTML);
#endif
	res->setBody(NotFound_tmp_stream.str().c_str());
	return res;
}
