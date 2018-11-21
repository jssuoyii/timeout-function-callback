PHP_INCLUDE = `php-config --includes`
PHP_LIBS = `php-config --libs`
PHP_LDFLAGS = `php-config --ldflags`
PHP_INCLUDE_DIR = `php-config --include-dir`
PHP_EXTENSION_DIR = `php-config --extension-dir`

timeout.so: timeout.h  timeout.cc 
	c++ -DHAVE_CONFIG_H -g -o timeout.so -O0 -fPIC -shared  timeout.h  timeout.cc -std=c++11 ${PHP_INCLUDE} -I${PHP_INCLUDE_DIR} -lphpx
install:timeout.so
	\cp timeout.so ${PHP_EXTENSION_DIR}/
	rm -f timeout.so
 
