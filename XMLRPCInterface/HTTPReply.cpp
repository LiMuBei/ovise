/* HTTP reply, adapted from the original Boost HTTP-Server3 example
 * http://www.boost.org/doc/libs/1_42_0/doc/html/boost_asio/examples.html#boost_asio.examples.http_server_3
*/

// taken and adapt from sharome XMLRPCInterface

#include "HTTPReply.h"
#include <boost/lexical_cast.hpp>

namespace StatusStrings {

const std::string ok =
  "HTTP/1.0 200 OK\r\n";
const std::string created =
  "HTTP/1.0 201 Created\r\n";
const std::string accepted =
  "HTTP/1.0 202 Accepted\r\n";
const std::string no_content =
  "HTTP/1.0 204 No Content\r\n";
const std::string multiple_choices =
  "HTTP/1.0 300 Multiple Choices\r\n";
const std::string moved_permanently =
  "HTTP/1.0 301 Moved Permanently\r\n";
const std::string moved_temporarily =
  "HTTP/1.0 302 Moved Temporarily\r\n";
const std::string not_modified =
  "HTTP/1.0 304 Not Modified\r\n";
const std::string bad_request =
  "HTTP/1.0 400 Bad Request\r\n";
const std::string unauthorized =
  "HTTP/1.0 401 Unauthorized\r\n";
const std::string forbidden =
  "HTTP/1.0 403 Forbidden\r\n";
const std::string not_found =
  "HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
  "HTTP/1.0 500 Internal Server Error\r\n";
const std::string not_implemented =
  "HTTP/1.0 501 Not Implemented\r\n";
const std::string bad_gateway =
  "HTTP/1.0 502 Bad Gateway\r\n";
const std::string service_unavailable =
  "HTTP/1.0 503 Service Unavailable\r\n";

boost::asio::const_buffer ToBuffer(HTTPReply::StatusType status)
{
  switch (status)
  {
  case HTTPReply::ok:
    return boost::asio::buffer(ok);
  case HTTPReply::created:
    return boost::asio::buffer(created);
  case HTTPReply::accepted:
    return boost::asio::buffer(accepted);
  case HTTPReply::no_content:
    return boost::asio::buffer(no_content);
  case HTTPReply::multiple_choices:
    return boost::asio::buffer(multiple_choices);
  case HTTPReply::moved_permanently:
    return boost::asio::buffer(moved_permanently);
  case HTTPReply::moved_temporarily:
    return boost::asio::buffer(moved_temporarily);
  case HTTPReply::not_modified:
    return boost::asio::buffer(not_modified);
  case HTTPReply::bad_request:
    return boost::asio::buffer(bad_request);
  case HTTPReply::unauthorized:
    return boost::asio::buffer(unauthorized);
  case HTTPReply::forbidden:
    return boost::asio::buffer(forbidden);
  case HTTPReply::not_found:
    return boost::asio::buffer(not_found);
  case HTTPReply::internal_server_error:
    return boost::asio::buffer(internal_server_error);
  case HTTPReply::not_implemented:
    return boost::asio::buffer(not_implemented);
  case HTTPReply::bad_gateway:
    return boost::asio::buffer(bad_gateway);
  case HTTPReply::service_unavailable:
    return boost::asio::buffer(service_unavailable);
  default:
    return boost::asio::buffer(internal_server_error);
  }
}

} // namespace status_strings

namespace MiscStrings {

const char NameValueSeparator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

} // namespace misc_strings

std::vector<boost::asio::const_buffer> HTTPReply::ToBuffers()
{
  std::vector<boost::asio::const_buffer> Buffers;
  Buffers.push_back(StatusStrings::ToBuffer(Status));
  for (std::size_t i = 0; i < Headers.size(); ++i)
  {
    HTTPHeader& h = Headers[i];
    Buffers.push_back(boost::asio::buffer(h.Name));
    Buffers.push_back(boost::asio::buffer(MiscStrings::NameValueSeparator));
    Buffers.push_back(boost::asio::buffer(h.Value));
    Buffers.push_back(boost::asio::buffer(MiscStrings::crlf));
  }
  Buffers.push_back(boost::asio::buffer(MiscStrings::crlf));
  Buffers.push_back(boost::asio::buffer(Content));
  return Buffers;
}

