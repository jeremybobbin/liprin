#!/bin/sh
rm f1 f2
./atomic_append f1 1000000 & ./atomic_append f1 1000000 
./atomic_append f2 1000000 x & ./atomic_append f2 1000000 x

stat f1
stat f2
