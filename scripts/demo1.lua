pos = {
  x = 214,
  y = 15, -- this is Z (up) in GTA
  z = 51
}

-- position the camera
city_view:setCamPosition(pos.x, pos.y, pos.z)
-- set pseudo-3d view
city_view:setTopDownView(true)
-- increase the visible range; default is 15
--city_view:setVisibleRange(20)
--screen.setFullscreen(true)
--spritecache.setScale2x(false)


tick = 0

-- If a global function 'game_tick' exists, it will be called
-- every 100 ms, but it is pretty pointless at this time.
function game_tick()
  if tick == 1 then
    screen.makeScreenShot("test.bmp")
  end
  tick = tick + 1
end
