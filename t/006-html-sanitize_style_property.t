use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * blocks() * 2;

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: html_sanitize_style_property whitelist
--- config
location /t {
    html_sanitize on;
    html_sanitize_style_property color font-size;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=2
<h1 style=\"color:red\;text-align:center;font-size:12pt;\">h1</h1>
"
--- response_body: <h1 style="color:red;font-size:12pt;">h1</h1>



=== test 2: html_sanitize_style_property whitelist multi
--- config
location /t {
    html_sanitize on;
    html_sanitize_style_property color;
    html_sanitize_style_property font-size;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=2
<h1 style=\"color:red;text-align:center;font-size:12pt;\">h1</h1>
"
--- response_body: <h1 style="color:red;font-size:12pt;">h1</h1>



=== test 3: html_sanitize_style_property whitelist multi
--- config
location /t {
    html_sanitize on;
    html_sanitize_style_property color;
    html_sanitize_style_property font-size;
    html_sanitize_style_property background-image;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=2
<h1 style=\"color:red;text-align:center;font-size:12pt;background-image: expression(alert(1));\">h1</h1>
"
--- response_body: <h1 style="color:red;font-size:12pt;background-image:expression(alert(1));">h1</h1>



=== test 4: html_sanitize_style_property !important
--- config
location /t {
    html_sanitize on;
    html_sanitize_style_property color;
    html_sanitize_style_property font-size;
    html_sanitize_style_property background-image;
}
--- request eval
"POST /t?element=1&attribute=1&style_property=2
<h1 style=\"color:red;text-align:center;font-size:12pt !important;background-image: expression(alert(1));\">h1</h1>
"
--- response_body: <h1 style="color:red;font-size:12pt !important;background-image:expression(alert(1));">h1</h1>
