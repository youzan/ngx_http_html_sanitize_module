// Copyright 2017 Youzan Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: detailyang@gmail.com (Yang Bingwu)


#ifndef __NGX_HTTP_html_SANITIZE__H_INCLUDED_
#define __NGX_HTTP_html_SANITIZE__H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#define NGX_HTTP_HTML_SANITIZE_HASH_MAX_SIZE 2048
#define NGX_HTTP_HTML_SANITIZE_HASH_BUCKET_SIZE ngx_cacheline_size

#define NGX_HTTP_HTML_SANITIZE_ELEMENT_SIZE 32
#define NGX_HTTP_HTML_SANITIZE_ATTRIBUTE_SIZE 32
#define NGX_HTTP_HTML_SANITIZE_PROTOCOL_SIZE 4
#define NGX_HTTP_HTML_SANITIZE_IFRAME_PROTOCOL_SIZE 4
#define NGX_HTTP_HTML_SANITIZE_DOMAIN_SIZE 16
#define NGX_HTTP_HTML_SANITIZE_IFRAME_DOMAIN_SIZE 16

#define NGX_HTTP_HTML_SANITIZE_STYLE_PROPERTY_SIZE 32


typedef struct {
    ngx_flag_t             enable;
    ngx_int_t              hash_max_size;
    ngx_int_t              hash_bucket_size;
    ngx_array_t           *elements;
    ngx_hash_init_t        eh;
    ngx_array_t           *attributes;
    ngx_hash_keys_arrays_t ah;
    ngx_hash_combined_t    cah;
    ngx_array_t           *protocols;
    ngx_hash_init_t        ph;
    ngx_array_t           *iframe_protocols;
    ngx_hash_init_t        iph;
    ngx_array_t           *style_properties;
    ngx_hash_init_t        sph;
    ngx_array_t           *domains;
    ngx_hash_keys_arrays_t dh;
    ngx_hash_combined_t    cdh;
    ngx_array_t           *iframe_domains;
    ngx_hash_keys_arrays_t idh;
    ngx_hash_combined_t    icdh;
} ngx_http_html_sanitize_loc_conf_t;


typedef struct {
    int done:1;
    int waiting_more_body:1;
    int document:1;
    int html:1;
    int script:1;
    int style:1;
    int style_property_value:1;
    int url_protocol:1;
    int url_domain:1;
    int iframe_url_protocol:1;
    int iframe_url_domain:1;
    int element;
    int attribute;
    int style_property;
    int namespace;
    int context;
} ngx_http_html_sanitize_ctx_t;


typedef struct {
    ngx_chain_t *head;
    ngx_chain_t *tail;
} ngx_http_html_sanitize_chain_t;


#endif
