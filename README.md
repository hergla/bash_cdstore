# bash_cdstore
Store visited directories in sqlite3 db for reuse

# Build:

Building the binary and installing it, is a simple as that. Or just use the Makefile.

```
gcc -o cdstore cdstore.c -l sqlite3
chmod 755 cdstore
sudo cp cdstore /usr/local/bin
```

# Activate:

Add this to your .profile

```
PS1='$(/usr/local/bin/cdstore -a)\u@\H;|w $ '
export PS1

funnction c()
{
   dir=$(/usr/local/bin/cdstore 3>&1 1>&2 2>&3); cd $dir
}
```


