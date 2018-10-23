
local function split(s, pattern)
    local t = {}
    s:gsub("[^" .. pattern .. "]*", function(x) table.insert(t, x) end)
    return t
end

function run(binary, num_processors, lower_bound, upper_bound, num_intervals)
    -- string.format("mpirun -np %d --hostfile ./hosts ./bin/%s %d %d",
    local cmd = string.format("mpirun -np %d ./bin/%s %d %d",
        num_processors, binary, lower_bound, upper_bound)
    if num_intervals then
        cmd = cmd .. " " .. num_intervals
    end

    local pipe = assert(io.popen(cmd, "r"))
    local output = split(pipe:read("*all"), ";")
    pipe:close()
    output = {result = tonumber(output[1]), delta = tonumber(output[2])}
    return output
end

local tests = {
    interval = {-5, 5},
    problem1 = {
        binary = "main1",
        num_processors = {1, 2, 4, 8}
    },
    problem2 = {
        version1 = {
            binary = "main21",
            num_processors = {2, 4, 8, 16},
            num_intervals = {32, 128, 512, 1024}
        },
        version2 = {
            binary = "main22",
            num_processors = {2, 4, 8, 16}
        }
    }
}

local t1 = tests.problem1
local t2 = tests.problem2.version1
local t3 = tests.problem2.version2
local lower_bound = tests.interval[1]
local upper_bound = tests.interval[2]

--------------------------------------------------
--
-- Running
--
--------------------------------------------------

os.execute("make all")
if arg[1] == "copy" then
    os.execute("mrcp all bin/main1 /bin/main1")
    os.execute("mrcp all bin/main21 /bin/main21")
    os.execute("mrcp all bin/main22 /bin/main22")
end

local results = {
    {"", "1", "21-32", "21-128", "21-512", "21-1024", "22"},
    {1,  "-", "-", "-", "-", "-", "-"},
    {2,  "-", "-", "-", "-", "-", "-"},
    {4,  "-", "-", "-", "-", "-", "-"},
    {8,  "-", "-", "-", "-", "-", "-"},
    {16, "-", "-", "-", "-", "-", "-"},
}

for i, procs in ipairs(t1.num_processors) do
    local output = run(t1.binary, procs, lower_bound, upper_bound)
    results[i + 1][2] = output
end

for i, procs in ipairs(t2.num_processors) do
    for j, itvs in ipairs(t2.num_intervals) do
        local output = run(t2.binary, procs, lower_bound, upper_bound, itvs)
        results[i + 2][j + 2] = output
    end
end

for i, procs in ipairs(t3.num_processors) do
    local output = run(t3.binary, procs, lower_bound, upper_bound)
    results[i + 2][7] = output
end

--------------------------------------------------
--
-- Printing
--
--------------------------------------------------

function stringfy_output(output)
    if type(output) == "table" then
        return string.format("%.3f", output.delta or "0.0")
    else
        return output
    end
end

for i = 1, 6 do
    for j = 1, 7 do
        io.write(stringfy_output(results[i][j]) .. "\t")
    end
    io.write("\n")
end
