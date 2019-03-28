import urllib.request
import sys
import curses


def download_file(url):
    # rm trailing '\n' or ' '
    end_url_idx = 0
    for i in range(len(url)-1, 0, -1):
        if url[i] != "\n" or url[i] != " ":
            end_url_idx = i
            break
    if end_url_idx == 0:
        return
    url = url[0:end_url_idx]
    # open url
    with urllib.request.urlopen(url) as response:
        # info of file
        content_len = int(response.getheader("Content-Length"))
        file_name = url.split('/')[-1]
        sys.stdout.write("file @name: {0}, @size: {1}MB\n".format(
            file_name, (content_len/1024)/1024))
        # data = bytearray(1024)
        # download procedure
        win = curses.initscr()
        with open(file_name, "wb") as f:
            for i in range(0, content_len, 1024):
                # data[0:1024] = response.read(1024)
                f.write(response.read(1024))
                # sys.stdout.write("@size: {0}MB\n".format(((i+1024)/1024)/1024))
                try:
                    win.addch("1")
                finally:
                    curses.endwin()
                win.refresh()
                # sys.stdout.flush()
            f.flush()
        curses.endwin()


# http://mirrors.us.kernel.org/ubuntu-releases/18.04.2/ubuntu-18.04.2-live-server-amd64.iso
url = """\
http://www.baidu.com/img/baidu_85beaf5496f291521eb75ba38eacbd87.svg
"""
try:
    download_file(url)
finally:
    curses.endwin
