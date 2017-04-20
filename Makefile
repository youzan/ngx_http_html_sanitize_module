PATH := /opt/nginx/sbin:/usr/local/bin:/usr/local/nginx/sbin:$(PATH)

test:
	@WORKDIR=$(shell pwd) /usr/bin/prove -r t

.PHONY: test
