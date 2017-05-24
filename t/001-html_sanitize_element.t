use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);
plan tests => repeat_each() * (blocks() + 14);

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: html_sanitize_element h1
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
}
--- request eval
"POST /t?element=2
<h1>h1<h2>h2</h2></h1>
"
--- response_body: <h1>h1</h1>


=== TEST 2: html_sanitize_element multi head
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element h2;
}
--- request eval
"POST /t?element=2
<h1>h1<h2>h2</h2></h1>
"
--- response_body: <h1>h1</h1><h2>h2</h2>



=== TEST 3: html_sanitize_element h1 p a
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element p;
    html_sanitize_element a;
}
--- request eval
"POST /t?element=2
<h1>h1</h1>
<p>p<a>a<span>span</span></a></p>
"
--- response_body: <h1>h1</h1><p>p<a>a</a></p>



=== TEST 4: html_sanitize_element empty tr
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element table tbody tr th td;
}
--- request eval
"POST /t?element=2
<table>
  <tr>
    <th>Month</th>
    <th>Savings</th>
  </tr>
  <tr>
  </tr>
</table>
"
--- response_body: <table><tbody><tr><th>Month</th><th>Savings</th></tr><tr></tr></tbody></table>



=== TEST 5: Explicitly disable <script> and <style>
--- config
location /t {
    html_sanitize on;
    html_sanitize_element html head body;
    html_sanitize_element h1;
    html_sanitize_element p;
    html_sanitize_element a;
}
--- request eval
"POST /t?element=1
<abcd>1234<script>script</script><style>style</style></abcd>
"
--- response_body: <abcd>1234</abcd>



=== TEST 6: unknown tag should be right in utf8
--- config
location /t {
    html_sanitize on;
}
--- request eval
["POST /t?element=1
<abcd>你好</abcd><defg>世界</defg>
",
"POST /t?element=1
<abcd>你好</abcd><defg>世界</defg>
",
"POST /t?element=1
<abcd>你好</abcd><defg>世界</defg>",
"POST /t?element=1
<abcd>你好</abcd><defg>世界</defg>",
"POST /t?element=1
<abcd>你好</abcd><defg>世界</defg>",
]
--- response_body eval
["<abcd>你好</abcd><defg>世界</defg>",
"<abcd>你好</abcd><defg>世界</defg>",
"<abcd>你好</abcd><defg>世界</defg>",
"<abcd>你好</abcd><defg>世界</defg>",
"<abcd>你好</abcd><defg>世界</defg>",
]
