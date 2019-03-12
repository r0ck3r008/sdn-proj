#!/bin/zsh

docker run -e PATH=/pox:$PATH -e PYTHONPATH=/pox/ext -v ${PWD}/ext:/pox/ext -it pox
