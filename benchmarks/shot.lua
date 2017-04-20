local open = io.open

local function read_file(path)
    local file = open(path, "rb") -- r read mode and b binary mode
    if not file then return nil end
    local content = file:read "*a" -- *a or *all reads the whole file
    file:close()
    return content
end

html = read_file('./google.html')

wrk.method = "POST"
wrk.body  = html
wrk.headers["Content-Type"] = "text/html"
wrk.path = '/?element=1&attribute=1'
