<p align="center">
   <img src="https://raw.githubusercontent.com/Cylix/tacopie/master/assets/images/tacopie_logo.jpg"/>
</p>

# Taco Pie [![Build Status](https://travis-ci.org/Cylix/tacopie.svg?branch=master)](https://travis-ci.org/Cylix/tacopie)
`tacopie` is a TCP Client & Server C++11 library.

## Requirement
`tacopie` has **no dependency**. Its only requirement is `C++11`.

## Example
`tacopie::tcp_server`:
```cpp
tacopie::tcp_server s;
s.start("127.0.0.1", 3001, [] (tacopie::tcp_client& client) -> bool {
  std::cout << "New client" << std::endl;
  return true;
});
```
`tacopie::tcp_server` [full documentation](https://github.com/Cylix/tacopie/wiki/TCP-Server) and [detailed example](https://github.com/Cylix/tacopie/wiki/Examples#tcp-server).

## Wiki
A [Wiki](https://github.com/Cylix/tacopie/wiki) is available and provides full documentation for the library as well as [installation explanations](https://github.com/Cylix/tacopie/wiki/Installation).

## License
`tacopie` is under [MIT License](LICENSE).

## Contributing
Please refer to [CONTRIBUTING.md](CONTRIBUTING.md).

## Author
[Simon Ninon](http://simon-ninon.fr)
