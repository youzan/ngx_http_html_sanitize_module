use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * blocks() * 2;

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: do node decode html entities
--- config
location /t {
    html_sanitize on;
    html_sanitize_iframe_url_protocol https;
}
--- request eval
"POST /t?element=1&attribute=1
xxx&quot;&lt;svg/onload=alert(1) onfocus=alert(2)&gt;
"
--- response_body: xxx&quot;&lt;svg/onload=alert(1) onfocus=alert(2)&gt;
