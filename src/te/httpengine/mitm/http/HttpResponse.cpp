/*
* Copyright (c) 2016 Jesse Nicholson.
*
* This file is part of Http Filtering Engine.
*
* Http Filtering Engine is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or (at
* your option) any later version.
*
* In addition, as a special exception, the copyright holders give
* permission to link the code of portions of this program with the OpenSSL
* library.
*
* You must obey the GNU General Public License in all respects for all of
* the code used other than OpenSSL. If you modify file(s) with this
* exception, you may extend this exception to your version of the file(s),
* but you are not obligated to do so. If you do not wish to do so, delete
* this exception statement from your version. If you delete this exception
* statement from all source files in the program, then also delete it
* here.
*
* Http Filtering Engine is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with Http Filtering Engine. If not, see <http://www.gnu.org/licenses/>.
*/

#include "HttpResponse.hpp"

namespace te
{
	namespace httpengine
	{
		namespace mitm
		{
			namespace http
			{

				HttpResponse::HttpResponse()
				{
					m_httpParserSettings.on_status = &OnStatus;

					m_httpParser = static_cast<http_parser*>(malloc(sizeof(http_parser)));
					http_parser_init(m_httpParser, HTTP_RESPONSE);
					m_httpParser->data = this;
				}

				HttpResponse::~HttpResponse()
				{

				}

				const uint16_t HttpResponse::StatusCode() const
				{
					return m_statusCode;
				}

				void HttpResponse::StatusCode(const uint16_t code)
				{
					m_statusCode = code;

					m_statusString.clear();

					if (m_httpVersion == HttpProtocolVersion::HTTP1)
					{
						m_statusString.append(u8"HTTP/1.0 ");
					}
					else if (m_httpVersion == HttpProtocolVersion::HTTP1_1)
					{
						m_statusString.append(u8"HTTP/1.1 ");
					}
					else if (m_httpVersion == HttpProtocolVersion::HTTP2)
					{
						m_statusString.append(u8"HTTP/2.0 ");
					}

					m_statusString.append(std::to_string(code));
					m_statusString.append(u8" ").append(StatusCodeToMessage(code));
				}

				const std::string& HttpResponse::StatusString() const
				{
					return m_statusString;
				}

				void HttpResponse::StatusString(const std::string& status)
				{
					m_statusString = status;
				}

				std::string HttpResponse::HeadersToString() const
				{
					std::string ret;

					ret.append(m_statusString);					

					for (auto header = m_headers.begin(); header != m_headers.end(); ++header)
					{
						ret.append(u8"\r\n").append(header->first).append(u8": ").append(header->second);
					}

					ret.append(u8"\r\n\r\n");

					return ret;
				}

				std::vector<char> HttpResponse::HeadersToVector() const
				{
					std::string headersAsString = HeadersToString();

					return std::vector<char>(headersAsString.begin(), headersAsString.end());
				}

				int HttpResponse::OnStatus(http_parser* parser, const char *at, size_t length)
				{
					if (parser != nullptr)
					{
						HttpResponse* trans = static_cast<HttpResponse*>(parser->data);

						if (trans == nullptr)
						{
							return -1;
						}

						if (parser->http_major == 1)
						{
							if (parser->http_minor == 0)
							{
								trans->m_httpVersion = HttpProtocolVersion::HTTP1;
							}
							else
							{
								// assume 1.1
								trans->m_httpVersion = HttpProtocolVersion::HTTP1_1;
							}							
						}
						else if(parser->http_major == 2)
						{
							trans->m_httpVersion = HttpProtocolVersion::HTTP2;
						}
						else 
						{
							// assume 1.1
							trans->m_httpVersion = HttpProtocolVersion::HTTP1_1;
						}

						trans->StatusCode(parser->status_code);						
					}
					else {
						return -1;
					}

					return 0;
				}

