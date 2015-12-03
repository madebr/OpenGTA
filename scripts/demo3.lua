pos = {
  x = 214,
  y = 13, -- this is Z (up) in GTA
  z = 51
}


screen.setFullscreen(true)
city_view:setTopDownView(false)
-- position the camera
camera.setEye(pos.x, pos.y, pos.z)
camera.setCenter(pos.x, 0, pos.z)
camera.setUp(0, 0, -1)
-- set pseudo-3d view
-- increase the visible range; default is 15
city_view:setVisibleRange(20)

-- If a global function 'game_tick' exists, it will be called
-- every 100 ms, but it is pretty pointless at this time.

tick_count = 0
next_move_tick = 30

function game_tick()
  if tick_count == next_move_tick then
    x, y, z = camera.getEye()
    camera.interpolateToPosition(x+math.random()*40-20, y, z + math.random()*40-20, 2000)
    next_move_tick = next_move_tick + 50
  end
  tick_count = tick_count + 1
end
