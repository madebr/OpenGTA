function pairsByKeys (t, f)
  local a = {}
  for n in pairs(t) do table.insert(a, n) end
    table.sort(a, f)
    local i = 0      -- iterator variable
    local iter = function ()   -- iterator function
    i = i + 1
    if a[i] == nil then return nil
    else return a[i], t[a[i]]
    end
  end
  return iter
end

function config_as_string()
  local conf_str = ""
  if type(config) ~= 'table' then return conf_str end
  table.sort(config)
  for i, j in pairsByKeys(config) do
    if type(j) == 'boolean' then
      if j == true then
        conf_str = conf_str .. i .. ' = true\n'
      else
        conf_str = conf_str .. i .. ' = false\n'
      end
    elseif type(j) == 'string' then
      conf_str = conf_str .. i .. ' = "' .. j .. '"\n'
    else
      conf_str = conf_str .. i .. ' = ' .. j .. '\n'
    end
  end
  return conf_str
end


print("-- CONFIG DUMP --")
print(config_as_string())
print("-- END OF CONFIG --")
quit()

