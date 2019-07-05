#pragma once

#include "redpanda/kafka/requests/headers.h"
#include "redpanda/kafka/requests/request_reader.h"
#include "utils/fragmented_temporary_buffer.h"

#include <seastar/core/future.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/sharded.hh>
#include <seastar/util/log.hh>

#include <memory>

namespace kafka::requests {

extern seastar::logger kreq_log;

class request_context {
public:
    request_context(
      request_header&& header, fragmented_temporary_buffer&& request) noexcept
      : _header(std::move(header))
      , _request(std::move(request))
      , _reader(_request.get_istream()) {
    }

    const request_header& header() const {
        return _header;
    }

    request_reader& reader() {
        return _reader;
    }

private:
    request_header _header;
    fragmented_temporary_buffer _request;
    request_reader _reader;
};

class response;
using response_ptr = seastar::foreign_ptr<std::unique_ptr<response>>;

// Executes the API call identified by the specified request_context.
seastar::future<response_ptr>
process_request(request_context&, seastar::smp_service_group);

} // namespace kafka::requests