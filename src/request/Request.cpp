#include "../../includes/Request.hpp"

Request::Request(): _method(0), _parse_state(RequestLine), _bodyFd(-1), _content_len(0), _received_bytes(0) {}

