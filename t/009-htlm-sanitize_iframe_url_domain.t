use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * blocks() * 2;

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: html_sanitize_iframe_url_domain empty
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1
<iframe src=\"https://www.google.com\"></iframe>
"
--- response_body: <iframe src="https://www.google.com"></iframe>



=== TEST 2: html_sanitize_iframe_url_domain enable www.google.com
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http https;
    html_sanitize_iframe_url_domain  www.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1&iframe_url_domain=1
<iframe src=\"https://www.google.com\"></iframe>
"
--- response_body: <iframe src="https://www.google.com"></iframe>


=== TEST 3: html_sanitize_iframe_url_domain enable *.google.com
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http https;
    html_sanitize_iframe_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1&iframe_url_domain=1
<iframe src=\"https://www.google.com\"></iframe>
"
--- response_body: <iframe src="https://www.google.com"></iframe>



=== TEST 4: html_sanitize_iframe_url_domain enable *.google.com
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http https;
    html_sanitize_iframe_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1&iframe_url_domain=1
<iframe src=\"https://www.facebook.com\"></iframe>
"
--- response_body: <iframe></iframe>
