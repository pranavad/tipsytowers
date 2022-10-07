import click
import os

from mlr.share.projects.integrated_model.utils.constants import CoreUtils


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
@click.option('-e', '--exp_type', type=click.STRING, help='experiment type information')
@click.option('-t', '--exp_trial_num', type=click.STRING, help='experiment trial number information')
@click.option("-i", "--init_only", default=False, is_flag=True, help="only make the init g file")
def main(build_only, build_force, exp_type, exp_trial_num, init_only):
    if build_only:
        os.system("make --always-make --silent -f makefile_make_g_file")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_make_g_file")

    init_filename = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.DOT_G
    final_filename = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.DOT_F_G

    init_file_path = os.path.join(CoreUtils.get_exp_data_dir_path(), "init_files", init_filename)
    fin_file_path = os.path.join(CoreUtils.get_exp_data_dir_path(), "fin_files", final_filename)

    out_init_file_path = os.path.join(CoreUtils.get_init_files_dir_path(), init_filename)
    out_final_file_path = os.path.join(CoreUtils.get_fin_files_dir_path(), final_filename)

    arguments_list = [init_file_path, out_init_file_path]
    arguments_string = " ".join(arguments_list)
    os.system("./make_g_files.exe " + arguments_string)

    if not init_only:
        arguments_list = [fin_file_path, out_final_file_path]
        arguments_string = " ".join(arguments_list)
        os.system("./make_g_files.exe " + arguments_string)


if __name__ == '__main__':
    main()
