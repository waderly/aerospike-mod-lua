--[[
function append(r,bin,...)
    local l = r[bin] or list()

    local len = select('#',...)
    for i=1, len do
        list.append(l, select(i,...))
    end

    r[bin] = l

    if aerospike:exists(r) then
        aerospike:update(r)
    else
        aerospike:create(r)
    end

    return r[bin]
end

function prepend(r,bin,...)
    local l = r[bin] or list()

    local len = select('#',...)
    for i=1, len do
        list.prepend(l, select(i,...))
    end

    r[bin] = l
    
    if aerospike:exists(r) then
        aerospike:update(r)
    else
        aerospike:create(r)
    end

    return r[bin]
end

function drop(r,bin,n)
    local l = r[bin] or list()
    return list.drop(l, n)
end

function take(r,bin,n)
    local l = r[bin] or list()
    return list.take(l, n)
end

function iterate(r,k,...)

    local l = list()

    local len = select('#',...)
    for i=1, len do
        list.append(l, select(i,...))
    end

    local j = list.iterator(l);
    while j:has_next() do
        info(j:next())
    end

    return 1
end

function lappend(r,l,...)
    local len = select('#',...)
    for i=1, len do
        list.append(l, select(i,...))
    end
    return l
end

function bappend(r,b,...)
    local l = r[b] or list()
    local len = select('#',...)
    for i=1, len do
        list.append(l, select(i,...))
    end
    if aerospike:exists(r) then
        aerospike:create(r)
    else
        aerospike:update(r)
    end
    return r[b]
end

function bprint(r,b)
    local l = r[b]
    if l == nil then
        info("nil")
    else
        info("size %d", list.size(l))
    end
    return l;
end
]]
function set(r,b,l)
    r[b] = l
    if aerospike:exists(r) then
        aerospike:create(r)
    else
        aerospike:update(r)
    end

    if aerospike:exists(r) then
        info("record exists!")
    else
        info("record doesn't exist!")
    end
    return 0
end

function get(r,b)
    local l = r[b]
    if l == nil then
        return 1
    else
        return 0
    end
end

function newbytes(r,a,b,c)
    local b = bytes{1,2,3}
    info("1 => %s",b[1] or "<nil>")
    info("2 => %s",b[2] or "<nil>")
    info("3 => %s",b[3] or "<nil>")
    return l[2]
end
