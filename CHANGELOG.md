# Changelog

## v1.1.0
**Tag**: `1.1.0`.

**Date**: Dec. 16th, 2016

**Description**:

Improvement of the public API:

* Make server on_new_connection callback take shared_ptr as parameter instead of reference (provide more flexibility to the client app)
* Provide access to tcp_socket in the public API of tcp_client and tcp_server


## v1.0.0
**Tag**: `1.0.0`.

**Date**: Dec. 12th, 2016

**Description**:

TCP Client & Server for Unix & Mac platform.
Documented in the wiki.

Windows port will come as well as some additional features in later releases.
In particular, some features available on the cpp_redis network module will be imported here.
