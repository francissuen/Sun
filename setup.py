import urllib.request
import sys
import numbers
import datetime


def fancy_bytes_format(bytes):
    if not isinstance(bytes, numbers.Number):
        return
    KB = 1024
    MB = 1024 * KB
    G = 1024 * MB
    unit, scale = "B", bytes
    if bytes > G:
        unit = "G"
        scale = bytes / G
    elif bytes > MB:
        unit = "MB"
        scale = bytes / MB
    elif bytes > KB:
        unit = "KB"
        scale = bytes / KB
    return scale, unit, fixedWidth


def rm_url_trailing_trash_characters(raw_url):
    trash_characters = "\n "
    endOfUrlIdx = -1
    for i in range(-1, -len(raw_url), -1):
        if trash_characters.count(raw_url[i]) == 0:
            endOfUrlIdx = i
            break
    return raw_url[0:endOfUrlIdx]


class logger:
    def __init__(self):
        self.dateMsg = datetime.time.asctime()

        def log(self, msg):
            sys.stdout.write(self.dateMsg + "\n" + msg)


def download_file(url):
    url = rm_url_trailing_trash_characters(url)
    if len(url) == 0:
        return
    file_name = url.split('/')[-1]
    if len(file_name) == 0:
        return
    # open url
    with urllib.request.urlopen(url) as response:
        # info of file
        content_len = int(response.getheader("Content-Length"))
        log = logger()
        scale, unit = fancy_bytes(content_len)
        log.log("file @name: {0}, @size: {1} {2}".format(
            file_name, scale, unit))
        # download procedure
        sizeOfWritten = 0
        with open(file_name, "wb") as f:
            for i in range(0, content_len, 1024):
                # data[0:1024] = response.read(1024)
                data = response.read(1024)
                sizeOfWritten += len(data)
                f.write(data)
                sys.stdout.write("@size: {0}MB\r"
                                 .format(((sizeOfWritten)/1024)/1024))
                sys.stdout.flush()

            f.flush()
            sys.stdout.write("written to file @name: {0}\n".format(file_name))
            sys.stdout.flush()


# http://mirrors.us.kernel.org/ubuntu-releases/18.04.2/ubuntu-18.04.2-live-server-amd64.iso
url = """\
https://github.com/francissuen/fsCMake/releases/download/v1.0.3/fsCMake.tar.xz
"""
download_file(url)
