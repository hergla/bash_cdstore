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
if [[ -n "$PS1" ]]; then
PS1='$(/usr/local/bin/cdstore -a)\u@\H:\w $ '
export PS1

function c()
{
   dir=$(/usr/local/bin/cdstore 3>&1 1>&2 2>&3); cd $dir
}
fi

```

# Usage
Shows only four entries, but you will get the last tweenty visited directories.  

hergen@oel:/var/www/hergen $ c  
0 - /var/www/hergen  
1 - /usr/bin  
2 - /var/tmp  
3 - /home/hergen  
Enter row number: 2  
hergen@oel:/var/tmp $ c  
0 - /var/tmp  
1 - /var/www/hergen  
2 - /usr/bin  
3 - /home/hergen  
Enter row number: 2  
hergen@oel:/usr/bin $ c  
0 - /usr/bin  
1 - /var/tmp  
2 - /var/www/hergen  
3 - /home/hergen  
Enter row number:  
hergen@oel:/usr/bin $  
