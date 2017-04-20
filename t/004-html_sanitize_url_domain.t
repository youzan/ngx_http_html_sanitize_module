use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * blocks() * 2;

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: html_sanitize_url_domain empty
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
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<a href=\"https://www.google.com\">a</a>
"
--- response_body: <a>a</a>



=== TEST 2: html_sanitize_url_domain enable www.google.com
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element a;
    html_sanitize_attribute a.href;
    html_sanitize_url_protocol https;
    html_sanitize_url_domain www.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<a href=\"https://www.google.com\">a</a>
"
--- response_body: <a href="https://www.google.com">a</a>



=== TEST 3: html_sanitize_url_domain enable *.google.com
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element a;
    html_sanitize_attribute a.href;
    html_sanitize_url_protocol https;
    html_sanitize_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<a href=\"https://www.google.com\">a</a>
"
--- response_body: <a href="https://www.google.com">a</a>



=== TEST 4: html_sanitize_url_domain enable *.google.com
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element a;
    html_sanitize_attribute a.href;
    html_sanitize_url_protocol https;
    html_sanitize_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<a href=\"https://www.facebook.com\">a</a>
"
--- response_body: <a>a</a>



=== TEST 5: html_sanitize_url_domain enable *.google.com and style_property
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https;
    html_sanitize_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1&style_property=1&style_property_value=1
<a style=\"background-color:url(https://www.facebook.com);\">a</a>
"
--- response_body: <a style="">a</a>



=== TEST 6: html_sanitize_url_domain enable *.google.com and style_property
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https;
    html_sanitize_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1&style_property=1&style_property_value=1
<a style=\"background-color:url(https://www.google.com);\">a</a>
"
--- response_body: <a style="background-color:url(https://www.google.com);">a</a>



=== TEST 7: html_sanitize_url_domain enable *.google.com and style_property
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https;
    html_sanitize_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1&style_property=1&style_property_value=1
<a style=\"background-color:url(https://google.com);\">a</a>
"
--- response_body: <a style="">a</a>
