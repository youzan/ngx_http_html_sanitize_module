#! /bin/bash

time curl -X POST -o /dev/null -d @google.html "http://127.0.0.1:8888?document=1&context=0&html=1&element=0&attribute=0"
