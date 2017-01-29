# Changelog

## v2.0.0
**Tag**: `2.0.0`.

**Date**: Jan. 29th, 2017

**Description**:

* Feature: Port the library onto windows
* Feature: Make the library usable by cpp_redis
* Fix: some sockets where not removed from io_service tracking. Now fixed
* Improvement: handle POLLHUP events


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
