from mars import dependency

fsCMake = {
    "name": "fsCMake",
    "ver": "v0.0.1",
    "addr": """\
https://github.com/francissuen/fsCMake/releases/download/{0}/fsCMake.tar.xz"""}

fsCMake["addr"] = fsCMake["addr"].format(fsCMake["ver"])

dependencies = [
    dependency.Dependency({
        "name": fsCMake["name"],
        "addr": fsCMake["addr"]})]

for d in dependencies:
    d.fix()
