#!/bin/sh

# https://exploreflask.com/deployment.html

sudo gunicorn -b 127.0.0.1:8000 -p minitwit.pid -D minitwit:app
