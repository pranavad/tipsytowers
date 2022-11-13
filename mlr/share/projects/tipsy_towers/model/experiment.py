import os

from mlr.share.projects.tipsy_towers.model.file_generator import GFileGenerator
from mlr.share.projects.tipsy_towers.utils.constants import NameUtils, CoreUtils


class ExpType:
    STABILITY = "stab"

    @staticmethod
    def get_exp_type(input_string):
        return ExpType.STABILITY


class Experiment:
    def __init__(self, exp_type, exp_trial_num):

        self.exp_type = ExpType.get_exp_type(exp_type)
        self.exp_trial_num = exp_trial_num

        self.exp_name_to_print = self.exp_type + " " + str(self.exp_trial_num)

        self._block_names_list = None
        self._block_categories = None

    @staticmethod
    def get_block_names(exp_type, exp_trial_num=None):
        trial_num = exp_trial_num.split("_")[0]

        if exp_type == ExpType.STABILITY:
            total_b_blocks = 0
            total_c_blocks = 0

            if trial_num == "1":
                total_b_blocks = 15
            elif trial_num == "2":
                total_b_blocks = 8
                total_c_blocks = 8
            elif trial_num == "3":
                total_b_blocks = 8
                total_c_blocks = 9
            elif trial_num == "4":
                total_b_blocks = 18
            elif trial_num == "5":
                total_b_blocks = 10
                total_c_blocks = 9
            elif trial_num == "6":
                total_b_blocks = 20
            elif trial_num == "7":
                total_b_blocks = 21
            elif trial_num == "8":
                total_b_blocks = 22

            block_names = []
            block_name_categories = {}
            if total_b_blocks > 0:
                block_names.extend([NameUtils.NAME_DIFF_B + str(num) for num in range(total_b_blocks)])
                block_name_categories[NameUtils.NAME_DIFF_B] = total_b_blocks
            if total_c_blocks > 0:
                block_names.extend([NameUtils.NAME_DIFF_C + str(num) for num in range(total_c_blocks)])
                block_name_categories[NameUtils.NAME_DIFF_C] = total_c_blocks
            return block_names, block_name_categories
        return None

    def _get_init_file_path(self):
        filename = self.exp_type + CoreUtils.UNDERSCORE + self.exp_trial_num
        return os.path.join(CoreUtils.get_init_files_dir_path(), filename + CoreUtils.DOT_G)

    def _get_fin_file_path(self):
        filename = self.exp_type + CoreUtils.UNDERSCORE + self.exp_trial_num
        return os.path.join(CoreUtils.get_fin_files_dir_path(), filename + CoreUtils.DOT_F_G)

    def _get_temp_fin_file_path(self):
        filename = self.exp_type + CoreUtils.UNDERSCORE + self.exp_trial_num
        return os.path.join(CoreUtils.get_exp_fin_files_dir_path(), filename + CoreUtils.DOT_F_G)

    def _init_block_names_list(self):
        self._block_names_list, self._block_categories = Experiment.get_block_names(self.exp_type, self.exp_trial_num)

    def _initialize_experiment(self):
        self._init_block_names_list()

    def _run_g_file_generator(self):
        init_filename = self._get_init_file_path()
        final_filename = self._get_fin_file_path()
        temp_final_filename = self._get_temp_fin_file_path()
        trial_version = self.exp_trial_num.split("_")[1]

        block_nums = [self._block_categories[category] for category in self._block_categories.keys()]
        block_colors = [GFileGenerator.DEFAULT_COLOR]
        if len(self._block_categories.keys()) > 1:
            block_colors = [GFileGenerator.GREEN_COLOR, GFileGenerator.YELLOW_COLOR]

        block_loc = [GFileGenerator.TABLE_CENTER for i in range(len(block_colors))]

        if trial_version == "2":
            GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                 self.exp_trial_num, block_nums, block_colors, block_loc,
                                                 GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_IS_NO_NOISE)
        elif trial_version == "3":
            GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                 self.exp_trial_num, block_nums, block_colors, block_loc,
                                                 GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_IS_SMALL_NOISE)
        elif trial_version == "4":
            GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                 self.exp_trial_num, block_nums, block_colors, block_loc,
                                                 GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_IS_LARGE_NOISE)
        else:
            if self.exp_trial_num == '1_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '1_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_TRIPLE, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '1_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_TRIPLE, GFileGenerator.FIN_IS_LARGE_NOISE)
            elif self.exp_trial_num == '2_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '2_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_DOUBLE, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '2_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_DOUBLE, GFileGenerator.FIN_IS_LARGE_NOISE)
            elif self.exp_trial_num == '3_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '3_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_TRIPLE, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '3_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_TRIPLE, GFileGenerator.FIN_IS_SMALL_NOISE)
            elif self.exp_trial_num == '4_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '4_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_DOUBLE, GFileGenerator.FIN_IS_SMALL_NOISE)
            elif self.exp_trial_num == '4_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_DOUBLE, GFileGenerator.FIN_IS_LARGE_NOISE)
            elif self.exp_trial_num == '5_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '5_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_TRIPLE, GFileGenerator.FIN_IS_SMALL_NOISE)
            elif self.exp_trial_num == '5_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_TRIPLE, GFileGenerator.FIN_IS_LARGE_NOISE)
            elif self.exp_trial_num == '6_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '6_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '6_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_IS_NO_NOISE)
            if self.exp_trial_num == '7_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '7_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '7_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_IS_NO_NOISE)
            if self.exp_trial_num == '8_1':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_CUSTOM, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '8_5':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_DOUBLE, GFileGenerator.FIN_IS_NO_NOISE)
            elif self.exp_trial_num == '8_6':
                GFileGenerator.generate_trial_g_file(init_filename, final_filename, temp_final_filename,
                                                     self.exp_trial_num, block_nums, block_colors, block_loc,
                                                     GFileGenerator.FIN_DOUBLE, GFileGenerator.FIN_IS_SMALL_NOISE)

    def run_experiment(self):
        self._initialize_experiment()
        self._run_g_file_generator()