namespace StockReplies {

const char ok[] = "";
const char created[] =
  "<html>"
  "<head><title>Created</title></head>"
  "<body><h1>201 Created</h1></body>"
  "</html>";
const char accepted[] =
  "<html>"
  "<head><title>Accepted</title></head>"
  "<body><h1>202 Accepted</h1></body>"
  "</html>";
const char no_content[] =
  "<html>"
  "<head><title>No Content</title></head>"
  "<body><h1>204 Content</h1></body>"
  "</html>";
const char multiple_choices[] =
  "<html>"
  "<head><title>Multiple Choices</title></head>"
  "<body><h1>300 Multiple Choices</h1></body>"
  "</html>";
const char moved_permanently[] =
  "<html>"
  "<head><title>Moved Permanently</title></head>"
  "<body><h1>301 Moved Permanently</h1></body>"
  "</html>";
const char moved_temporarily[] =
  "<html>"
  "<head><title>Moved Temporarily</title></head>"
  "<body><h1>302 Moved Temporarily</h1></body>"
  "</html>";
const char not_modified[] =
  "<html>"
  "<head><title>Not Modified</title></head>"
  "<body><h1>304 Not Modified</h1></body>"
  "</html>";
const char bad_request[] =
  "<html>"
  "<head><title>Bad Request</title></head>"
  "<body><h1>400 Bad Request</h1></body>"
  "</html>";
const char unauthorized[] =
  "<html>"
  "<head><title>Unauthorized</title></head>"
  "<body><h1>401 Unauthorized</h1></body>"
  "</html>";
const char forbidden[] =
  "<html>"
  "<head><title>Forbidden</title></head>"
  "<body><h1>403 Forbidden</h1></body>"
  "</html>";
const char not_found[] =
  "<html>"
  "<head><title>Not Found</title></head>"
  "<body><h1>404 Not Found</h1></body>"
  "</html>";
const char internal_server_error[] =
  "<html>"
  "<head><title>Internal Server Error</title></head>"
  "<body><h1>500 Internal Server Error</h1></body>"
  "</html>";
const char not_implemented[] =
  "<html>"
  "<head><title>Not Implemented</title></head>"
  "<body><h1>501 Not Implemented</h1></body>"
  "</html>";
const char bad_gateway[] =
  "<html>"
  "<head><title>Bad Gateway</title></head>"
  "<body><h1>502 Bad Gateway</h1></body>"
  "</html>";
const char service_unavailable[] =
  "<html>"
  "<head><title>Service Unavailable</title></head>"
  "<body><h1>503 Service Unavailable</h1></body>"
  "</html>";

std::string ToString(HTTPReply::StatusType status)
{
  switch (status)
  {
  case HTTPReply::ok:
    return ok;
  case HTTPReply::created:
    return created;
  case HTTPReply::accepted:
    return accepted;
  case HTTPReply::no_content:
    return no_content;
  case HTTPReply::multiple_choices:
    return multiple_choices;
  case HTTPReply::moved_permanently:
    return moved_permanently;
  case HTTPReply::moved_temporarily:
    return moved_temporarily;
  case HTTPReply::not_modified:
    return not_modified;
  case HTTPReply::bad_request:
    return bad_request;
  case HTTPReply::unauthorized:
    return unauthorized;
  case HTTPReply::forbidden:
    return forbidden;
  case HTTPReply::not_found:
    return not_found;
  case HTTPReply::internal_server_error:
    return internal_server_error;
  case HTTPReply::not_implemented:
    return not_implemented;
  case HTTPReply::bad_gateway:
    return bad_gateway;
  case HTTPReply::service_unavailable:
    return service_unavailable;
  default:
    return internal_server_error;
  }
}

} // namespace stock_replies

HTTPReply HTTPReply::StockReply(HTTPReply::StatusType status)
{
  HTTPReply rep;
  rep.Status = status;
  rep.Content = StockReplies::ToString(status);
  rep.Headers.resize(2);
  rep.Headers[0].Name = "Content-Length";
  rep.Headers[0].Value = boost::lexical_cast<std::string>(rep.Content.size());
  rep.Headers[1].Name = "Content-Type";
  rep.Headers[1].Value = "text/html";
  return rep;
}