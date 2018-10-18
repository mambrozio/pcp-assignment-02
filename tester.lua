
local function split(s, pattern)
    local t = {}
    s:gsub("[^" .. pattern .. "]*", function(x) table.insert(t, x) end)
    return t
end

function run(binary, num_processors, lower_bound, upper_bound, num_intervals)
    local cmd = string.format("mpirun -np %d --hostfile ./hosts ./bin/%s %d %d",
        num_processors, binary, lower_bound, upper_bound)
    if num_intervals then
        cmd = cmd .. " " .. num_intervals
    end

    local pipe = assert(io.popen(cmd, "r"))
    local output = split(pipe:read("*all"), ";")
    pipe:close()
    output = {result = output[1], delta = output[2]}
    return output
end

local tests = {
    lower_bound = -5,
    upper_bound = 5,
    problem1 = {
        binary = "main1",
        num_processors = {1, 2, 4, 8},
        outputs = {}
    },
    problem2 = {
        version1 = {
            binary = "main21",
            num_processors = {2, 4, 8, 16},
            num_intervals = {32, 128, 512, 1024},
            outputs = {}
        },
        version2 = {
            binary = "main22",
            num_processors = {2, 4, 8, 16},
            outputs = {}
        }
    }
}

local t1 = tests.problem1
local t2 = tests.problem2.version1
local t3 = tests.problem2.version2
local lower_bound = tests.lower_bound
local upper_bound = tests.upper_bound

--------------------------------------------------
--
-- Running
--
--------------------------------------------------

os.execute("make all")

for _, procs in ipairs(t1.num_processors) do
    local output = run(t1.binary, procs, lower_bound, upper_bound)
    table.insert(t1.outputs, output)
end

for i, procs in ipairs(t2.num_processors) do
    table.insert(t2.outputs, {})
    for j, itvs in ipairs(t2.num_intervals) do
        local output = run(t2.binary, procs, lower_bound, upper_bound, itvs)
        table.insert(t2.outputs[i], output)
    end
end

for _, procs in ipairs(t3.num_processors) do
    local output = run(t3.binary, procs, lower_bound, upper_bound)
    table.insert(t3.outputs, output)
end

--------------------------------------------------
--
-- Printing
--
--------------------------------------------------

function printoutput(output)
    if output.result and output.delta then
        print("Result:\t" .. output.result)
        print("Time:\t" .. output.delta)
    else
        print("No output")
    end
end

print("-------------------- Problem 01")
for i, output in ipairs(t1.outputs) do
    print("----- " .. t1.num_processors[i] .. " processors")
    printoutput(output)
end

print("-------------------- Problem 02 [version 1]")
for i in ipairs(t2.outputs) do
    for j in ipairs(t2.outputs[i]) do
        local output = t2.outputs[i][j]
        print("----- " ..
            t2.num_processors[i] .. " processors - " ..
            t2.num_intervals[j] .. " intervals"
        )
        printoutput(output)
    end
end

print("-------------------- Problem 02 [version 2]")
for i, output in ipairs(t3.outputs) do
    print("----- " .. t3.num_processors[i] .. " processors")
    printoutput(output)
end
