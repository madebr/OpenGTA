local n
--definition_indices = {}
--for n in pairs(definitions) do table.insert(definition_indices, n) end
--table.sort(definition_indices)

command_indices = {}
for n in pairs(commands) do table.insert(command_indices, n) end
table.sort(command_indices)

function idx_by_gtaidx(gta_idx)
  local i
  for i = 1, #command_indices do
    if command_indices[i] == gta_idx then return i end
  end
  --error("Invalid gta-cmd-idx: " .. gta_idx)
  return -1
end

-- from: http://lua-users.org/wiki/StringRecipes
-- Compatibility: Lua-5.1
function split(str, pat)
   local t = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = string.find(str, fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = string.find(str, fpat, last_end)
   end
   if last_end <= string.len(str) then
      cap = string.sub(str, last_end)
      table.insert(t, cap)
   end
   return t
end

function run_command(idx, cmd, params)
  local res = 0
  if cmd == "KICKSTART" then
  --  new_thread(idx, params[1])
    print("\tKICKSTART in " .. idx .. " from " .. params[1] .. " = " ..idx_by_gtaidx(params[1] + 0))
    new_thread(idx, idx_by_gtaidx(params[1] + 0))
  else
    print("\t" .. cmd .. " in line " .. idx)
  end
  return res 
end

function iterate_commands(idx)
  local is_sleeping = false
  while idx <= #command_indices do
    local gta_idx = command_indices[idx]
    if is_sleeping == true then 
      while true do
          print("  thread is sleeping in " .. gta_idx)
          coroutine.yield()
      end
    end
    --print("LINE " .. gta_idx)
    local cmd = commands[gta_idx]
    local cmd_split = split(cmd, " ")
    local cmd_name = cmd_split[1]
    --print(cmd)
    table.remove(cmd_split, 1)
    local ok_idx = cmd_split[2]
    local fail_idx = cmd_split[3]
    if idx == 1 then
      idx = idx + 1
    elseif idx > 1 then
      local next_idx = 0
      --print("  "..gta_idx .. " " .. cmd_name)
      if run_command(gta_idx, cmd_name, cmd_split) == 1 then
        next_idx = fail_idx + 0
        print('   failed '..fail_idx)
      else
        next_idx = ok_idx + 0
        print('   ok '..ok_idx)
      end
      if next_idx == 0 then
        idx = idx + 1
      elseif next_idx == -1 then
        if cmd_name == "CRANE" then idx = idx + 1
        else
          is_sleeping = true
          --error("EXIT: " .. next_idx)
        end
      else
        idx = idx_by_gtaidx(next_idx)
      end
    end
    coroutine.yield()
          print('  RESUMING2')
  end
  coroutine.yield()
          print('  RESUMING3')
end

threads = {}
thread_indices = {}
function new_thread(t_id, start_at_idx)
  table.insert(threads, coroutine.create(function () iterate_commands(start_at_idx) end))
  print("  "..#threads ..  " = " ..t_id)
  table.insert(thread_indices, t_id)
end


new_thread(0, 1)
while #threads > 0 do
  for i, j in pairs(threads) do
    print("thread " .. thread_indices[i] .. " [" .. #threads .. "]")
    if coroutine.status(j) == "suspended" then
      coroutine.resume(j)
    elseif coroutine.status(j) == "dead" then
      table.remove(threads, i)
      table.remove(thread_indices, i)
      print("removing dead " .. i)
    end
  end
end
