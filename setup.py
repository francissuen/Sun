try:
    from mars import dependency
except ImportError:
    import subprocess
    import sys
    subprocess.run([sys.executable, "-m", "pip", "install",
                    "git+https://github.com/francissuen/mars.git@master"])
    from mars import dependency

fsCMake = {
    "name": "fsCMake",
    "ver": "v0.0.0",
    "addr": """\
https://github.com/francissuen/fsCMake/releases/download/{0}/fsCMake.tar.xz"""}

fsCMake["addr"] = fsCMake["addr"].format(fsCMake["ver"])

deps = dependency.Dependency()

deps.add(dependency.DepInfo(fsCMake))

deps.fix()
