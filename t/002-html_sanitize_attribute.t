use lib 'lib';
use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * blocks() * 2;

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: html_sanitize_attribute
--- config
location /t {
    html_sanitize on;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 class=\"uhh\">h1</h1>
"
--- response_body: <h1>h1</h1>



=== TEST 2: html_sanitize_attribute h1.class
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.class;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 class=\"uhh\">h1</h1>
"
--- response_body: <h1 class="uhh">h1</h1>



=== TEST 3: html_sanitize_attribute ignore a.href
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1 a;
}
--- request eval
"POST /t?element=1&attribute=2
<a href=\"http://www.google.com\">a</a>
"
--- response_body: <a>a</a>



=== TEST 4: html_sanitize_attribute add a.href
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1 a;
    html_sanitize_attribute h1.g;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 g=1>h1</h1>
"
--- response_body: <h1 g=1>h1</h1>



=== TEST 5: html_sanitize_attribute multi
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.g;
    html_sanitize_attribute h2.g;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 g=1>h1</h1><h2 g=1>h2</h2>
"
--- response_body: <h1 g=1>h1</h1><h2 g=1>h2</h2>



=== TEST 5: html_sanitize_attribute match parent only
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.g;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 g=1>h1</h1><h2 g=1>h2</h2>
"
--- response_body: <h1 g=1>h1</h1><h2>h2</h2>



=== TEST 6: html_sanitize_attribute match prefix wildcard
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute *.g;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 g=1>h1</h1><h2 g=1>h2</h2>
"
--- response_body: <h1 g=1>h1</h1><h2 g=1>h2</h2>



=== TEST 7: html_sanitize_attribute match suffix wildcard
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.*;
}
--- request eval
"POST /t?element=1&attribute=2
<h1 g=1 j=1>h1</h1><h2 g=1>h2</h2>
"
--- response_body: <h1 g=1 j=1>h1</h1><h2>h2</h2>
