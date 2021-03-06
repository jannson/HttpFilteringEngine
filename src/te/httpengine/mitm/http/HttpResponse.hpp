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

#pragma once

#include "BaseHttpTransaction.hpp"

namespace te
{
	namespace httpengine
	{
		namespace mitm
		{
			namespace http
			{

				/// <summary>
				/// The HttpResponse class manages the response side of HTTP transactions. The
				/// response class differs from the HttpRequest class only in providing additional
				/// methods for fetching and manipulating data specific to HTTP responses, such as
				/// the status code of the response.
				/// </summary>
				class HttpResponse : public BaseHttpTransaction
				{

				public:

					/// <summary>
					/// Default constructor. Initializes the internal http_parser object which is
					/// used for accurately parsing request headers. The body/payload methods of the
					/// http_parser callbacks are largely ignored, as the HttpResponse already holds
					/// the payoad buffer.
					/// </summary>
					HttpResponse();

					/// <summary>
					/// Default destructor. Frees the internal http_parser.
					/// </summary>
					virtual ~HttpResponse();

					/// <summary>
					/// Gets the status code of the response. 
					/// </summary>
					/// <returns>
					/// The status code of the response, if available. May return a zero value,
					/// which simply indicates that the status has not been extracted from any
					/// transaction headers.
					/// </returns>
					const uint16_t StatusCode() const;

					/// <summary>
					/// Sets the status code of the response. Setting the status code also
					/// internally sets the correct status message string. As such, passing an
					/// invalid status code will result in undefined behaviour, which will most
					/// likely result in a broken and failed transaction. Originally, throwing an
					/// exception was considered, however, given the known cases where even large
					/// companies decide to invent their own status codes and messages (like twitter
					/// 420 Enhance Your Calm for limiting clients making too many requests), the
					/// idea of throwing exceptions was abandoned, thus allowing the user to make
					/// changes which may or may not result in bugs.
					/// 
					/// Because this is a convenience function for avoiding having to correctly
					/// write out standard-defined status codes, please do not use this method for
					/// injecting your own extra special hipster status codes. If you must, do so
					/// using the StatusString(std::string&) method, ensuring that you include code
					/// and message proper, as well as the correct HTTP version (if this doesn't 
					/// already seem like a bad idea, it should start to now).
					/// </summary>
					/// <param name="code">
					/// The standard-defined, valid HTTP status code to set for the response. 
					/// </param>
					void StatusCode(const uint16_t code);

					/// <summary> 
					/// The status string for the response. Note that while a setter
					/// method is provided, one should use StatusCode(const uint16_t&) to minimize
					/// the risk of user mistakes such as mismatching descriptions to codes, typeos,
					/// etc.
					/// 
					/// StatusCode(const uint16_t&) automatically internally sets this string to the
					/// correct value based on the provided code, if the code is legal. 
					/// </summary>
					/// <returns> 
					/// The full status string, including the string representation of the
					/// HTTP status code. 
					/// </returns>
					const std::string& StatusString() const;

					/// <summary> 
					/// Setter to manually build the status code of the response. It is
					/// not advised to use this, but rather to use StatusCode(const uint16_t&), as
					/// it is designed both for convenience and to ensure that the string is
					/// entirely built correctly based on the known HTTP protocol version, the
					/// proper message associated with the code, etc. 
					/// 
					/// Use only if you're a hipster and well defined HTTP status codes are too
					/// mainstream for you. Use at your own risk, etc.
					/// </summary> 
					/// <param name="status">
					/// The complete status message for the response.
					/// </param>
					void StatusString(const std::string& status);

					/// <summary>
					/// Convenience function for formatting the transaction headers into a
					/// std::string container.
					/// </summary>
					/// <returns>
					/// std::string populated with the complete, formatted transaction 
					/// headers.
					/// </returns>
					virtual std::string HeadersToString() const;

					/// <summary> 
					/// Convenience function for formatting the transaction headers into a
					/// std::vector char container. 
					/// </summary> 
					/// <returns> 
					/// std::vector char populated with the complete formatted transaction 
					/// headers. 
					/// </returns>
					virtual std::vector<char> HeadersToVector() const;

				protected:

					/// <summary>
					/// The status code for the response.
					/// </summary>
					uint16_t m_statusCode;

					/// <summary>
					/// The status string for the response. Note that this contains the full status
					/// message, including the code in string format.
					/// </summary>
					std::string m_statusString;

					/// <summary>
					/// Called when the status read been completed by the http_parser.
					/// </summary>
					/// <param name="parser">
					/// The http_parser, returned in the callback for establishing context, since
					/// the callback is static. A pointer to the transaction object is held in
					/// parser->data.
					/// </param>
					/// <param name="at">
					/// A pointer to the position in the data buffer where the data begins. 
					/// </param>
					/// <param name="length">
					/// The length of the data in the buffer. 
					/// </param>
					/// <returns>
					/// The parsing status determined within the callback. Callbacks must return 0
					/// on success. Returning a non-zero value indicates error to the parser, making
					/// it exit immediately.
					/// </returns>
					static int OnStatus(http_parser* parser, const char *at, size_t length);

					/// <summary>
					/// Method for interally constructing the entire status code string, based on
					/// the supplied status code.
					/// </summary>
					/// <param name="code">
					/// The legal/defined HTTP Status Code. 
					/// </param>
					/// <returns>
					/// The string message description to accompany the Http Status Code. 
					/// </returns>
					std::string StatusCodeToMessage(const uint16_t& code) const;

				};

			} /* namespace http */
		} /* namespace mitm */
	} /* namespace httpengine */
} /* namespace te */