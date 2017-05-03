#!/bin/sh

g++ -c src/*.cc --std=c++11 -I src/headers

g++ src/wheespa.cpp -o wheespa *.o /usr/lib/x86_64-linux-gnu/libsqlite3.so.0 /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1 /usr/lib/x86_64-linux-gnu/libssl.so.1.1 -l pthread --std=c++11 -I src/headers

rm *.o

./wheespa -lv

