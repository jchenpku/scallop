#!/bin/bash

tag="B668"

./run.sh $tag 0.01

sleep 1.11
./run.sh $tag 1

sleep 1.11
./run.sh $tag 2.5

sleep 1.11
./run.sh $tag 5

sleep 1.11
./run.sh $tag 7.5

sleep 1.11
./run.sh $tag 10

sleep 1.11
./run.sh $tag 25

sleep 1.11
./run.sh $tag 50

sleep 1.11
./run.sh $tag 75

sleep 1.11
./run.sh $tag 100
