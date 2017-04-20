use Test::Nginx::Socket; # 'no_plan';

repeat_each(2);

plan tests => repeat_each() * blocks();

#no_long_string();
#no_diff;

run_tests();

__DATA__

=== TEST 1: enable html sanitize config
--- config
location /t {
    html_sanitize on;
}
--- request
POST /t
--- error_code: 204



=== TEST 2: disable html sanitize config
--- config
location /t {
    html_sanitize off;
}
--- request
POST /t
--- error_code: 404
