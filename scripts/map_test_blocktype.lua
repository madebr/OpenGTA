-- run it:
-- ./lua_map_test $map_file scripts/map_test_blocktype.lua
--
-- may soon also work from within luaviewer

if rawget(_G, 'map') == nil then 
  print("No map loaded!")
  os.exit()
end

function block_type_name(t)
  if t == 0 then return "air" end
  if t == 1 then return "water" end
  if t == 2 then return "road" end
  if t == 3 then return "pavement" end
  if t == 4 then return "field" end
  if t == 5 then return "building" end
  if t == 6 then return "unused1" end
  if t == 7 then return "unused2" end
  return "????????"
end

function find_all_of_type(t)
list = {}
for y = 0, 255 do
  for x = 0, 255 do
    c = map:getNumBlocksAt(x, y)
    for i = 0, c - 1 do
      info = map:getBlockAt(x, y, i)
      if info:getBlockType() == t then
        table.insert(list, { x, y, i })
      end
    end
  end
end
return list
end


function find_air_columns()
for y = 0, 255 do
  for x = 0, 255 do
    c = map:getNumBlocksAt(x, y)
    is_air = true
    for i = 0, c - 1 do
      info = map:getBlockAt(x, y, i)
      if info:getBlockType() ~= 0 then
        is_air = false
      end
    end
    if is_air == true then
      for i = 0, c - 1 do
        info = map:getBlockAt(x, y, i)
        lid = info:getTextureId(0)
        if lid > 0 and i < 4 then
          print("Air-column: " .. x .. ", " .. y)
          print("Texture " .. lid .. " set at " .. i)
        end
      end
    end
    
  end
end
end

function is_block_of_type(x, y, z, t)
  local c = map:getNumBlocksAt(x, y)
  assert(c > z)
  local info = map:getBlockAt(x, y, z)
  return info:getBlockType() == t
end

--find_air_columns()

function find_block_holes()
for y = 0, 255 do
  for x = 0, 255 do
    c = map:getNumBlocksAt(x, y)
    info = map:getBlockAt(x, y, c - 1)
    if info:getTextureId(0) > 0 and info:getBlockType() == 0 then
      print(x .. ", " .. y .. " has texture but no type above; level: " .. c - 1 ..
      " lid: " .. info:getTextureId(0))
    end
  end
end
end

find_block_holes()

--num = map:getNumBlocksAt(25, 17)
--for i = 0, num-1 do
--  info = map:getBlockAt(25, 17, i)
--  info:setBlockType(3)
--  print(i)
--  print("texture: " .. info:getTextureId(0))
--  print("type: " .. info:getBlockType())
--end