				std::string HttpResponse::StatusCodeToMessage(const uint16_t& code) const
				{
					switch (code)
					{

					case 100:
						return std::string(u8"Continue");

					case 101:
						return std::string(u8"Switching Protocols");

					case 102:
						return std::string(u8"Processing");

					case 200:
						return std::string(u8"OK");

					case 201:
						return std::string(u8"Created");

					case 202:
						return std::string(u8"Accepted");

					case 203:
						return std::string(u8"Non-Authoritative Information");

					case 204:
						return std::string(u8"No Content");

					case 205:
						return std::string(u8"Reset Content");

					case 206:
						return std::string(u8"Partial Content");

					case 207:
						return std::string(u8"Multi-Status");

					case 208:
						return std::string(u8"Already Reported");

					case 226:
						return std::string(u8"IM Used");

					case 300:
						return std::string(u8"Multiple Choices");

					case 301:
						return std::string(u8"Moved Permanently");

					case 302:
						return std::string(u8"Found");

					case 303:
						return std::string(u8"See Other");

					case 304:
						return std::string(u8"Not Modified");

					case 305:
						return std::string(u8"Use Proxy");

					case 306:
						return std::string(u8"Switch Proxy");

					case 307:
						return std::string(u8"Temporary Redirect");

					case 308:
						return std::string(u8"Permanent Redirect");

					case 400:
						return std::string(u8"Bad Request");

					case 401:
						return std::string(u8"Unauthorized");

					case 402:
						return std::string(u8"Payment Required");

					case 403:
						return std::string(u8"Forbidden");

					case 404:
						return std::string(u8"Not Found");

					case 405:
						return std::string(u8"Method Not Allowed");

					case 406:
						return std::string(u8"Not Acceptable");

					case 407:
						return std::string(u8"Proxy Authentication Required");

					case 408:
						return std::string(u8"Request Timeout");

					case 409:
						return std::string(u8"Conflict");

					case 410:
						return std::string(u8"Gone");

					case 411:
						return std::string(u8"Length Required");

					case 412:
						return std::string(u8"Precondition Failed");

					case 413:
						return std::string(u8"Request Entity Too Large");

					case 414:
						return std::string(u8"Request-URI Too Long");

					case 415:
						return std::string(u8"Unsupported Media Type");

					case 416:
						return std::string(u8"Requested Range Not Satisfiable");

					case 417:
						return std::string(u8"Expectation Failed");

					case 418:
						return std::string(u8"I'm a teapot");

					case 419:
						return std::string(u8"Authentication Timeout");

					case 420:
						return std::string(u8"Method Failure");

					case 422:
						return std::string(u8"Unprocessable Entity");

					case 423:
						return std::string(u8"Locked");

					case 424:
						return std::string(u8"Failed Dependency");

					case 426:
						return std::string(u8"Upgrade Required");

					case 428:
						return std::string(u8"Precondition Required");

					case 429:
						return std::string(u8"Too Many Requests");

					case 431:
						return std::string(u8"Request Header Fields Too Large");

					case 440:
						return std::string(u8"Login Timeout");

					case 444:
						return std::string(u8"No Response");

					case 449:
						return std::string(u8"Retry With");

					case 450:
						return std::string(u8"Blocked by Windows Parental Controls");

					case 451:
						return std::string(u8"Unavailable For Legal Reasons");

					case 494:
						return std::string(u8"Request Header Too Large");

					case 495:
						return std::string(u8"Cert Error");

					case 496:
						return std::string(u8"No Cert");

					case 497:
						return std::string(u8"HTTP to HTTPS");

					case 498:
						return std::string(u8"Token expired/invalid");

					case 499:
						return std::string(u8"Client Closed Request");

					case 500:
						return std::string(u8"Internal Server Error");

					case 501:
						return std::string(u8"Not Implemented");

					case 502:
						return std::string(u8"Bad Gateway");

					case 503:
						return std::string(u8"Service Unavailable");

					case 504:
						return std::string(u8"Gateway Timeout");

					case 505:
						return std::string(u8"HTTP Version Not Supported");

					case 506:
						return std::string(u8"Variant Also Negotiates");

					case 508:
						return std::string(u8"Loop Detected");

					case 509:
						return std::string(u8"Bandwidth Limit Exceeded");

					case 510:
						return std::string(u8"Not Extended");

					case 598:
						return std::string(u8"Network read timeout error");

					case 599:
						return std::string(u8"Network connect timeout error");

					default:
						return std::string();
					}
				}

			} /* namespace http */
		} /* namespace mitm */
	} /* namespace httpengine */
} /* namespace te */