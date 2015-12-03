
--screen.setFullscreen(true)
camera.setCenter(61.4261, 7.56265, 2.2242)
camera.setEye(61.4185, 8.72266, 0.810034)
camera.setUp(0, 1, 0)
city_view:setVisibleRange(30)
city_view:setTopDownView( false )

ticks = 0
m_step = 1

function step()
  if m_step == 1 then
camera.setCenter(61.8193, 5.56514, 14.7887)
camera.setEye(61.6693, 6.00516, 16.1949)
  elseif m_step == 2 then
camera.setCenter(17.8087, 16.7441, 17.0151)
camera.setEye(16.7382, 17.3441, 16.0909)
  elseif m_step == 3 then
camera.setCenter(22.4076, 14.1664, 20.9857)
camera.setEye(21.3371, 14.7664, 20.0615)
  elseif m_step == 4 then
camera.setCenter(15.9171, 7.45477, 197.031)
camera.setEye(17.2552, 8.13476, 196.573)
  elseif m_step == 5 then
camera.setCenter(4.66008, 5.85241, 198.842)
camera.setEye(3.24865, 6.53241, 198.754)
  elseif m_step == 6 then
camera.setCenter(4.08809, 7.71514, 200.964)
camera.setEye(4.36918, 10.0751, 202.35)
  elseif m_step == 7 then
camera.setCenter(91.2481, 6.10855, 230.936)
camera.setEye(91.2458, 6.86855, 229.522)
  elseif m_step == 8 then
camera.setCenter(225.143, 6.53355, 173.986)
camera.setEye(226.363, 7.29354, 174.702)
  elseif m_step == 9 then
camera.setCenter(249.057, 7.64056, 143.877)
camera.setEye(248.829, 8.40055, 142.481)
  elseif m_step == 10 then
camera.setCenter(234.41, 5.14698, 115.074)
camera.setEye(235.747, 5.50698, 115.533)
  elseif m_step == 11 then
camera.setCenter(149.634, 8.1291, 14.9788)
camera.setEye(151.044, 8.6491, 14.8622)
  end
  m_step = m_step + 1
end

--city_view:setCamPosition(143, 9, 23)
--city_view:setVisibleRange(15)
--city_view:setTopDownView( true )

function game_tick()
  ticks = ticks + 1
  if ticks > 50 then
    step()
    ticks = 0
  end
end
