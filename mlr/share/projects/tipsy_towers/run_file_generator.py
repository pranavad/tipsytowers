import click
import os


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
def main(build_only, build_force):
    if build_only:
        os.system("make --always-make --silent -f makefile_file_generator")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_file_generator")


if __name__ == '__main__':
    main()
