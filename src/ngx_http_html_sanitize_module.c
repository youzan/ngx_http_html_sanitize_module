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


#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include "ngx_http_html_sanitize_module.h"
#include "gumbo.h"
#include "katana.h"


static void *
ngx_http_html_sanitize_create_loc_conf(ngx_conf_t *cf);
static char *
ngx_http_html_sanitize_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);

static char *
ngx_conf_html_sanitize_set_flag(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static char *
ngx_conf_html_sanitize_set_element(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static char *
ngx_conf_html_sanitize_set_attribute(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static char *
ngx_conf_html_sanitize_set_protocol(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static char *
ngx_conf_html_sanitize_set_iframe_protocol(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static char *
ngx_conf_html_sanitize_set_domain(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static char *
ngx_conf_html_sanitize_set_iframe_domain(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static char *
ngx_conf_html_sanitize_set_style_property(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);

static ngx_int_t
ngx_http_html_sanitize_handler(ngx_http_request_t *r);
static ngx_int_t
ngx_http_html_sanitize_travel(ngx_http_request_t *r);
static void
ngx_http_html_sanitize_post_read(ngx_http_request_t *r);
static ngx_int_t
ngx_http_html_sanitize_get_post_body(ngx_http_request_t *r,
    u_char **begin, u_char **end);

static inline ngx_int_t
ngx_http_html_sanitize_substitute_xml_entities_into_text(ngx_http_request_t *r,
    u_char *str, u_char **dst, char quote);
static inline ngx_int_t
ngx_http_html_sanitize_void_tag(GumboTag tag);
static inline ngx_int_t
ngx_http_html_sanitize_entity_substitution(GumboTag tag);
static inline ngx_int_t
ngx_http_html_sanitize_link_attr(GumboTag tag, const char *attr);
static inline ngx_int_t
ngx_http_html_sanitize_inline_tag(GumboTag tag);
static inline ngx_int_t
ngx_http_html_sanitize_whitespace_tag(GumboTag tag);
static inline ngx_int_t
ngx_http_html_sanitize_is_iframe_src(GumboTag tag, const char *attr);
static inline ngx_int_t
ngx_http_html_sanitize_is_style_attribute(const char *name);
static inline ngx_int_t
ngx_http_html_sanitize_is_style_expression(const char *name);
static inline void
ngx_http_html_sanitize_rtrim(ngx_str_t *value);
static inline void
ngx_http_html_sanitize_style_quote_replace(u_char *p, size_t len,
    const char *quote);
static inline ngx_int_t
ngx_http_html_sanitize_url_absolute(const char *url);

static ngx_int_t
ngx_http_html_sanitize_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc,
    u_char *src, ngx_int_t ssize);
static ngx_int_t
ngx_http_html_sanitize_document_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node);
static ngx_int_t
ngx_http_html_sanitize_root_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node);
static ngx_int_t
ngx_http_html_sanitize_element_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node);
static ngx_int_t
ngx_http_html_sanitize_text_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node);
static ngx_int_t
ngx_http_html_sanitize_attribute_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node, GumboAttribute *attr,
    ngx_str_t *tagname, ngx_int_t no_entities);
static ngx_int_t
ngx_http_html_sanitize_whitespace_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node);
static ngx_int_t
ngx_http_html_sanitize_style_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node, ngx_str_t *tagname,
    GumboAttribute *attr, const char *quote);
static ngx_int_t
ngx_http_html_sanitize_style_declration_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node, ngx_str_t *tagname,
    GumboAttribute *attr, const char *quote, KatanaDeclaration *declaration);
static ngx_int_t
ngx_http_html_sanitize_style_declration_property_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, const char *quote,
    KatanaDeclaration *declaration);

static ngx_int_t
ngx_http_html_sanitize_get_tag_name(ngx_http_request_t *r, GumboNode *node,
    ngx_str_t *value);
static ngx_int_t
ngx_http_html_sanitize_append_string_to_chain(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, const char *str);
static ngx_int_t
ngx_http_html_sanitize_build_first_chain(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc);

static ngx_int_t
ngx_http_html_sanitize_lookup_url_domain(const char *url, u_char **begin,
    u_char **end);
static void*
ngx_http_html_sanitize_allocator(void* userdata, size_t size);
static void
ngx_http_html_sanitize_deallocator(void* userdata, void* ptr);
static ngx_int_t
ngx_http_html_sanitize_pasre_args(ngx_http_request_t *r);
static ngx_int_t
ngx_http_html_sanitize_peek_args(ngx_str_t *args,
    ngx_str_t *key, ngx_str_t *value);


