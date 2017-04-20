use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * blocks() * 2;

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: html_sanitize_url_protocol empty
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element a;
    html_sanitize_attribute a.href;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<a href=\"http://www.google.com\">a</a>
"
--- response_body: <a>a</a>



=== TEST 2: html_sanitize_url_protocol http
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element a;
    html_sanitize_attribute a.href;
    html_sanitize_url_protocol http;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1
<a href=\"http://www.google.com\">a</a>
"
--- response_body: <a href="http://www.google.com">a</a>



=== TEST 3: html_sanitize_url_protocol https
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element a;
    html_sanitize_attribute a.href;
    html_sanitize_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1
<a href=\"http://www.google.com\">a</a>
"
--- response_body: <a>a</a>



=== TEST 4: html_sanitize_url_protocol https
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element a;
    html_sanitize_attribute a.href;
    html_sanitize_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1
<a href=\"https://www.google.com\">a</a>
"
--- response_body: <a href="https://www.google.com">a</a>



=== TEST 5: html_sanitize_url_protocol and style property value
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=1&style_property_value=1&url_protocol=1
<a style=\"backaground-image:url(http://www.google.com);\">a</a>
"
--- response_body: <a style="">a</a>



=== TEST 6: html_sanitize_url_protocol and style property value
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=1&style_property_value=1&url_protocol=1
<a style=\"backaground-image:url(https://www.google.com);\">a</a>
"
--- response_body: <a style="backaground-image:url(https://www.google.com);">a</a>



=== TEST 7: html_sanitize_url_protocol and style property value javascript:
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=1&style_property_value=1&url_protocol=1
<a style=\"backaground-image:url(javascript:alert(1));\">a</a>
"
--- response_body: <a style="">a</a>
