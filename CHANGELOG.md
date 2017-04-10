# Changelog


## [v2.4.0](https://github.com/Cylix/tacopie/releases/tag/2.4.0)
### Tag
`2.4.0`
### Date
April 9th, 2017
### Changes
None.
### Additions
* Provide support for Unix socket. Simply pass in 0 as the port when building a `tcp_socket`, `tcp_client` or `tcp_server`. Then, the host will automatically be treated as the path to a Unix socket instead of a real host.
### Removals
None.





## [v2.3.0](https://github.com/Cylix/tacopie/releases/tag/2.3.0)
### Tag
`2.3.0`
### Date
April 9th, 2017
### Changes
None.
### Additions
* TCP server now supports `wait_for_removal` as a parameter for `.stop()`. Please refer to the [documentation](https://github.com/Cylix/tacopie/wiki/TCP-Server#stop) for more information.
### Removals
None.




## [v2.2.0](https://github.com/Cylix/tacopie/releases/tag/2.2.0)
### Tag
`2.2.0`
### Date
April 4th, 2017
### Changes
* IO Service is now based on `select` and not on `poll` anymore to solve some issues encountered on windows due to the buggy implementation of `poll` on windows Systems.
### Additions
None.
### Removals
None.




## [v2.1.0](https://github.com/Cylix/tacopie/releases/tag/2.1.0)
### Tag
`2.1.0`
### Date
March 19th, 2017
### Changes
* read and write TCP client callbacks now takes a reference to the result as parameter instead of a const-reference.
### Additions
None.
### Removals
* install_deps.sh has been removed in favor of CMakelists.txt enhancement.



## [v2.0.1](https://github.com/Cylix/tacopie/releases/tag/2.0.1)
### Tag
`2.0.1`
### Date
Feb. 17th, 2017
### Changes
* Fix: replace gethostbyname() (not thread-safe) usage by getaddrinfo() (thread-safe) on unix platforms. No change where required on windows as getaddrinfo() was already in use before.
### Additions
None.
### Removals
None.




## [v2.0.0](https://github.com/Cylix/tacopie/releases/tag/2.0.0)
### Tag
`2.0.0`
### Date
Jan. 29th, 2017
### Changes
* Fix: some sockets were not removed from io_service tracking. Now fixed
* Improvement: handle POLLHUP events
### Additions
* Feature: Port the library onto windows
* Feature: Make the library usable by cpp_redis
### Removals
None.




## [v1.1.0](https://github.com/Cylix/tacopie/releases/tag/1.1.0)
### Tag
`1.1.0`
### Date
Dec. 16th, 2016
### Changes
* Make server on_new_connection callback take shared_ptr as parameter instead of reference (provide more flexibility to the client app)
### Additions
* Provide access to tcp_socket in the public API of tcp_client and tcp_server
### Removals
None.




## [v1.0.0](https://github.com/Cylix/tacopie/releases/tag/1.0.0)
### Tag
`1.0.0`
### Date
Dec. 12th, 2016
### Changes
None.
### Additions
* TCP Client & Server for Unix & Mac platform.
* Documented in the wiki.
### Removals
None.