static ngx_command_t ngx_http_html_sanitize_commands[] = {
    {
        ngx_string("html_sanitize"),
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_conf_html_sanitize_set_flag,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_html_sanitize_loc_conf_t, enable),
        NULL
    },
    {
        ngx_string("html_sanitize_hash_max_size"),
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_conf_set_num_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_html_sanitize_loc_conf_t, hash_max_size),
        NULL
    },
    {
        ngx_string("html_sanitize_hash_bucket_size"),
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_conf_set_num_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_html_sanitize_loc_conf_t, hash_bucket_size),
        NULL
    },
    {
        ngx_string("html_sanitize_element"),
        NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_html_sanitize_set_element,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    {
        ngx_string("html_sanitize_attribute"),
        NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_html_sanitize_set_attribute,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    {
        ngx_string("html_sanitize_url_protocol"),
        NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_html_sanitize_set_protocol,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    {
        ngx_string("html_sanitize_iframe_url_protocol"),
        NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_html_sanitize_set_iframe_protocol,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    {
        ngx_string("html_sanitize_url_domain"),
        NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_html_sanitize_set_domain,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    {
        ngx_string("html_sanitize_iframe_url_domain"),
        NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_html_sanitize_set_iframe_domain,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    {
        ngx_string("html_sanitize_style_property"),
        NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_html_sanitize_set_style_property,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};


static ngx_http_module_t  ngx_http_html_sanitize_ctx = {
    NULL,
    NULL,

    NULL,
    NULL,

    NULL,
    NULL,

    ngx_http_html_sanitize_create_loc_conf,
    ngx_http_html_sanitize_merge_loc_conf,
};


ngx_module_t ngx_http_html_sanitize_module = {
    NGX_MODULE_V1,
    &ngx_http_html_sanitize_ctx,              /* module context */
    ngx_http_html_sanitize_commands,          /* module directives */
    NGX_HTTP_MODULE,                          /* module type */
    NULL,                                     /* init master */
    NULL,                                     /* init module */
    NULL,                                     /* init process */
    NULL,                                     /* init thread */
    NULL,                                     /* exit thread */
    NULL,                                     /* exit process */
    NULL,                                     /* exit master */
    NGX_MODULE_V1_PADDING
};


static void *
ngx_http_html_sanitize_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_html_sanitize_loc_conf_t *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_html_sanitize_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    /*
     * set by ngx_pcalloc():
     * conf->enable = 0
     */

    conf->enable = NGX_CONF_UNSET;
    conf->hash_max_size = NGX_CONF_UNSET;
    conf->hash_bucket_size = NGX_CONF_UNSET;

    return conf;
}


static char *
ngx_http_html_sanitize_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_html_sanitize_loc_conf_t *gslf = child;

    ngx_int_t       i, rc;
    ngx_hash_key_t *attribute, *domain, *iframe_domain;
    ngx_hash_init_t dh;

    /*
     * no main src conf
     */

    if (gslf->hash_max_size == NGX_CONF_UNSET) {
        gslf->hash_max_size = NGX_HTTP_HTML_SANITIZE_HASH_MAX_SIZE;
    }

    if (gslf->hash_bucket_size == NGX_CONF_UNSET) {
        gslf->hash_bucket_size = NGX_HTTP_HTML_SANITIZE_HASH_BUCKET_SIZE;
    } else {
        gslf->hash_bucket_size = ngx_align(gslf->hash_bucket_size,
                                           ngx_cacheline_size);
    }

    gslf->eh.key = ngx_hash_key_lc;
    gslf->eh.max_size = gslf->hash_max_size;
    gslf->eh.bucket_size = gslf->hash_bucket_size;
    gslf->eh.name = "html_sanitize_element";
    gslf->eh.pool = cf->pool;

    if (gslf->elements && gslf->elements->nelts) {
        if (ngx_hash_init(&(gslf->eh), (ngx_hash_key_t*) gslf->elements->elts,
                          gslf->elements->nelts) != NGX_OK)
        {
            return NGX_CONF_ERROR;
        }
    }

    gslf->ph.key = ngx_hash_key_lc;
    gslf->ph.max_size = gslf->hash_max_size;
    gslf->ph.bucket_size = gslf->hash_bucket_size;
    gslf->ph.name = "html_sanitize_protocol";
    gslf->ph.pool = cf->pool;

    if (gslf->protocols && gslf->protocols->nelts) {
        if (ngx_hash_init(&(gslf->ph), (ngx_hash_key_t*) gslf->protocols->elts,
                          gslf->protocols->nelts) != NGX_OK)
        {
            return NGX_CONF_ERROR;
        }
    }

    gslf->iph.key = ngx_hash_key_lc;
    gslf->iph.max_size = gslf->hash_max_size;
    gslf->iph.bucket_size = gslf->hash_bucket_size;
    gslf->iph.name = "html_sanitize_iframe_protocol";
    gslf->iph.pool = cf->pool;

    if (gslf->iframe_protocols && gslf->iframe_protocols->nelts) {
        if (ngx_hash_init(&(gslf->iph),
                          (ngx_hash_key_t*) gslf->iframe_protocols->elts,
                          gslf->iframe_protocols->nelts) != NGX_OK)
        {
            return NGX_CONF_ERROR;
        }
    }

    gslf->sph.key = ngx_hash_key_lc;
    gslf->sph.max_size = gslf->hash_max_size;
    gslf->sph.bucket_size = gslf->hash_bucket_size;
    gslf->sph.name = "html_sanitize_style_property";
    gslf->sph.pool = cf->pool;

    if (gslf->style_properties && gslf->style_properties->nelts) {
        if (ngx_hash_init(&(gslf->sph),
                          (ngx_hash_key_t*) gslf->style_properties->elts,
                          gslf->style_properties->nelts) != NGX_OK)
        {
            return NGX_CONF_ERROR;
        }
    }

    dh.key = ngx_hash_key_lc;
    dh.max_size = gslf->hash_max_size;
    dh.bucket_size = gslf->hash_bucket_size;
    dh.name = "html_sanitize_attribute";
    dh.pool = cf->pool;

    if (gslf->attributes && gslf->attributes->nelts) {
        gslf->ah.pool = cf->pool;
        gslf->ah.temp_pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, cf->log);
        if (gslf->ah.temp_pool == NULL) {
            return NGX_CONF_ERROR;
        }

        if(ngx_hash_keys_array_init(&(gslf->ah), NGX_HASH_LARGE) != NGX_OK)
        {
            ngx_destroy_pool(gslf->ah.temp_pool);
            return NGX_CONF_ERROR;
        }

        for (i = 0; i < (ngx_int_t) gslf->attributes->nelts; i++) {
            attribute = gslf->attributes->elts;
            rc = ngx_hash_add_key(&(gslf->ah), &attribute[i].key, &attribute[i],
                             NGX_HASH_WILDCARD_KEY);
            if (rc == NGX_ERROR) {
                ngx_destroy_pool(gslf->ah.temp_pool);
                return NGX_CONF_ERROR;
            }

            if (rc == NGX_DECLINED) {
                ngx_log_error(NGX_LOG_EMERG, cf->log, 0,
                              "invalid html_sanitize_url_attribute or"
                              " wildcard \"%V\"",
                              &attribute[i].key);
                ngx_destroy_pool(gslf->ah.temp_pool);
                return NGX_CONF_ERROR;
            }

            if (rc == NGX_BUSY) {
                ngx_log_error(NGX_LOG_WARN, cf->log, 0,
                              "conflicting html_sanitize_url_attribute"
                              " \"%V\", ignored",
                              &attribute[i].key);
            }
        }

        if(gslf->ah.keys.nelts) {
            dh.hash = &gslf->cah.hash;
            dh.temp_pool = NULL;
            if(ngx_hash_init(&dh, gslf->attributes->elts,
                             gslf->attributes->nelts) != NGX_OK) {
                ngx_destroy_pool(gslf->dh.temp_pool);
                return NGX_CONF_ERROR;
            }
        }

        if(gslf->ah.dns_wc_head.nelts) {
            dh.hash = NULL;
            dh.temp_pool = gslf->ah.temp_pool;

            if(ngx_hash_wildcard_init(&dh,
                                      gslf->ah.dns_wc_head.elts,
                                      gslf->ah.dns_wc_head.nelts) != NGX_OK)
            {
                ngx_destroy_pool(gslf->ah.temp_pool);
                return NGX_CONF_ERROR;
            }

            gslf->cah.wc_head = (ngx_hash_wildcard_t *)dh.hash;
        }

        if(gslf->ah.dns_wc_tail.nelts) {
            dh.hash = NULL;
            dh.temp_pool = gslf->ah.temp_pool;

            if(ngx_hash_wildcard_init(&dh,
                                      gslf->ah.dns_wc_tail.elts,
                                      gslf->ah.dns_wc_tail.nelts) != NGX_OK)
            {
                ngx_destroy_pool(gslf->ah.temp_pool);
                return NGX_CONF_ERROR;
            }

            gslf->cah.wc_tail = (ngx_hash_wildcard_t *)dh.hash;
        }

        ngx_destroy_pool(gslf->ah.temp_pool);
    }

    dh.key = ngx_hash_key_lc;
    dh.max_size = gslf->hash_max_size;
    dh.bucket_size = gslf->hash_bucket_size;
    dh.name = "html_sanitize_domain";
    dh.pool = cf->pool;

    if (gslf->domains && gslf->domains->nelts) {

        gslf->dh.pool = cf->pool;
        gslf->dh.temp_pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, cf->log);
        if (gslf->dh.temp_pool == NULL) {
            return NGX_CONF_ERROR;
        }

        if(ngx_hash_keys_array_init(&(gslf->dh), NGX_HASH_LARGE) != NGX_OK)
        {
            ngx_destroy_pool(gslf->dh.temp_pool);
            return NGX_CONF_ERROR;
        }

        for (i = 0; i < (ngx_int_t) gslf->domains->nelts; i++) {
            domain = gslf->domains->elts;
            rc = ngx_hash_add_key(&(gslf->dh), &domain[i].key, &domain[i],
                             NGX_HASH_WILDCARD_KEY);
            if (rc == NGX_ERROR) {
                ngx_destroy_pool(gslf->dh.temp_pool);
                return NGX_CONF_ERROR;
            }

            if (rc == NGX_DECLINED) {
                ngx_log_error(NGX_LOG_EMERG, cf->log, 0,
                              "invalid html_sanitize_url_domain or"
                              " wildcard \"%V\"",
                              &domain[i].key);
                ngx_destroy_pool(gslf->dh.temp_pool);
                return NGX_CONF_ERROR;
            }

            if (rc == NGX_BUSY) {
                ngx_log_error(NGX_LOG_WARN, cf->log, 0,
                              "conflicting html_sanitize_url_domain"
                              " \"%V\", ignored",
                              &domain[i].key);
            }
        }

        if(gslf->dh.keys.nelts) {
            dh.hash = &gslf->cdh.hash;
            dh.temp_pool = NULL;
            if(ngx_hash_init(&dh, gslf->domains->elts,
                             gslf->domains->nelts) != NGX_OK) {
                ngx_destroy_pool(gslf->dh.temp_pool);
                return NGX_CONF_ERROR;
            }
        }

        if(gslf->dh.dns_wc_head.nelts) {
            dh.hash = NULL;
            dh.temp_pool = gslf->dh.temp_pool;

            if(ngx_hash_wildcard_init(&dh,
                                      gslf->dh.dns_wc_head.elts,
                                      gslf->dh.dns_wc_head.nelts) != NGX_OK)
            {
                ngx_destroy_pool(gslf->dh.temp_pool);
                return NGX_CONF_ERROR;
            }

            gslf->cdh.wc_head = (ngx_hash_wildcard_t *)dh.hash;
        }

        if(gslf->dh.dns_wc_tail.nelts) {
            dh.hash = NULL;
            dh.temp_pool = gslf->dh.temp_pool;

            if(ngx_hash_wildcard_init(&dh,
                                      gslf->dh.dns_wc_tail.elts,
                                      gslf->dh.dns_wc_tail.nelts) != NGX_OK)
            {
                ngx_destroy_pool(gslf->dh.temp_pool);
                return NGX_CONF_ERROR;
            }

            gslf->cdh.wc_tail = (ngx_hash_wildcard_t *)dh.hash;
        }

        ngx_destroy_pool(gslf->dh.temp_pool);
    }

    dh.key = ngx_hash_key_lc;
    dh.max_size = gslf->hash_max_size;
    dh.bucket_size = gslf->hash_bucket_size;
    dh.name = "html_sanitize_iframe_domain";
    dh.pool = cf->pool;

    if (gslf->iframe_domains && gslf->iframe_domains->nelts) {

        gslf->idh.pool = cf->pool;
        gslf->idh.temp_pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, cf->log);
        if (gslf->idh.temp_pool == NULL) {
            return NGX_CONF_ERROR;
        }

        if(ngx_hash_keys_array_init(&(gslf->idh), NGX_HASH_LARGE) != NGX_OK)
        {
            ngx_destroy_pool(gslf->dh.temp_pool);
            return NGX_CONF_ERROR;
        }

        for (i = 0; i < (ngx_int_t) gslf->iframe_domains->nelts; i++) {
            iframe_domain = gslf->iframe_domains->elts;
            rc = ngx_hash_add_key(&(gslf->idh), &iframe_domain[i].key, &iframe_domain[i],
                                  NGX_HASH_WILDCARD_KEY);
            if (rc == NGX_ERROR) {
                ngx_destroy_pool(gslf->idh.temp_pool);
                return NGX_CONF_ERROR;
            }

            if (rc == NGX_DECLINED) {
                ngx_log_error(NGX_LOG_EMERG, cf->log, 0,
                              "invalid html_sanitize_url_domain or"
                              " wildcard \"%V\"",
                              &iframe_domain[i].key);
                ngx_destroy_pool(gslf->idh.temp_pool);
                return NGX_CONF_ERROR;
            }

            if (rc == NGX_BUSY) {
                ngx_log_error(NGX_LOG_WARN, cf->log, 0,
                              "conflicting html_sanitize_url_domain"
                              " \"%V\", ignored",
                              &iframe_domain[i].key);
            }
        }

        if(gslf->idh.keys.nelts) {
            dh.hash = &gslf->icdh.hash;
            dh.temp_pool = NULL;
            if(ngx_hash_init(&dh, gslf->iframe_domains->elts,
                             gslf->iframe_domains->nelts) != NGX_OK) {
                ngx_destroy_pool(gslf->idh.temp_pool);
                return NGX_CONF_ERROR;
            }
        }

        if(gslf->idh.dns_wc_head.nelts) {
            dh.hash = NULL;
            dh.temp_pool = gslf->idh.temp_pool;

            if(ngx_hash_wildcard_init(&dh,
                                      gslf->idh.dns_wc_head.elts,
                                      gslf->idh.dns_wc_head.nelts) != NGX_OK)
            {
                ngx_destroy_pool(gslf->idh.temp_pool);
                return NGX_CONF_ERROR;
            }

            gslf->icdh.wc_head = (ngx_hash_wildcard_t *)dh.hash;
        }

        if(gslf->idh.dns_wc_tail.nelts) {
            dh.hash = NULL;
            dh.temp_pool = gslf->idh.temp_pool;

            if(ngx_hash_wildcard_init(&dh,
                                      gslf->idh.dns_wc_tail.elts,
                                      gslf->idh.dns_wc_tail.nelts) != NGX_OK)
            {
                ngx_destroy_pool(gslf->idh.temp_pool);
                return NGX_CONF_ERROR;
            }

            gslf->icdh.wc_tail = (ngx_hash_wildcard_t *)dh.hash;
        }

        ngx_destroy_pool(gslf->idh.temp_pool);
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_flag(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_str_t                          *value;
    ngx_http_core_loc_conf_t           *clcf;
    ngx_http_html_sanitize_loc_conf_t *gslf;

    value = cf->args->elts;
    gslf = conf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    if (gslf->enable != NGX_CONF_UNSET) {
        return "is duplicate";
    }

    if (ngx_strcasecmp(value[1].data, (u_char *) "on") == 0) {
        clcf->handler = ngx_http_html_sanitize_handler;
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_element(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    ngx_str_t                          *value;
    ngx_uint_t                          i;
    ngx_hash_key_t                     *element;
    ngx_http_html_sanitize_loc_conf_t *gslf;

    gslf = conf;

    if (gslf->elements == NULL) {
        gslf->elements = ngx_array_create(cf->pool,
            NGX_HTTP_HTML_SANITIZE_ELEMENT_SIZE, sizeof(ngx_hash_key_t));
        if (gslf->elements == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 1, value = cf->args->elts; i < cf->args->nelts; i++) {
        element = ngx_array_push(gslf->elements);
        if (element == NULL) {
            return NGX_CONF_ERROR;
        }

        element->key.data = value[i].data;
        element->key.len = value[i].len;
        element->key_hash = ngx_hash_key_lc(value[i].data, value[i].len);
        element->value = (void*)1;
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_attribute(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    u_char                             *p, *pe;
    size_t                              len;
    ngx_str_t                          *value;
    ngx_uint_t                          i, j;
    ngx_hash_key_t                     *attribute;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    gslf = conf;

    if (gslf->attributes == NULL) {
        gslf->attributes = ngx_array_create(cf->pool,
            NGX_HTTP_HTML_SANITIZE_ATTRIBUTE_SIZE, sizeof(ngx_hash_key_t));
        if (gslf->attributes == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 1, value = cf->args->elts; i < cf->args->nelts; i++) {
        attribute = ngx_array_push(gslf->attributes);
        if (attribute == NULL) {
            return NGX_CONF_ERROR;
        }

        /*
         * attribute format: element.attribute
         */

        pe = value[i].data;
        for (j = 0; j < value[i].len; j++) {
            if (pe[j] == '.')  {
                break;
            }
        }

        if (j >= value[i].len - 1) {
            len = 1 + value[i].len + 1
                  + sizeof(" format unmatch and attribuet format:"
                         " element.attribute");
            p = ngx_pcalloc(cf->pool, len);
            if (p == NULL) {
                return NGX_CONF_ERROR;
            }

            ngx_memcpy(p, "\"", 1);
            ngx_memcpy(p + 1, value[i].data, value[i].len);
            ngx_memcpy(p + 1 + value[i].len, "\"", 1);
            ngx_memcpy(p + 1 + value[i].len + 1,
                       " format unmatch and attribuet format:"
                       " element.attribute",
                       sizeof(" format unmatch and attribuet format:"
                       " element.attribute") - 1);
            return (char *) p;
        }

        attribute->key.data = value[i].data;
        attribute->key.len = value[i].len;
        attribute->key_hash = ngx_hash_key_lc(value[i].data,
                                              value[i].len);
        attribute->value = (void *) 1;
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_protocol(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    ngx_str_t                          *value;
    ngx_uint_t                          i;
    ngx_hash_key_t                     *protocol;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    gslf = conf;

    if (gslf->protocols == NULL) {
        gslf->protocols = ngx_array_create(cf->pool,
            NGX_HTTP_HTML_SANITIZE_PROTOCOL_SIZE, sizeof(ngx_hash_key_t));
        if (gslf->protocols == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 1, value = cf->args->elts; i < cf->args->nelts; i++) {
        protocol = ngx_array_push(gslf->protocols);
        if (protocol == NULL) {
            return NGX_CONF_ERROR;
        }

        protocol->key.data = value[i].data;
        protocol->key.len = value[i].len;
        protocol->key_hash = ngx_hash_key_lc(value[i].data, value[i].len);
        protocol->value = (void*)1;
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_iframe_protocol(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    ngx_str_t                          *value;
    ngx_uint_t                          i;
    ngx_hash_key_t                     *iframe_protocol;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    gslf = conf;

    if (gslf->iframe_protocols == NULL) {
        gslf->iframe_protocols = ngx_array_create(cf->pool,
                        NGX_HTTP_HTML_SANITIZE_IFRAME_PROTOCOL_SIZE,
                        sizeof(ngx_hash_key_t));
        if (gslf->iframe_protocols == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 1, value = cf->args->elts; i < cf->args->nelts; i++) {
        iframe_protocol = ngx_array_push(gslf->iframe_protocols);
        if (iframe_protocol == NULL) {
            return NGX_CONF_ERROR;
        }

        iframe_protocol->key.data = value[i].data;
        iframe_protocol->key.len = value[i].len;
        iframe_protocol->key_hash = ngx_hash_key_lc(value[i].data, value[i].len);
        iframe_protocol->value = (void*)1;
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_domain(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    ngx_str_t                          *value;
    ngx_uint_t                          i;
    ngx_hash_key_t                     *domain;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    gslf = conf;

    if (gslf->domains == NULL) {
        gslf->domains = ngx_array_create(cf->pool,
            NGX_HTTP_HTML_SANITIZE_DOMAIN_SIZE, sizeof(ngx_hash_key_t));
        if (gslf->domains == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 1, value = cf->args->elts; i < cf->args->nelts; i++) {
        domain = ngx_array_push(gslf->domains);
        if (domain == NULL) {
            return NGX_CONF_ERROR;
        }

        domain->key.data = value[i].data;
        domain->key.len = value[i].len;
        domain->key_hash = ngx_hash_key_lc(value[i].data, value[i].len);
        domain->value = (void*)1;
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_iframe_domain(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    ngx_str_t                          *value;
    ngx_uint_t                          i;
    ngx_hash_key_t                     *iframe_domain;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    gslf = conf;

    if (gslf->iframe_domains == NULL) {
        gslf->iframe_domains = ngx_array_create(cf->pool,
            NGX_HTTP_HTML_SANITIZE_DOMAIN_SIZE, sizeof(ngx_hash_key_t));
        if (gslf->iframe_domains == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 1, value = cf->args->elts; i < cf->args->nelts; i++) {
        iframe_domain = ngx_array_push(gslf->iframe_domains);
        if (iframe_domain == NULL) {
            return NGX_CONF_ERROR;
        }

        iframe_domain->key.data = value[i].data;
        iframe_domain->key.len = value[i].len;
        iframe_domain->key_hash = ngx_hash_key_lc(value[i].data, value[i].len);
        iframe_domain->value = (void*)1;
    }

    return NGX_CONF_OK;
}


static char *
ngx_conf_html_sanitize_set_style_property(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf)
{
    ngx_str_t                          *value;
    ngx_uint_t                          i;
    ngx_hash_key_t                     *property;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    gslf = conf;

    if (gslf->style_properties == NULL) {
        gslf->style_properties = ngx_array_create(cf->pool,
            NGX_HTTP_HTML_SANITIZE_STYLE_PROPERTY_SIZE, sizeof(ngx_hash_key_t));
        if (gslf->style_properties == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 1, value = cf->args->elts; i < cf->args->nelts; i++) {
        property = ngx_array_push(gslf->style_properties);
        if (property == NULL) {
            return NGX_CONF_ERROR;
        }

        property->key.data = value[i].data;
        property->key.len = value[i].len;
        property->key_hash = ngx_hash_key_lc(value[i].data, value[i].len);
        property->value = (void*)1;
    }

    return NGX_CONF_OK;
}


static ngx_int_t
ngx_http_html_sanitize_handler(ngx_http_request_t *r)
{
    ngx_int_t                           rc;
    ngx_http_html_sanitize_ctx_t       *ctx;

    if (r->method != NGX_HTTP_POST) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);
    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_html_sanitize_ctx_t));
        if (ctx == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        /*
         * ctx->waiting_more_body = 0;
         * ctx->done = 0;
         */

        ngx_http_set_ctx(r, ctx, ngx_http_html_sanitize_module);
        ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http html start to read client request body");

        rc = ngx_http_read_client_request_body(r,
                                            ngx_http_html_sanitize_post_read);
        if (rc == NGX_ERROR || rc >= NGX_HTTP_SPECIAL_RESPONSE) {
#if (nginx_version < 1002006) ||                                             \
        (nginx_version >= 1003000 && nginx_version < 1003009)
            r->main->count--;
#endif
            return rc;
        }

        if (rc == NGX_AGAIN) {
            ctx->waiting_more_body = 1;
            return NGX_DONE;
        }
    }

    if (ctx->done == 0 || ctx->waiting_more_body) {
        return NGX_DONE;
    }

    return ngx_http_html_sanitize_travel(r);
}


static void
ngx_http_html_sanitize_post_read(ngx_http_request_t *r)
{
    ngx_http_html_sanitize_ctx_t     *ctx;

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http html sanitize read request body");

    ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);

    ctx->done = 1;

    if (ctx->waiting_more_body) {
        ctx->waiting_more_body = 0;
        ngx_http_finalize_request(r, ngx_http_html_sanitize_handler(r));
    } else {
        r->main->count--;
    }
}


static ngx_int_t
ngx_http_html_sanitize_get_post_body(ngx_http_request_t *r,
    u_char **begin, u_char **end)
{
    u_char      *buf, *last, *p;
    ngx_int_t    len;
    ngx_buf_t   *b;
    ngx_chain_t *cl;

    buf = NULL;
    last = NULL;
    p = NULL;

    if (r->request_body == NULL || r->request_body->bufs == NULL) {
        goto success;
    }

    if (r->request_body->bufs->next != NULL) {

        /*
         * get request body size then copy it to linear continuous memory
         */

        for (len = 0, cl = r->request_body->bufs; cl; cl = cl->next) {
            b = cl->buf;

            if (b->in_file) {
                goto failed;
            }

            len += (b->last - b->pos);
        }

        if (len == 0) {
            goto success;
        }

        buf = ngx_palloc(r->pool, len);
        if (buf == NULL) {
            goto failed;
        }

        p = buf;
        last = p + len;

        for (cl = r->request_body->bufs; cl; cl = cl->next) {
            p = ngx_copy(p, cl->buf->pos, cl->buf->last - cl->buf->pos);
        }

    } else {

        b = r->request_body->bufs->buf;
        buf = b->pos;
        last = b->last;
    }

success:

    *begin = buf;
    *end = last;

    return NGX_OK;

failed:

    *begin = buf;
    *end = last;

    return NGX_ERROR;
}


static ngx_int_t
ngx_http_html_sanitize_travel(ngx_http_request_t *r)
{
    u_char                          *buf, *last;
    ngx_int_t                        len;
    ngx_int_t                        rc;
    ngx_buf_t                       *b;
    ngx_chain_t                     *chain, *prev;
    ngx_http_html_sanitize_chain_t  *sc;

    rc = ngx_http_html_sanitize_get_post_body(r, &buf, &last);
    if (rc == NGX_ERROR) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    dd("request bufs from %p to %p %d bytes", buf, last, (int) (last - buf));

    len = last - buf;
    if (len == 0) {
        return NGX_HTTP_NO_CONTENT;
    }

    sc = ngx_pcalloc(r->pool, sizeof(ngx_http_html_sanitize_chain_t));
    if (sc == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    sc->head = NULL;
    sc->tail = NULL;

    rc = ngx_http_html_sanitize_visit(r, sc, buf, len);
    if (rc == NGX_ERROR) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    for (len = 0, chain = sc->head, prev = sc->head;
         chain != sc->tail->next;
         chain = chain->next)
    {
        b = chain->buf;

        if (b->memory == 0) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        len += ngx_buf_size(b);

        if (ngx_buf_size(b) != 0) {
            prev = chain;
            continue;
        }

        if (chain == sc->head) {
            sc->head = chain->next;
        }

        prev->next = chain->next;
    }

    dd("output chain size:%d", (int) len);

    if (len == 0) {
        return NGX_HTTP_NO_CONTENT;
    }

    r->headers_out.content_length_n = len;
    r->headers_out.status = NGX_HTTP_OK;
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    return ngx_http_output_filter(r, sc->head);
}


static ngx_int_t
ngx_http_html_sanitize_peek_args(ngx_str_t *args,
    ngx_str_t *key, ngx_str_t *value)
{
    u_char  *p, *last, *name, *begin;
    size_t   len;

    begin = args->data;
    p = args->data;
    last = args->data + args->len;
    name = key->data;
    len = key->len;

    for ( /* void */ ; p < last; p++) {
        p = ngx_strlcasestrn(p, last - 1, name, len - 1);

        if (p == NULL) {
            return NGX_ERROR;
        }

        if ((p == begin || *(p - 1) == '&') && *(p + len) == '=') {

            value->data = p + len + 1;

            p = ngx_strlchr(p, last, '&');

            if (p == NULL) {
                p = last;
            }

            value->len = p - value->data;

            return NGX_OK;
        }
    }

    return NGX_ERROR;
}


static ngx_int_t
ngx_http_html_sanitize_pasre_args(ngx_http_request_t *r)
{
    ngx_str_t namespace = ngx_string("namespace");
    ngx_str_t context = ngx_string("context");
    ngx_str_t document = ngx_string("document");
    ngx_str_t html = ngx_string("html");
    ngx_str_t script = ngx_string("script");
    ngx_str_t style = ngx_string("style");
    ngx_str_t element = ngx_string("element");
    ngx_str_t attribute = ngx_string("attribute");
    ngx_str_t url_domain = ngx_string("url_domain");
    ngx_str_t iframe_url_domain = ngx_string("iframe_url_domain");
    ngx_str_t url_protocol = ngx_string("url_protocol");
    ngx_str_t iframe_url_protocol = ngx_string("iframe_url_protocol");
    ngx_str_t style_property = ngx_string("style_property");
    ngx_str_t style_property_value = ngx_string("style_property_value");

    ngx_str_t                     *args, value;
    ngx_int_t                      rc, integer;
    ngx_http_html_sanitize_ctx_t  *ctx;

    args = &r->args;
    ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);

    /*
     * TODO: use helper function to reduce repeated code
     */

    rc = ngx_http_html_sanitize_peek_args(args, &namespace, &value);
    if (rc == NGX_ERROR) {
        ctx->namespace = GUMBO_NAMESPACE_HTML;

    } else {

        /*
         * GUMBO_NAMESPACE_HTML,
         * GUMBO_NAMESPACE_SVG,
         * GUMBO_NAMESPACE_MATHML
         */

        if (value.len == 4
            && ngx_strncasecmp(value.data, (u_char*) "html", 4) == 0)
        {
            ctx->namespace = GUMBO_NAMESPACE_HTML;

        } else if (value.len == 3
                   && ngx_strncasecmp(value.data, (u_char *)"svg", 3) == 0)
        {
            ctx->namespace = GUMBO_NAMESPACE_SVG;

        } else if (value.len == 6
                   && ngx_strncasecmp(value.data, (u_char *) "mathml", 6) == 0)
        {
            ctx->namespace = GUMBO_NAMESPACE_MATHML;

        } else {
            ctx->namespace = GUMBO_NAMESPACE_HTML;
        }
    }

    rc = ngx_http_html_sanitize_peek_args(args, &context, &value);
    if (rc == NGX_ERROR) {
        ctx->context = GUMBO_TAG_BODY;

    } else {
        integer = ngx_atoi(value.data, value.len);
        if (integer <= GUMBO_TAG_LAST) {
            ctx->context = integer;

        } else {
            ctx->context = GUMBO_TAG_BODY;
        }
    }

    rc = ngx_http_html_sanitize_peek_args(args, &document, &value);
    if (rc == NGX_ERROR) {
        ctx->document = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        if (integer == 0 || integer == 1) {
            ctx->document = integer;

        } else {
            ctx->document = 0;
        }
    }

    rc = ngx_http_html_sanitize_peek_args(args, &html, &value);
    if (rc == NGX_ERROR) {
        ctx->html = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        if (integer == 0 || integer == 1) {
            ctx->html = integer;

        } else {
            ctx->html = 0;
        }
    }

    rc = ngx_http_html_sanitize_peek_args(args, &script, &value);
    if (rc == NGX_ERROR) {
        ctx->script = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        if (integer == 0 || integer == 1) {
            ctx->script = integer;

        } else {
            ctx->script = 0;
        }
    }

    rc = ngx_http_html_sanitize_peek_args(args, &style, &value);
    if (rc == NGX_ERROR) {
        ctx->style = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        if (integer == 0 || integer == 1) {
            ctx->style = integer;

        } else {
            ctx->style = 0;
        }
    }

    rc = ngx_http_html_sanitize_peek_args(args, &url_domain, &value);
    if (rc == NGX_ERROR) {
        ctx->url_domain = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->url_domain = (integer >= 0 && integer <= 1) ? integer : 0;
    }

    rc = ngx_http_html_sanitize_peek_args(args, &url_protocol, &value);
    if (rc == NGX_ERROR) {
        ctx->url_protocol = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->url_protocol = (integer >= 0 && integer <= 1) ? integer : 0;
    }

    rc = ngx_http_html_sanitize_peek_args(args, &iframe_url_domain, &value);
    if (rc == NGX_ERROR) {
        ctx->iframe_url_domain = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->iframe_url_domain = (integer >= 0 && integer <= 1) ? integer : 0;
    }

    rc = ngx_http_html_sanitize_peek_args(args, &iframe_url_protocol, &value);
    if (rc == NGX_ERROR) {
        ctx->iframe_url_protocol = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->iframe_url_protocol = (integer >= 0 && integer <= 1) ? integer : 0;
    }

    rc = ngx_http_html_sanitize_peek_args(args, &element, &value);
    if (rc == NGX_ERROR) {
        ctx->element = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->element = integer;
    }

    rc = ngx_http_html_sanitize_peek_args(args, &attribute, &value);
    if (rc == NGX_ERROR) {
        ctx->attribute = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->attribute = integer;
    }

    rc = ngx_http_html_sanitize_peek_args(args, &style_property, &value);
    if (rc == NGX_ERROR) {
        ctx->style_property = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->style_property = integer;
    }

    rc = ngx_http_html_sanitize_peek_args(args, &style_property_value, &value);
    if (rc == NGX_ERROR) {
        ctx->style_property_value = 0;

    } else {
        integer = ngx_atoi(value.data, value.len);
        ctx->style_property_value = integer;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc,
    u_char *src, ngx_int_t ssize)
{
    ngx_int_t                        rc;
    GumboOutput                     *output;
    GumboOptions                     options;
    ngx_http_html_sanitize_ctx_t    *ctx;

    rc = ngx_http_html_sanitize_pasre_args(r);
    if (rc != NGX_OK) {
        return rc;
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);
    options.fragment_namespace = ctx->namespace;
    options.fragment_context = ctx->context;
    options.tab_stop = 8;
    options.max_errors = -1;
    options.stop_on_first_error = false;
    options.userdata = NULL;
    options.allocator = ngx_http_html_sanitize_allocator;
    options.deallocator = ngx_http_html_sanitize_deallocator;

    output = gumbo_parse_with_options(&options, (const char *) src, ssize);

    if (ctx->document) {
        rc = ngx_http_html_sanitize_document_visit(r, sc, output->document);
        if (rc == NGX_ERROR) {
            return NGX_ERROR;
        }

    } else {
        rc = ngx_http_html_sanitize_build_first_chain(r, sc);
        if (rc == NGX_ERROR) {
            return NGX_ERROR;
        }
    }


    rc = ngx_http_html_sanitize_root_visit(r, sc, output->root);
    if (rc == NGX_ERROR) {
        return NGX_ERROR;
    }

    gumbo_destroy_output(&options, output);

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_build_first_chain(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc)
{
    ngx_buf_t   *buf;
    ngx_chain_t *cc;

    buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL) {
        return NGX_ERROR;
    }

    buf->pos = (void *) "";
    buf->last = buf->pos;
    buf->last_buf = 1;
    buf->memory = 1;

    cc = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
    if (cc == NULL) {
        return NGX_ERROR;
    }

    cc->buf = buf;
    cc->next = NULL;

    sc->head = cc;
    sc->tail = cc;
    sc->head->next = NULL;

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_document_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node)
{
    u_char        *p, *last;
    ngx_buf_t     *buf;
    ngx_int_t      len;
    ngx_chain_t   *cc;
    GumboDocument *doc;

    doc = &node->v.document;
    if (doc->has_doctype && doc->public_identifier != NULL) {
        /*
            "<! DOCTYPE %s PUBLIC \"%s\" \"%s\">\n"
            doc->name, doc->public_identifier, doc->system_identifier);
        */

        len = sizeof("<!DOCTYPE ") - 1
              + strlen(doc->name)
              + sizeof(" PUBLIC \"") - 1
              + strlen(doc->public_identifier)
              + sizeof("\" \"") - 1
              + strlen(doc->system_identifier)
              + sizeof("\">\n") - 1;
        p = ngx_pcalloc(r->pool, len);
        if (p == NULL) {
            return NGX_ERROR;
        }

        last = ngx_snprintf(p, len, "<!DOCTYPE %s PUBLIC \"%s\" \"%s\">\n",
            doc->name, doc->public_identifier, doc->system_identifier);

    } else {
        len = sizeof("<!DOCTYPE HTML ") - 1
              + strlen(doc->name)
              + sizeof(">\n") - 1;

        p = ngx_pcalloc(r->pool, len);
        if (p == NULL) {
            return NGX_ERROR;
        }

        last = ngx_snprintf(p, len, "<!DOCTYPE HTML %s>\n", doc->name);
    }

    buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL) {
        return NGX_ERROR;
    }

    buf->pos = p;
    buf->last = last;
    buf->last_buf = 1;
    buf->memory = 1;

    cc = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
    if (cc == NULL) {
        return NGX_ERROR;
    }

    cc->buf = buf;
    cc->next = NULL;

    sc->head = cc;
    sc->tail = cc;
    sc->head->next = NULL;

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_root_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node)
{
    switch(node->type) {
        case GUMBO_NODE_ELEMENT:
        case GUMBO_NODE_TEMPLATE:
            return ngx_http_html_sanitize_element_visit(r, sc, node);
        case GUMBO_NODE_TEXT:
            return ngx_http_html_sanitize_text_visit(r, sc, node);
        case GUMBO_NODE_WHITESPACE:
            return ngx_http_html_sanitize_whitespace_visit(r, sc, node);
        case GUMBO_NODE_COMMENT:
        case GUMBO_NODE_CDATA:
            break;
        default:
            break;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_style_declration_property_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, const char *quote,
    KatanaDeclaration *declaration)
{
    size_t       len;
    u_char      *p, *last;
    ngx_buf_t   *buf;
    ngx_chain_t *cc;

    if (declaration->important) {

        /*
        * "%s:%s !important;";
        */

        len = strlen(declaration->property) + sizeof(":") - 1
            + strlen(declaration->raw) + sizeof(" !important;") - 1;
        p = ngx_pcalloc(r->pool, len);
        if (p == NULL) {
            return NGX_ERROR;
        }

        last = ngx_snprintf(p, len, "%s:%s !important;",
                            declaration->property,
                            declaration->raw);
    } else {

        /*
        * "%s:%s;";
        */

        len = strlen(declaration->property) + sizeof(":") - 1
            + strlen(declaration->raw) + sizeof(";") - 1;
        p = ngx_pcalloc(r->pool, len);
        if (p == NULL) {
            return NGX_ERROR;
        }

        last = ngx_snprintf(p, len, "%s:%s;",
                            declaration->property,
                            declaration->raw);
    }

    ngx_http_html_sanitize_style_quote_replace(p, (last - p), quote);

    buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL) {
        return NGX_ERROR;
    }

    buf->pos = p;
    buf->last = last;
    buf->last_buf = 1;
    buf->memory = 1;

    cc = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
    if (cc == NULL) {
        return NGX_ERROR;
    }

    cc->buf = buf;
    cc->next = NULL;
    sc->tail->next = cc;
    sc->tail = cc;

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_style_declration_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node, ngx_str_t *tagname,
    GumboAttribute *attr, const char *quote, KatanaDeclaration *declaration)
{
    size_t                             len;
    void                              *find;
    u_char                            *p, *ub, *ue;
    ngx_int_t                          i, rc;
    ngx_uint_t                         k;
    KatanaArray                       *values;
    KatanaValue                       *value;
    ngx_http_html_sanitize_ctx_t      *ctx;
    ngx_http_html_sanitize_loc_conf_t *gslf;

    gslf = ngx_http_get_module_loc_conf(r, ngx_http_html_sanitize_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);

    if (ctx->style_property == 2) {
        if (gslf->style_properties && gslf->style_properties->nelts > 0) {
            gslf = ngx_http_get_module_loc_conf(r, ngx_http_html_sanitize_module);
            ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);
            len = strlen(declaration->property);
            k = ngx_hash_key_lc((u_char *) declaration->property, len);
            find = ngx_hash_find(gslf->sph.hash, k,
                                (u_char *)declaration->property, len);
            if (find == NULL) {
                return NGX_OK;
            }

        } else {
            return NGX_OK;
        }
    }

    if (ctx->style_property_value == 0) {
        rc = ngx_http_html_sanitize_style_declration_property_visit(r, sc, quote,
                                                                    declaration);
        return rc;
    }

    values = declaration->values;
    for (i = 0; i < values->length; i++) {
        value = values->data[i];

        dd("style property %s value unit:%u",
           declaration->property, value->unit);

        switch(value->unit) {
            case KATANA_VALUE_URI:
                if (ngx_http_html_sanitize_url_absolute(value->string)) {
                    if (gslf->protocols && gslf->protocols->nelts > 0) {
                        p = (u_char *) value->string;
                        while (*p++ != ':') { /* void */ }
                        len = p - 1 - (u_char *) value->string;
                        k = ngx_hash_key_lc((u_char *) value->string, len);
                        find = ngx_hash_find(gslf->ph.hash, k,
                                                (u_char *)value->string, len);
                        if (find == NULL) {
                            return NGX_OK;
                        }

                        if (ctx->url_domain == 0) {
                            continue;
                        }

                        if (!(gslf->domains && gslf->domains->nelts > 0)) {
                            continue;
                        }

                        rc = ngx_http_html_sanitize_lookup_url_domain(
                                value->string, &ub, &ue);
                        if (rc != NGX_OK) {
                            return NGX_OK;
                        }

                        dd("style url domain: %.*s", (int) (ue - ub + 1), ub);

                        len = ue - ub + 1;
                        k = ngx_hash_key_lc(ub, len);
                        find = ngx_hash_find_combined(&(gslf->cdh), k, ub, len);
                        if (find == NULL) {
                            return NGX_OK;
                        }
                    }
                }
                break;

            case KATANA_VALUE_PARSER_FUNCTION:
                if (ngx_http_html_sanitize_is_style_expression(
                        value->function->name))
                {
                    /*
                        * skip expression declaration
                        */
                        return NGX_OK;
                }
                break;

            default:
                break;
        }
    }

    return ngx_http_html_sanitize_style_declration_property_visit(r, sc,
                                                                  quote,
                                                                  declaration);
}


static ngx_int_t
ngx_http_html_sanitize_style_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node, ngx_str_t *tagname,
    GumboAttribute *attr, const char *quote)
{
    u_char                            *p;
    size_t                             len;
    ngx_int_t                          i, rc;
    KatanaOutput                      *output;
    KatanaDeclaration                 *declaration;

    len = sizeof(" ") - 1
        + strlen(attr->name)
        + sizeof("=") - 1
        + strlen(quote)
        + 1;
    p = ngx_pcalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ngx_snprintf(p, len - 1, " %s=%s", attr->name, quote);
    p[len] = 0;

    rc = ngx_http_html_sanitize_append_string_to_chain(r, sc, (const char *) p);
    if (rc != NGX_OK) {
        return rc;
    }

    output = katana_parse(attr->value, strlen(attr->value),
                          KatanaParserModeDeclarationList);

    dd("<%.*s>: style declartions length:%d",
        (int) tagname->len, tagname->data, output->declarations->length);

    for (i = 0; i < output->declarations->length; i++) {
        declaration = output->declarations->data[i];
        rc = ngx_http_html_sanitize_style_declration_visit(r, sc, node,
                                                      tagname, attr, quote,
                                                      declaration);
        if (rc != NGX_OK) {
            katana_destroy_output(output);
            return rc;
        }
    }

    katana_destroy_output(output);

    rc = ngx_http_html_sanitize_append_string_to_chain(r, sc, quote);
    if (rc != NGX_OK) {
        return rc;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_attribute_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node,
    GumboAttribute *attr, ngx_str_t *tagname, ngx_int_t no_entities)
{
    char                                first;
    u_char                             *p, *value, *last, *find, *ub, *ue;
    size_t                              attrlen, valuelen;
    ngx_int_t                           rc, len;
    ngx_uint_t                          k;
    ngx_buf_t                          *buf;
    const char                         *quote;
    ngx_chain_t                        *cc;
    ngx_http_html_sanitize_ctx_t       *ctx;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    attrlen = strlen(attr->name);
    gslf = ngx_http_get_module_loc_conf(r, ngx_http_html_sanitize_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);

    if (attr->name == NULL || attr->value == NULL) {
        return NGX_OK;
    }

    /*
     * 0: disable attribute
     * 1: allow all attribute
     * 2: allow whitelist attribute only
     */

    if (ctx->attribute == 0) {
        return NGX_OK;
    }

    /*
     * 0: dont check url protocol
     * 1: check url protocol
     */

    if (ctx->url_protocol == 0) {
        goto LINK_ATTR_CHECKED;
    }

    if (ngx_http_html_sanitize_link_attr(node->v.element.tag,
                                         attr->name))
    {
        if (ngx_http_html_sanitize_url_absolute(attr->value)) {
            if (gslf->protocols && gslf->protocols->nelts > 0) {
                p = (u_char *) attr->value;
                while (*p++ != ':') { /* void */ }
                len = p - 1 - (u_char *) attr->value;
                k = ngx_hash_key_lc((u_char *) attr->value, len);
                find = ngx_hash_find(gslf->ph.hash, k,
                                        (u_char *)attr->value, len);
                if (find == NULL) {
                    return NGX_OK;
                }

                /*
                 * require to skip url domain check
                 */

                if (ctx->url_domain == 0) {
                    goto LINK_ATTR_CHECKED;
                }

                if (!(gslf->domains && gslf->domains->nelts > 0)) {
                    return NGX_OK;
                }

                rc = ngx_http_html_sanitize_lookup_url_domain(attr->value,
                                                              &ub, &ue);
                if (rc != NGX_OK) {
                    return NGX_OK;
                }

                dd("html url domain: %.*s", (int) (ue - ub + 1), ub);

                len = ue - ub + 1;
                k = ngx_hash_key_lc(ub, len);
                find = ngx_hash_find_combined(&(gslf->cdh), k, ub, len);
                if (find == NULL) {
                    return NGX_OK;
                }

            } else {
                return NGX_OK;
            }
        }
    }

LINK_ATTR_CHECKED:

    /*
     * specially treat iframe.src
     * 0: dont check url protocol
     * 1: check url protocol
     */

    if (ctx->iframe_url_protocol == 0) {
        goto IFRAME_SRC_CHECKED;
    }

    if (ngx_http_html_sanitize_is_iframe_src(node->v.element.tag, attr->name)) {
        if (ngx_http_html_sanitize_url_absolute(attr->value)) {
            if (gslf->iframe_protocols && gslf->iframe_protocols->nelts > 0) {
                p = (u_char *) attr->value;
                while (*p++ != ':') { /* void */ }
                len = p - 1 - (u_char *) attr->value;
                k = ngx_hash_key_lc((u_char *) attr->value, len);
                find = ngx_hash_find(gslf->iph.hash, k,
                                        (u_char *)attr->value, len);
                if (find == NULL) {
                    return NGX_OK;
                }

                /*
                 * require to skip iframe url domain check
                 */

                if (ctx->iframe_url_domain == 0) {
                    goto IFRAME_SRC_CHECKED;
                }

                if (!(gslf->iframe_domains
                    && gslf->iframe_domains->nelts > 0))
                {
                    return NGX_OK;
                }

                rc = ngx_http_html_sanitize_lookup_url_domain(attr->value,
                                                              &ub, &ue);
                if (rc != NGX_OK) {
                    return NGX_OK;
                }

                dd("iframe html url domain: %.*s", (int) (ue - ub + 1), ub);

                len = ue - ub + 1;
                k = ngx_hash_key_lc(ub, len);
                find = ngx_hash_find_combined(&(gslf->icdh), k, ub, len);
                if (find == NULL) {
                    return NGX_OK;
                }

            } else {
                return NGX_OK;
            }
        }
    }

IFRAME_SRC_CHECKED:

    /*
     * attr whitelist
     */

    if (ctx->attribute == 2) {
        if (gslf->attributes && gslf->attributes->nelts > 0)
        {

            /*
             * Fix Me: use better algo to instead of
             * hash find to avoid memory allocate
             */

            len = tagname->len + sizeof(".") - 1 + attrlen;
            p = ngx_pcalloc(r->pool, len);
            if (p == NULL) {
                return NGX_ERROR;
            }

            ngx_memcpy(p, tagname->data, tagname->len);
            p[tagname->len] = '.';
            ngx_memcpy(p + tagname->len + 1, attr->name, attrlen);

            k = ngx_hash_key_lc(p, len);

            find = ngx_hash_find_combined(&(gslf->cah), k, p, len);

            dd("%.*s attr:%s k:%u find:%p", (int) tagname->len,
               tagname->data, attr->name, (unsigned) k, find);

            if (find == NULL) {
                return NGX_OK;
            }
        } else {

            return NGX_OK;
        }
    }

    if (attr->value != NULL
        || attr->original_value.data[0] == '"'
        || attr->original_value.data[0] == '\'')
    {
        first = attr->original_value.data[0];
        if (first == '\'') {
            quote = "\'";
        } else if (first == '\"') {
            quote = "\"";
        } else {
            quote = "";
        }

        if (no_entities) {
            value = (u_char *) attr->value;

        } else {
            rc = ngx_http_html_sanitize_substitute_xml_entities_into_text(r,
                                    (u_char *) attr->value, &value, quote[0]);
            if (rc == NGX_ERROR) {
                return NGX_ERROR;
            }
        }

        /*
        * 0: disable style property
        * 1: allow all style property
        * 2: allow whitelist style property only
        */

        if (ngx_http_html_sanitize_is_style_attribute(attr->name) == 1) {
            if (ctx->style_property == 0) {
                return NGX_OK;

            } else if (!(ctx->style_property == 1
                       && ctx->style_property_value == 0)
                      )
            {
                return ngx_http_html_sanitize_style_visit(r, sc, node, tagname,
                                                          attr, quote);
            }
        }

        valuelen = strlen((const char *)value);

        ngx_http_html_sanitize_style_quote_replace(value, valuelen, quote);

        len = sizeof(" ") - 1
              + strlen(attr->name)
              + sizeof("=") - 1
              + strlen(quote)
              + valuelen
              + strlen(quote);

        p = ngx_pcalloc(r->pool, len);
        if (p == NULL) {
            return NGX_ERROR;
        }

        last = ngx_snprintf(p, len, " %s=%s%s%s", attr->name, quote, value,
                            quote);

        buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
        if (buf == NULL) {
            return NGX_ERROR;
        }

        buf->pos = p;
        buf->last = last;
        buf->last_buf = 1;
        buf->memory = 1;

        cc = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
        if (cc == NULL) {
            return NGX_ERROR;
        }

        cc->buf = buf;
        cc->next = NULL;
        sc->tail->next = cc;
        sc->tail = cc;

        return NGX_OK;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_element_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node)
{
    ngx_str_t     tagname = ngx_null_string;

    char                               *open_right_tag,
                                       *open_left_tag, *close_tag;
    void                               *find;
    ngx_int_t                           i, rc, is_empty_tag;
    ngx_int_t                           outputtag;
    ngx_uint_t                          k;
    GumboNode                          *cnode;
    GumboVector                        *children;
    GumboVector                        *attrs;
    GumboElement                       *ele;
    ngx_http_html_sanitize_ctx_t       *ctx;
    ngx_http_html_sanitize_loc_conf_t  *gslf;

    gslf = ngx_http_get_module_loc_conf(r, ngx_http_html_sanitize_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_html_sanitize_module);
    ele = &(node->v.element);
    children = &(ele->children);
    attrs = &(ele->attributes);

    /*
     * 0: disable elements
     * 1: allow all elements
     * 2: allow whitelist elements only
     */

    if (ctx->element == 0) {
        return NGX_OK;
    }

    rc = ngx_http_html_sanitize_get_tag_name(r, node, &tagname);
    if (rc == NGX_ERROR) {
        return NGX_ERROR;
    }

    dd("tagname: %d %.*s", (int) tagname.len, (int) tagname.len, tagname.data);

    if (tagname.len == 0) {
        return NGX_OK;
    }

    if (ctx->element == 2) {
        if (gslf->elements && gslf->elements->nelts > 0) {

            k = ngx_hash_key_lc(tagname.data, tagname.len);
            find = ngx_hash_find(gslf->eh.hash, k, tagname.data, tagname.len);
            if (find == NULL) {
                return NGX_OK;
            }

        } else {
            return NGX_OK;
        }
    }

    /*
     * Explicitly disable script and style element
     */

    if (ctx->script == 0) {
        if (node->v.element.tag == GUMBO_TAG_SCRIPT) {
            return NGX_OK;
        }
    }

    if (ctx->style == 0) {
        if (node->v.element.tag == GUMBO_TAG_STYLE) {
            return NGX_OK;
        }
    }

    open_left_tag = ngx_pcalloc(r->pool, 1 + tagname.len + 1);
    if (open_left_tag == NULL) {
        return NGX_ERROR;
    }
    ngx_memcpy(open_left_tag, "<", 1);
    ngx_memcpy(open_left_tag + 1, tagname.data, tagname.len);
    open_left_tag[1 + tagname.len] = '\0';

    if (node->v.element.tag == GUMBO_TAG_HTML && ctx->html == 0) {
        outputtag = 0;
    } else {
        outputtag = 1;
    }

    if (outputtag == 1) {
        rc = ngx_http_html_sanitize_append_string_to_chain(r, sc,
                                                            open_left_tag);
        if (rc == NGX_ERROR) {
            return NGX_ERROR;
        }
    }

    for (i = 0; i < attrs->length; i++) {
        rc = ngx_http_html_sanitize_attribute_visit(r, sc, node,
                                                     attrs->data[i],
                                                     &tagname, 1);
        if (rc == NGX_ERROR) {
            return NGX_ERROR;
        }
    }

    is_empty_tag = ngx_http_html_sanitize_void_tag(node->v.element.tag);
    if (is_empty_tag) {
        open_right_tag = " />";
        close_tag = "";
    } else {
        open_right_tag = ">";
        close_tag = ngx_pcalloc(r->pool, 2 + tagname.len + 1 + 1);
        if (close_tag == NULL) {
            return NGX_ERROR;
        }

        ngx_snprintf((u_char *)close_tag, 2 + tagname.len + 1 + 1, "</%V>",
                     &tagname);
    }

    if (outputtag == 1) {
        /* output open_right_tag */
        rc = ngx_http_html_sanitize_append_string_to_chain(r, sc,
                                                            open_right_tag);
        if (rc == NGX_ERROR) {
            return NGX_ERROR;
        }
    }

    for (i = 0; i < children->length; i++) {
        cnode = children->data[i];
        rc = ngx_http_html_sanitize_root_visit(r, sc, cnode);
        if (rc == NGX_ERROR) {
            return NGX_ERROR;
        }
    }

    /* output close_tag */
    if (!is_empty_tag) {
        if (outputtag == 1) {
            rc = ngx_http_html_sanitize_append_string_to_chain(r, sc,
                                                                close_tag);
            if (rc == NGX_ERROR) {
                return NGX_ERROR;
            }
        }
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_get_tag_name(ngx_http_request_t *r, GumboNode *node,
    ngx_str_t *value)
{
    GumboStringPiece clone;

    value->data = NULL;
    value->len = 0;

    if (node->type == GUMBO_NODE_DOCUMENT) {
        value->data = (u_char *) "document";
        value->len = sizeof("document") - 1;

    } else {

        if (node->v.element.tag < GUMBO_TAG_UNKNOWN) {
            value->data = (u_char *) gumbo_normalized_tagname(
                                        node->v.element.tag);
            if (value->data != NULL) {
                value->len = strlen((const char *) value->data);
            }
        }
    }

    if (value->data == NULL || value->len == 0) {
        clone = node->v.element.original_tag;
        gumbo_tag_from_original_text(&clone);

        value->data = ngx_pcalloc(r->pool, clone.length);
        if (value->data == NULL) {
            return NGX_ERROR;
        }

        ngx_memcpy(value->data, clone.data, clone.length);
        value->len = clone.length;
    }

    if (value->data == NULL) {
        value->data = (u_char *) "";
        value->len = 0;
    }

    return NGX_OK;
}


static inline ngx_int_t
ngx_http_html_sanitize_link_attr(GumboTag tag, const char *attr)
{
    switch (tag) {
        case GUMBO_TAG_A:
            if (ngx_strncasecmp((u_char *) attr, (u_char *) "href", 4) == 0) {
                return 1;
            }
            break;
        case GUMBO_TAG_BLOCKQUOTE:
            if (ngx_strncasecmp((u_char *) attr, (u_char *) "cite", 10) == 0) {
                return 1;
            }
            break;
        case GUMBO_TAG_Q:
            if (ngx_strncasecmp((u_char*) attr, (u_char *) "cite", 4) == 0) {
                return 1;
            }
            break;
        case GUMBO_TAG_DEL:
            if (ngx_strncasecmp((u_char *) attr, (u_char *) "cite", 4) == 0) {
                return 1;
            }
            break;
        case GUMBO_TAG_IMG:
            if (ngx_strncasecmp((u_char *) attr, (u_char *) "src", 3) == 0) {
                return 1;
            }
            break;
        case GUMBO_TAG_INS:
            if (ngx_strncasecmp((u_char *) attr, (u_char *) "cite", 4) == 0) {
                return 1;
            }
            break;
        default:
            break;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_is_iframe_src(GumboTag tag, const char *attr)
{
    if (tag == GUMBO_TAG_IFRAME && strlen(attr) == 3
        &&
            (
                (
                    attr[0] == 's' || attr[0] == 'S'
                )
                &&
                (
                    attr[1] == 'r' || attr[1] == 'R'
                )
                &&
                (
                    attr[2] == 'c' || attr[2] == 'C'
                )
            )
        )
    {
        return 1;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_entity_substitution(GumboTag tag)
{
    switch (tag) {
        case GUMBO_TAG_SCRIPT:
        case GUMBO_TAG_STYLE:
            return 1;
        default:
            break;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_void_tag(GumboTag tag)
{
    /*
     * http://w3c.github.io/html-reference/syntax.html#void-element
     */

    switch (tag) {
        case GUMBO_TAG_AREA:
        case GUMBO_TAG_BASE:
        case GUMBO_TAG_BASEFONT:
        case GUMBO_TAG_BGSOUND:
        case GUMBO_TAG_BR:
        case GUMBO_TAG_COL:
        case GUMBO_TAG_EMBED:
        case GUMBO_TAG_FRAME:
        case GUMBO_TAG_HR:
        case GUMBO_TAG_IMAGE:
        case GUMBO_TAG_IMG:
        case GUMBO_TAG_INPUT:
        case GUMBO_TAG_KEYGEN:
        case GUMBO_TAG_LINK:
        case GUMBO_TAG_MENUITEM:
        case GUMBO_TAG_META:
        case GUMBO_TAG_PARAM:
        case GUMBO_TAG_SOURCE:
        case GUMBO_TAG_SPACER:
        case GUMBO_TAG_TRACK:
        case GUMBO_TAG_WBR:
            return 1;
        default:
            break;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_inline_tag(GumboTag tag)
{
    switch (tag) {
        case GUMBO_TAG_A:
        case GUMBO_TAG_ABBR:
        case GUMBO_TAG_ACRONYM:
        case GUMBO_TAG_B:
        case GUMBO_TAG_BDO:
        case GUMBO_TAG_BIG:
        case GUMBO_TAG_CITE:
        case GUMBO_TAG_CODE:
        case GUMBO_TAG_DFN:
        case GUMBO_TAG_EM:
        case GUMBO_TAG_FONT:
        case GUMBO_TAG_I:
        case GUMBO_TAG_IMG:
        case GUMBO_TAG_KBD:
        case GUMBO_TAG_NOBR:
        case GUMBO_TAG_S:
        case GUMBO_TAG_SMALL:
        case GUMBO_TAG_SPAN:
        case GUMBO_TAG_STRIKE:
        case GUMBO_TAG_STRONG:
        case GUMBO_TAG_SUB:
        case GUMBO_TAG_SUP:
        case GUMBO_TAG_TT:
            return 1;
        default:
            break;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_whitespace_tag(GumboTag tag)
{
    switch (tag) {
        case GUMBO_TAG_PRE:
        case GUMBO_TAG_TEXTAREA:
        case GUMBO_TAG_SCRIPT:
        case GUMBO_TAG_STYLE:
            return 1;
        default:
            break;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_is_style_attribute(const char *name)
{
    if (strlen(name) == sizeof("style") - 1
        &&
        (
            (name[0] == 's' || name[0] == 'S')
            &&
            (name[1] == 't' || name[1] == 'T')
            &&
            (name[2] == 'y' || name[2] == 'Y')
            &&
            (name[3] == 'l' || name[3] == 'L')
            &&
            (name[4] == 'e' || name[4] == 'E')
        )
    )
    {
        return 1;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_is_style_expression(const char *name)
{
    size_t len;

    len = strlen(name);
    if ((len == sizeof("expression(") - 1)
        &&
        (
            (name[0] == 'e' || name[0] == 'E')
            &&
            (name[1] == 'x' || name[1] == 'X')
            &&
            (name[2] == 'p' || name[2] == 'P')
            &&
            (name[3] == 'r' || name[3] == 'R')
            &&
            (name[4] == 'e' || name[4] == 'E')
            &&
            (name[5] == 's' || name[5] == 'S')
            &&
            (name[6] == 's' || name[6] == 'S')
            &&
            (name[7] == 'i' || name[7] == 'I')
            &&
            (name[8] == 'o' || name[8] == 'O')
            &&
            (name[9] == 'n' || name[9] == 'N')
            &&
            (name[10] == '(')
        )
       )
    {
        return 1;
    }

    return 0;
}


static inline ngx_int_t
ngx_http_html_sanitize_substitute_xml_entities_into_text(ngx_http_request_t *r,
    u_char *str, u_char **dst, char quote)
{
    u_char   *p, *pv, *value;
    ngx_int_t i, size, len;

    size = (ngx_int_t) strlen((const char *)str);

    for (i = 0, len = 0; i < size; i++) {
         p = str + i;

        switch(*p) {
            case '&':
                //"&amp;"
                len += 5;
            case '<':
                //"&lt;"
                len +=  4;
            case '>':
                //"&gt;"
                len +=  4;
            case '\'':
                //"&apos;"
                if (quote == '\'') {
                    len += 5;
                } else {
                    len += 1;
                }
            case '\"':
                 //"&quot;"
                if (quote == '\"') {
                    len += 5;
                } else {
                    len += 1;
                }
            default:
                len += 1;
                break;
        }
    }

    value = ngx_pcalloc(r->pool, len + 1);
    if (value == NULL) {
        return NGX_ERROR;
    }

    value[len] = '\0';
    pv = value;

    for (i = 0; i < size; i++) {
        p = (u_char *) str + i;

        switch(*p) {
            case '&':
                //"&amp;"
                pv = ngx_copy(pv, "&amp;", 5);
                break;
            case '<':
                //"&lt;"
                pv = ngx_copy(pv, "&lt;", 4);
                break;
            case '>':
                //"&gt;"
                pv = ngx_copy(value, "&gt;", 4);
                break;
            case '\'':
                //"&apos;"
                if (quote == '\'') {
                    pv = ngx_copy(value, "&apos;", 5);
                } else {
                    *pv++ = *p;
                }
                break;
            case '\"':
                 //"&quot;"
                if (quote == '\"') {
                    pv = ngx_copy(value, "&quot;", 5);
                } else {
                    *pv++ = *p;
                }
                break;
            default:
                *pv++ = *p;
                break;
        }
    }

    *dst = value;

    return NGX_OK;
}


static inline void
ngx_http_html_sanitize_rtrim(ngx_str_t *value)
{
    u_char *p;

    if (value->len == 0) {
        return;
    }

    p = value->data + value->len;
    while (*p++ == ' ') {
        value->len --;
    }
}


static inline void
ngx_http_html_sanitize_style_quote_replace(u_char *p, size_t len,
    const char *quote)
{
    ngx_uint_t i;

    for (i = 0; i < len; i++) {

        /*
         * It's trick to replace ' or " to right quote in inline CSS Style
         */

        switch (p[i]) {
            case '"':
                if (quote[0] == '"') {
                    p[i] = '\'';
                }
                break;
            case '\'':
                if (quote[0] == '\'') {
                    p[i] = '\"';
                }

                break;
        }
    }
}


static ngx_int_t
ngx_http_html_sanitize_text_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node)
{
    u_char      *value, *p;
    ngx_int_t    rc, entity_substitution, len;
    ngx_str_t    tagname, text;
    ngx_buf_t   *buf;
    ngx_chain_t *cc;

    rc = ngx_http_html_sanitize_get_tag_name(r, node->parent, &tagname);
    if (rc == NGX_ERROR) {
        return NGX_ERROR;
    }

    /*
     * FIX ME: xml_entities
     */

    if (0) {
        if (tagname.len > 0) {
            entity_substitution =
                ngx_http_html_sanitize_entity_substitution(node->v.element.tag);
            if (entity_substitution == 0) {
                value = (u_char *) node->v.text.text;

            } else {
                rc = ngx_http_html_sanitize_substitute_xml_entities_into_text(r,
                    (u_char *) (node->v.text.text), &value, 0);
                if (rc == NGX_ERROR) {
                    return NGX_ERROR;
                }
            }

        } else {
            value = (u_char *) node->v.text.text;
        }

    } else {
        value = (u_char *) node->v.text.text;
    }

    /*
     * Do not use text because of html entities
     */

    text.len = node->v.text.original_text.length;
    text.data = (u_char *) node->v.text.original_text.data;

    dd("tagname:%.*s-text:%.*s origintext-:%.*s", (int) tagname.len,
        tagname.data,
        (int) node->v.text.len, node->v.text.data,
        (int) node->v.text.original_text.length,
        node->v.text.original_text.data);

    ngx_http_html_sanitize_rtrim(&text);

    len = text.len;

    p = ngx_pcalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(p, text.data, text.len);

    buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL) {
        return NGX_ERROR;
    }

    buf->pos = p;
    buf->last = p + text.len;
    buf->last_buf = 1;
    buf->memory = 1;

    cc = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
    if (cc == NULL) {
        return NGX_ERROR;
    }

    cc->buf = buf;
    cc->next = NULL;
    sc->tail->next = cc;
    sc->tail = cc;

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_whitespace_visit(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, GumboNode *node)
{
    u_char      *p;
    size_t       len;
    ngx_int_t    keep_whitespace, is_inline;
    ngx_buf_t   *buf;
    ngx_chain_t *cc;

    keep_whitespace = ngx_http_html_sanitize_whitespace_tag(node->v.element.tag);
    is_inline = ngx_http_html_sanitize_inline_tag(node->v.element.tag);

    if (!keep_whitespace && !is_inline) {
        return NGX_OK;
    }

    len = strlen(node->v.text.text);

    p = ngx_pcalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(p, node->v.text.text, len);

    buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL) {
        return NGX_ERROR;
    }

    buf->pos = p;
    buf->last = p + len;
    buf->last_buf = 1;
    buf->memory = 1;

    cc = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
    if (cc == NULL) {
        return NGX_ERROR;
    }

    cc->buf = buf;
    cc->next = NULL;
    sc->tail->next = cc;
    sc->tail = cc;

    return NGX_OK;
}


static ngx_int_t
ngx_http_html_sanitize_append_string_to_chain(ngx_http_request_t *r,
    ngx_http_html_sanitize_chain_t *sc, const char *str)
{

    u_char      *p;
    ngx_int_t    len;
    ngx_buf_t   *buf;
    ngx_chain_t *cc;

    len = strlen(str);
    p = ngx_pcalloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(p, str, len);

    buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL) {
        return NGX_ERROR;
    }

    buf->pos = p;
    buf->last = p + len;
    buf->last_buf = 1;
    buf->memory = 1;

    cc = ngx_pcalloc(r->pool, sizeof(ngx_chain_t));
    if (cc == NULL) {
        return NGX_ERROR;
    }

    cc->buf = buf;
    cc->next = NULL;
    sc->tail->next = cc;
    sc->tail = cc;

    return NGX_OK;
}


static inline ngx_int_t
ngx_http_html_sanitize_url_absolute(const char *url)
{
    const char *p;

    p = url;
    while(*p) {
        if (*p == ':') {
            return 1;
        } else if (*p == '/' || *p == '?' || *p == '#') {
            return 0;
        }
        p++;
    }

    return 0;
}


static ngx_int_t
ngx_http_html_sanitize_lookup_url_domain(const char *url, u_char **begin,
    u_char **end)
{
    u_char *p;

    p = (u_char *) url;

    /*
     * lookup ://
     */

    while (*p && *p++ != ':') { /* void */ }
    if (*(p + 1) != '/' || *(p + 2) == '/') {
        return NGX_ERROR;
    }

    *begin = p + 2;

    /*
     * lookup /
     */

    p = *begin;

    for (;*p; p++) {
        switch (*p) {
            case '/':
            case ':':
            case '#':
            case '?':
                *end = p - 1;
                return NGX_OK;
            default:
                break;
        }
    }

    *end = p - 1;

    return NGX_OK;
}


static void *
ngx_http_html_sanitize_allocator(void* userdata, size_t size)
{
    return malloc(size);
}


static void
ngx_http_html_sanitize_deallocator(void* userdata, void* ptr)
{
    free(ptr);
}
