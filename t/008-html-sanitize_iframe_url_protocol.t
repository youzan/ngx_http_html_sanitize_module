use lib 'lib';
use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * blocks() * 2;

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: html_sanitize_iframe_url_protocol empty
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http https;
}
--- request eval
"POST /t?element=1&attribute=1
<iframe src=\"https://www.google.com\"></iframe>
"
--- response_body: <iframe src="https://www.google.com"></iframe>



=== TEST 2: html_sanitize_url_protocol http
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1
<iframe src=\"https://www.google.com\"></iframe>
"
--- response_body: <iframe></iframe>



=== TEST 3: html_sanitize_url_protocol https
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http;
    html_sanitize_iframe_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1
<iframe src=\"https://www.google.com\"></iframe>
"
--- response_body: <iframe src="https://www.google.com"></iframe>



=== TEST 4: html_sanitize_url_protocol javascript
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https http;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1
<iframe src=\"javasciprt:alert(1)\"></iframe>
"
--- response_body: <iframe></iframe>



=== TEST 5: html_sanitize_url_protocol ftp
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol https http;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1
<iframe src=\"ftp://1\"></iframe>
"
--- response_body: <iframe></iframe>
