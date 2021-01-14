from mars import dependency


deps = dependency.Dependency()

vesta = {
    "src_path": """\
https://github.com/francisjsun/vesta.git@dev"""}
deps.add(dependency.DepInfo(vesta), dependency.fs_trivial_git_proj_dep_sln)


def main():
    deps.fix()


if __name__ == "__main__":
    main()
