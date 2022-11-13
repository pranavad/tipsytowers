import click
import os

from mlr.share.projects.tipsy_towers.utils.constants import CoreUtils
from mlr.share.projects.tipsy_towers.utils.msg_utils import Msg


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
@click.option('-p', '--g_file_path', default="", type=click.STRING, help='file path to a .g file')
@click.option("-e", "--exp_type", default="", type=click.STRING, help="the experiment type")
@click.option("-t", "--exp_trial_num", default="", type=click.STRING, help="the name of the trial")
@click.option("-i", "--run_init", default=False, is_flag=True, help="init file to init file")
@click.option("-f", "--run_final", default=False, is_flag=True, help="final file to final file")
def main(build_only, build_force, g_file_path, exp_type, exp_trial_num, run_init, run_final):
    if build_only:
        os.system("make --always-make --silent -f makefile_run_viewer")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_run_viewer")

    file_path = None
    if len(g_file_path) > 1:
        file_path = g_file_path
    else:
        core_identifier = exp_type + CoreUtils.UNDERSCORE + exp_trial_num
        if run_init:
            file_path = os.path.join(CoreUtils.get_init_files_dir_path(), core_identifier + CoreUtils.DOT_G)
        elif run_final:
            file_path = os.path.join(CoreUtils.get_fin_files_dir_path(), core_identifier + CoreUtils.DOT_F_G)

    if file_path is None:
        Msg.print_error("ERROR [run_viewer]: must enter the type of file to view")
        assert False

    arguments_string = " ".join([file_path])
    os.system("./viewer.exe " + arguments_string)


if __name__ == '__main__':
    main()
