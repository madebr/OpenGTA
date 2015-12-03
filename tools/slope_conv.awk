BEGIN {
  slope_idx = -1;
  mode_seek_slope_hdr = 0;
  mode_seek_type = 1;
  mode_seek_modifier = 2;
  mode_seek_vertices = 3;
  num_vertices_read = 0;
  first_pass_flag = 0;
  reverse_flag = 0;
  till_block_complete = -1;
  mode = mode_seek_slope_hdr;
}

{
  if (mode == mode_seek_slope_hdr) {
    if ($1 == "Slope:") {
      #print "** slope start " $2;
      print "{ // slope: " $2;
      mode = mode_seek_type;
      slope_idx = $2;
      till_block_complete = 5;
      next;
    }
  }
  if (mode == mode_seek_type) {
    if ($1 == "LID" || $1 == "NORTH" || $1 == "SOUTH" || $1 == "WEST" || $1 == "EAST") {
      #print "* type " $1;
      last_type = $1;
      mode = mode_seek_modifier;
      first_pass_flag = 0;
      reverse_flag = 0;
      num_vertices_read = 0;
      till_block_complete = till_block_complete - 1;
      next;
    }
  }
  if (mode == mode_seek_modifier) {
    first_pass_flag = $1;
    reverse_flag = $2;
    mode = mode_seek_vertices;
    #print "* modifier: " first_pass_flag " " reverse_flag;
    next;
  }
  if (mode == mode_seek_vertices) {
    gsub(",", ".", $0);
    gsub(". ", ", ", $0);
    #print "v (" num_vertices_read "): " $0;

    # FIX: switch y<->z order
    gsub(",", "", $2);
    # offset y coord
    $2 = $2 + 1
    vertices[num_vertices_read] = $1 " " $3 ", " $2;
    #vertices[num_vertices_read] = $1 " " $2 ".00, " $3; # cheat .00 back
    
    num_vertices_read = num_vertices_read + 1;
    if (num_vertices_read == 4) {
      mode = mode_seek_type;
      #print "* end slope";
      print "  { // " last_type;
      if (reverse_flag == 0) {
        for (v=3; v > 0; v--) {
          print "   { " vertices[v] " },";
        }
        print "   { " vertices[0] " }";
        if (last_type == "EAST") {
          print "  }";
        }
        else {
          print "  },";
        }
      }
      else {
        for (v=3; v >= 0; v--) {
          print vertices[v];
        }
      }
      if (till_block_complete == 0) {
      #  print "** end block"
        if (slope_idx == 44) {
          print "}";
        }
        else {
          print "},";
        }
        mode = mode_seek_slope_hdr;
      }
    }
    next;
  }
}
