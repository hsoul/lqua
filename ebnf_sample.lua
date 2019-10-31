require "functions"

function ds(x)
	return x
end

function test(a, b)
	local x = a * b
	ds(x)
	return a + b
end

local a = {[4] = {ds = ds}, j = 1, k = 2}

a.j, a.k = 7, 6

pvalue(a)


a[4].ds(3)

local a = function()
	-- body
end

