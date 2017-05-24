use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * (blocks() * 2 - 2);

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: document default 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<h1>h1</h1>
"
--- response_body: <h1>h1</h1>



=== TEST 2: document set 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
}
--- request eval
"POST /t?document=1&element=1&attribute=1&url_protocol=1&url_domain=1
<h1>h1</h1>
"
--- response_body eval
"<!DOCTYPE HTML >
<h1>h1</h1>"



=== TEST 3: context default 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<h1>h1</h1>
"
--- response_body: <h1>h1</h1>



=== TEST 4: context set GUMBO_TAG_HTML(0)
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1&context=0
<h1>h1</h1>
"
--- response_body: <head></head><body><h1>h1</h1></body>



=== TEST 5: html default 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<h1>h1</h1>
"
--- response_body: <h1>h1</h1>



=== TEST 6: html set 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1&html=1
<h1>h1</h1>
"
--- response_body: <html><h1>h1</h1></html>



=== TEST 7: element default 0
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?attribute=1&url_protocol=1&url_domain=1
<h1>h1</h1>
"
--- error_code: 204



=== TEST 7: element set 1
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<h1>h1</h1>
"
--- response_body: <h1>h1</h1>



=== TEST 9: element set 2
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=2
<h1>h1</h1>
"
--- error_code: 204



=== TEST 10: attribute default 0
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1
<h1 g=1>h1</h1>
"
--- response_body: <h1>h1</h1>



=== TEST 11: attribute set 1
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1
<h1 g=1>h1</h1>
"
--- response_body: <h1 g=1>h1</h1>



=== TEST 12: attribute set 2
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.class;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 class=\"ha\" g=1>h1</h1>
"
--- response_body: <h1 class="ha">h1</h1>



=== TEST 13: style_property default 0
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1
<h1 q=1 style=\"color:red;\">h1</h1>
"
--- response_body: <h1 q=1>h1</h1>



=== TEST 14: style_property set 1
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=1
<h1 q=1 style=\"color:red;\">h1</h1>
"
--- response_body: <h1 q=1 style="color:red;">h1</h1>



=== TEST 15: style_property_value default 0
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=1
<h1 q=1 style=\"color:expression(alert(xss));\">h1</h1>
"
--- response_body: <h1 q=1 style="color:expression(alert(xss));">h1</h1>



=== TEST 15: style_property_value set 1
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=1&style_property_value=1
<h1 q=1 style=\"color:expression(alert(xss));font-size: 12pt;\">h1</h1>
"
--- response_body: <h1 q=1 style="font-size:12pt;">h1</h1>



=== TEST 16: url_protocol default 0
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1
<a href=\"https://www.google.com\">123</a>
"
--- response_body: <a href="https://www.google.com">123</a>



=== TEST 17: url_protocol set 1
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol http;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1
<a href=\"https://www.facebook.com\">123</a>
"
--- response_body: <a>123</a>



=== TEST 18: url_domain default 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol http https;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1
<a href=\"https://www.google.com\">123</a>
"
--- response_body: <a href="https://www.google.com">123</a>



=== TEST 18: url_domain set 1
--- config
location /t {
    html_sanitize on;
    html_sanitize_url_protocol http https;
    html_sanitize_url_domain *.google.com;
}
--- request eval
"POST /t?element=1&attribute=1&url_protocol=1&url_domain=1
<a href=\"https://www.facebook.com\">123</a>
"
--- response_body: <a>123</a>



=== TEST 19: iframe_url_protocol default 0
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=1
<iframe src=\"https://google.com\"></iframe>
"
--- response_body: <iframe src="https://google.com"></iframe>



=== TEST 20: iframe_url_protocol set 1
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1
<iframe src=\"http://www.google.com\"></iframe>
"
--- response_body: <iframe src="http://www.google.com"></iframe>



=== TEST 21: iframe_url_domain default 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol http https;
}
--- request eval
"POST /t?element=1&attribute=1&iframe_url_protocol=1
<iframe src=\"https://www.google.com\"></iframe>
"
--- response_body: <iframe src="https://www.google.com"></iframe>



=== TEST 22: iframe_url_domain set 1
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



=== TEST 23: Explicitly enable <script>
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element p;
    html_sanitize_element a;
}
--- request eval
"POST /t?element=1&script=1
<abcd>1234<script>script</script><style>style</style></abcd>
"
--- response_body: <abcd>1234<script>script</script></abcd>



=== TEST 24: Explicitly enable <style>
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element p;
    html_sanitize_element a;
}
--- request eval
"POST /t?element=1&style=1
<abcd>1234<script>script</script><style>style</style></abcd>
"
--- response_body: <abcd>1234<style>style</style></abcd>



=== TEST 25: Explicitly enable <script> and <style>
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element p;
    html_sanitize_element a;
}
--- request eval
"POST /t?element=1&style=1&script=1
<abcd>1234<script>script</script><style>style</style></abcd>
"
--- response_body: <abcd>1234<script>script</script><style>style</style></abcd>
