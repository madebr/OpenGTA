BEGIN {
  in_record = 1
  bug_num = 0
}
{
  if ($1 ~ /^\%\% *$/) {
#    print "RECORD END"
    if (length(bug_status) == 0) {
      bug_status = "open"
    }
    if (length(bug_has_image) == 0) {
      pr_image_str = ""
    }
    else {
      pr_image_str = "image: <a href=\"" bug_has_image "\">" bug_has_image "</a>"
    }
    print "<div class=\"a_bug\"><a name=\"bug_" bug_num "\"></a>"
    print " <span class=\"bug_n\">#" bug_num "</span> &nbsp; <span class=\"bug_t\">" bug_title "</span><br>"
    print " <span class=\"bug_s\">status: " bug_status "</span> &nbsp; <span>" pr_image_str "</span><br>"
    print " <span class=\"bug_d\">" bug_descr "</span>"
    print "</div>"
    print "<br>"
  }
  if ($1 == "Bug:") {
    sub(/^Bug: /, "", $0)
    bug_title = $0
    in_record = 2
    bug_has_image = ""
    bug_descr = ""
    bug_status = ""
    bug_num = bug_num + 1
  }
  else if ($1 == "Image:") {
    bug_has_image = $2
  }
  else if ($1 == "Status:") {
    bug_status = $2
  }
  else {
    if (in_record == 2) {
      if (length(bug_descr) > 0) {
        if (length($0) == 0) {
          bug_descr = bug_descr "<br>"
        }
        else {
          bug_descr = bug_descr " " $0
        }
      }
      else {
        bug_descr = $0
      }
    }
  }
}
