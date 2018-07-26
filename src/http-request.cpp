/*******************************************************************************
 *
 *  BSD 2-Clause License
 *
 *  Copyright (c) 2017, Sandeep Prakash
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 * Copyright (c) 2017, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   http-request.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Oct 17, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <glog/logging.h>
#include "utils.hpp"
#include "http-client.hpp"
#include "http-connection.hpp"
#include "http-request.hpp"

namespace ChCppUtils {
namespace Http {
namespace Client {

HttpResponse::HttpResponse() {
	responseCode = 0;
	request = nullptr;
}

HttpResponse::~HttpResponse() {

}

HttpRequest* HttpResponse::getRequest()  {
	return request;
}

HttpResponse &HttpResponse::setRequest(HttpRequest* request) {
	this->request = request;
	return *this;
}

uint32_t HttpResponse::getResponseCode()  {
	return responseCode;
}

HttpResponse &HttpResponse::setResponseCode(uint32_t responseCode) {
	this->responseCode = responseCode;
	return *this;
}

string& HttpResponse::getResponseMime()  {
	return responseMime;
}

 HttpResponse &HttpResponse::setResponseMime(string responseMime) {
	this->responseMime = responseMime;
	return *this;
}

string& HttpResponse::getResponseText()  {
	return responseText;
}

bool HttpResponse::getResponseBody(uint8_t **body, uint32_t *length) {
   return request->getResponseBody(body, length);
}

HttpResponse &HttpResponse::setResponseText(string responseText) {
	this->responseText = responseText;
	return *this;
}


HttpRequestEvent::HttpRequestEvent(HttpResponse *response) {
	this->response = response;
}

HttpResponse *HttpRequestEvent::getResponse() {
	return response;
}

HttpRequestLoadEvent::HttpRequestLoadEvent(HttpResponse *response) :
	HttpRequestEvent(response){
}

HttpRequest::OnLoad::OnLoad() {
	this->onload = nullptr;
}

HttpRequest::OnLoad &HttpRequest::OnLoad::set(_OnLoad onload) {
	this->onload = onload;
	return *this;
}

void HttpRequest::OnLoad::bind(void *this_) {
	this->this_= this_;
}

void HttpRequest::OnLoad::fire(HttpResponse *response) {
	if(this->onload) {
		HttpRequestLoadEvent *event = new HttpRequestLoadEvent(response);
		this->onload(event, this->this_);
		delete event;
	}
}

HttpRequest::HttpRequest() {
	id = generateUUID();
	uri = nullptr;
	context = new RequestContext();
	method = EVHTTP_REQ_GET;
	responseCode = 0;
}

HttpRequest::~HttpRequest() {
	if(uri) evhttp_uri_free(uri);
	delete context;
}

HttpRequest::OnLoad &HttpRequest::onLoad(_OnLoad onload) {
	return this->onload.set(onload);
}

void HttpRequest::_evHttpReqDone(struct evhttp_request *req, void *arg) {
	HttpRequest *this_ = (HttpRequest *) arg;
	this_->evHttpReqDone(req);
}

void HttpRequest::evHttpReqDone(struct evhttp_request *req) {
	end = getEpochNano();
	uint64_t elapsed = end - start;
	double elapsedMs = ((double) elapsed) / (1000 * 1000);
	LOG(INFO) << id << " | Async Request (Done): " << url << " (" << elapsedMs << "ms)";

	if (NULL == req) {
		LOG(ERROR) << "Request failed: " << url;
		HttpResponse *response = new HttpResponse();
		response->setRequest(this).setResponseCode(0).setResponseText("");
		onload.fire(response);
		delete response;
	} else {
		LOG(INFO) << id << " | Response for: " << url << ": " << req->response_code << " " << req->response_code_line;

		HttpConnection *connection = context->getConnection();
		connection->release();

		HttpResponse *response = new HttpResponse();
		response->setRequest(this)
		.setResponseCode(req->response_code)
		.setResponseText((char *)
				(req->response_code_line ? req->response_code_line : ""));
		onload.fire(response);
		delete response;
	}
	event_base_loopbreak(context->getBase());
}

HttpRequest &HttpRequest::open(evhttp_cmd_type method, string url) {
	this->method = method;
	this->url = url;

	LOG(INFO) << id << " | Opening new request: " << url;
	uri = evhttp_uri_parse(url.data());

	path = evhttp_uri_get_path(uri);
	query = evhttp_uri_get_query(uri) ? evhttp_uri_get_query(uri) : "";
	fragment = evhttp_uri_get_fragment(uri) ? evhttp_uri_get_fragment(uri) : "";

	string hostname = (char *) evhttp_uri_get_host(uri);
	HttpClient client = HttpClientImpl::NewInstance(hostname,
			(uint16_t) evhttp_uri_get_port(uri));

	LOG(INFO) << id << " | Using client: " << client->getId();

	context->setUrl(url);
	context->setConnection(client->open(method, evhttp_uri_get_path(uri)));

	context->setRequest(evhttp_request_new(HttpRequest::_evHttpReqDone, this));

	LOG(INFO) << id << " | Async Request (Created): " << url;
	struct evhttp_request *request = context->getRequest();
	evhttp_add_header(request->output_headers, "Connection", "keep-alive");
	LOG(INFO) << id << " | HEADER - " << "Connection: keep-alive";

	return *this;
}

HttpRequest &HttpRequest::setHeader(string name, string value) {
	struct evhttp_request *request = context->getRequest();
	evhttp_add_header(request->output_headers, name.data(),
	            value.data());
	LOG(INFO) << id << " | HEADER - " << name << ": " << value;
	return *this;
}

bool HttpRequest::send(size_t contentLength) {
	struct evhttp_request *request = context->getRequest();
	evhttp_add_header(request->output_headers, "Content-Length",
			to_string(contentLength).data());
	LOG(INFO) << id << " | HEADER - " << "Content-Length" << ": " <<
			to_string(contentLength).data();

	string fullPath = path +
			(query.size() ? "?" + query : "") +
			(fragment.size() ? "#" + fragment : "");

	HttpConnection *connection = context->getConnection();
	evhttp_make_request(connection->getConnection(),
			context->getRequest(), method, fullPath.data());
	start = getEpochNano();
	connection->send();
	LOG(INFO) << id << " | Async Request (Sent): " << url;
	return true;
}

bool HttpRequest::send() {
	return send(0);
}

bool HttpRequest::send(void *body, size_t len) {
	struct evhttp_request *request = context->getRequest();
	body && len && !evbuffer_add(request->output_buffer, body, len);
	return send(len);
}

uint32_t HttpRequest::getResponseCode() {
	return responseCode;
}

string& HttpRequest::getResponseMime() {
	return responseMime;
}

string& HttpRequest::getResponseText() {
	return responseText;
}

bool HttpRequest::getResponseBody(uint8_t **body, uint32_t *length) {
	struct evhttp_request *evreq = context->getRequest();
   struct evbuffer *bodyBuffer = evhttp_request_get_input_buffer(evreq);
   size_t bodyLength = evbuffer_get_length(bodyBuffer);
   *body = (uint8_t *) malloc(bodyLength);
   *length = evbuffer_remove(bodyBuffer, *body, bodyLength);
   if(*length != bodyLength) {
      LOG(INFO) << "Could not read entire body. Expected: " << bodyLength <<
         " bytes, read: " << (*length) << " bytes";
      return false;
   } else {
      return true;
   }
}

string &HttpRequest::getId() {
	return id;
}

} // End namespace Client.
} // End namespace Http.
} // End namespace ChCppUtils.

