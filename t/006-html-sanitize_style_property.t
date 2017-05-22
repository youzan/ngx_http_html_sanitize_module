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



=== test 5: html_sanitize_style_property "'" when style_property_value is 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.style;
    html_sanitize_style_property font-family;
}
--- request eval
"POST /t?element=1&attribute=2&style_property=1&style_property_value=0
<h1 style=\"font-family: 'Microsoft Yahei';\">h1</h1>
"
--- response_body: <h1 style="font-family: 'Microsoft Yahei';">h1</h1>



=== test 6: html_sanitize_style_property """ when style_property_value is 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.style;
    html_sanitize_style_property font-family;
}
--- request eval
"POST /t?element=1&attribute=2&style_property=1&style_property_value=0
<h1 style='font-family: \"Microsoft Yahei\";\'>h1</h1>
"
--- response_body: <h1 style='font-family: "Microsoft Yahei";'>h1</h1>



=== test 7: html_sanitize_style_property &quot; when style_property_value is 0
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.style;
    html_sanitize_style_property font-family;
}
--- request eval
"POST /t?element=1&attribute=2&style_property=1&style_property_value=0
<h1 style=\"font-family: &quot;Microsoft Yahei&quot;;\">h1</h1>
"
--- response_body: <h1 style="font-family: 'Microsoft Yahei';">h1</h1>



=== test 8: html_sanitize_style_property "'" when style_property_value is 1
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.style;
    html_sanitize_style_property font-family;
}
--- request eval
"POST /t?element=1&attribute=2&style_property=1&style_property_value=1
<h1 style=\"font-family: 'Microsoft Yahei';\">h1</h1>
"
--- response_body: <h1 style="font-family:'Microsoft Yahei';">h1</h1>



=== test 9: html_sanitize_style_property &quot; when style_property_value is 1
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.style;
    html_sanitize_style_property font-family;
}
--- request eval
"POST /t?element=1&attribute=2&style_property=1&style_property_value=1
<h1 style=\"font-family: &quot;Microsoft Yahei&quot;;\">h1</h1>
"
--- response_body: <h1 style="font-family:'Microsoft Yahei';">h1</h1>



=== test 10: html_sanitize_style_property """ when style_property_value is 1
--- config
location /t {
    html_sanitize on;
    html_sanitize_attribute h1.style;
    html_sanitize_style_property font-family;
}
--- request eval
"POST /t?element=1&attribute=2&style_property=1&style_property_value=1
<h1 style='font-family: \"Microsoft Yahei\";\'>h1</h1>
"
--- response_body: <h1 style='font-family:"Microsoft Yahei";'>h1</h1>
