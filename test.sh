make clean
printf "tree\nmkdir b\ntouch b/a\nwrite b/a\nhello\nappend b/a\n jivko\ncat b/a\nrm -r b\ntree\nexit\n" | make ext2
