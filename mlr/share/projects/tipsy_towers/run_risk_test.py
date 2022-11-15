import click
import os

from mlr.share.projects.tipsy_towers.utils.msg_utils import Msg


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
@click.option('-p', '--g_file_path', default="", type=click.STRING, help='file path to a .g file')
def main(build_only, build_force, g_file_path):
    if build_only:
        os.system("make --always-make --silent -f makefile_run_risk_test")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_run_risk_test")

    file_path = None
    if len(g_file_path) > 1:
        file_path = g_file_path

    if file_path is None:
        Msg.print_error("ERROR [run_viewer]: must enter the type of file to view")
        assert False

    arguments_string = " ".join([file_path])
    os.system("./viewer.exe " + arguments_string)


if __name__ == '__main__':
    main()
