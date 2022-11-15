import random
import subprocess

import numpy as np
import os

from mlr.share.projects.tipsy_towers.utils.compute_utils import sample_trunc_normal
from mlr.share.projects.tipsy_towers.utils.constants import CoreUtils
from mlr.share.projects.tipsy_towers.utils.file_utils import FileUtils
from mlr.share.projects.tipsy_towers.utils.msg_utils import Msg


class GFileGenerator:
    DEFAULT_COLOR = "[.92 .75 .52]"
    RED_COLOR = "[1 0 0]"
    BLUE_COLOR = "[0.007 0.8 0.99]"
    GREEN_COLOR = "[0 1 0]"
    YELLOW_COLOR = "[1 1 0]"
    PINK_COLOR = "[1 0.43 0.78]"

    TABLE_CENTER = "TABLE_CENTER"
    TABLE_LEFT_CENTER = "TABLE_LEFT_CENTER"
    TABLE_RIGHT_CENTER = "TABLE_RIGHT_CENTER"

    FIN_SINGLE = "FIN_SINGLE"
    FIN_DOUBLE = "FIN_DOUBLE"
    FIN_TRIPLE = "FIN_TRIPLE"
    FIN_CUSTOM = "FIN_CUSTOM"
    FIN_IS_NO_NOISE = "FIN_IS_NO_NOISE"
    FIN_IS_SMALL_NOISE = "FIN_IS_SMALL_NOISE"
    FIN_IS_LARGE_NOISE = "FIN_IS_LARGE_NOISE"

    SMALL_NOISE_MEAN = 0.0001
    SMALL_NOISE_VAR = 0.005
    LARGE_NOISE_MEAN = 0.004
    LARGE_NOISE_VAR = 0.013

    CENTER_LOC = 0.0, 0.0, 0.022
    PLACE_LEFT = 0.5, -0.3, 0.022
    PLACE_RIGHT = 0.5, 0.3, 0.022

    TOWER_GENERIC_LOC = 0.0, 0.0, 0.026
    TOWER_DOUBLE_LEFT = 0.0, -0.1, 0.026
    TOWER_DOUBLE_RIGHT = 0.0, 0.1, 0.026
    TOWER_TRIPLE_LEFT = 0.0, -0.15, 0.026
    TOWER_TRIPLE_CENTER = 0.0, 0.0, 0.026
    TOWER_TRIPLE_RIGHT = 0.0, 0.15, 0.026

    TABLE_X_LIM_MIN = -0.3
    TABLE_X_LIM_MAX = 0.3
    TABLE_Y_LIM_MIN = -0.6
    TABLE_Y_LIM_MAX = 0.6

    MIN_DIST_BETWEEN_BLOCKS = 0.1
    FIN_DIST_BETWEEN_BLOCKS = 0.005

    FIXED_NOISE_INCREMENT_VAL = 0.001
    SPLIT_BOUNDARY_VALUE = 0.02

    @staticmethod
    def _get_generic_body_line(body_name, body_color):
        return "body " + body_name + " { type=9 size=[.05 .05 .05 .005] color=" + body_color + " contact }\n"

    @staticmethod
    def _get_generic_joint_line(body_name, body_location="(0.0,0.0,0.026)", body_angle="0", block_below='table'):
        body_location_as_string = body_location.replace(",", " ")
        return "joint (" + block_below + " " + body_name + ")    { from=<T t" + \
               body_location_as_string + " d(" + \
               body_angle + " 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }\n"

    @staticmethod
    def extract_joint_data(joint_line):
        return tuple(float(i) for i in joint_line.split('from=<T t(')[1].split(')')[0].split(" "))

    @staticmethod
    def extract_refined_joint_data(joint_line):
        return tuple(float(i) for i in joint_line.split('type=JT_rigid from=<T ')[1].split(' > ')[0].split(" "))

    @staticmethod
    def _validate_sampled_loc(sampled_loc, all_loc_list, min_dist_between_blocks, fin_config):
        def _l2_distance(loc1, loc2):
            return np.sqrt((loc1[0] - loc2[0]) ** 2 + (loc1[1] - loc2[1]) ** 2)

        avoid_center_list = [GFileGenerator.FIN_SINGLE, GFileGenerator.FIN_DOUBLE, GFileGenerator.FIN_TRIPLE]
        if fin_config in avoid_center_list:
            if -0.01 < sampled_loc[1] < 0.01:
                return False

        for existing_loc in all_loc_list:
            if _l2_distance(existing_loc, sampled_loc) < min_dist_between_blocks:
                return False
        return True

    @staticmethod
    def _get_limits(limit, table_location):
        x_center = 0.0
        y_center = 0.0
        x_limit_min = max(-limit, GFileGenerator.TABLE_X_LIM_MIN)
        x_limit_max = min(limit, GFileGenerator.TABLE_X_LIM_MAX)
        y_limit_min = max(-limit, GFileGenerator.TABLE_Y_LIM_MIN)
        y_limit_max = min(limit, GFileGenerator.TABLE_Y_LIM_MAX)
        if table_location == GFileGenerator.TABLE_LEFT_CENTER:
            y_center = -0.25
            y_limit_min += y_center
            y_limit_max = -0.1
        if table_location == GFileGenerator.TABLE_RIGHT_CENTER:
            y_center = 0.25
            y_limit_min = 0.1
            y_limit_max += y_center
        return (x_center, y_center), x_limit_min, x_limit_max, y_limit_min, y_limit_max

    @staticmethod
    def _sample_loc_on_table(loc_list, table_loc, total_blocks_num, min_dist_between_blocks, fin_config) -> tuple:
        limit = 0.001 * total_blocks_num
        center, x_limit_min, x_limit_max, y_limit_min, y_limit_max = GFileGenerator._get_limits(limit, table_loc)

        sampled_loc = [sample_trunc_normal(center[0], x_limit_min, x_limit_max),
                       sample_trunc_normal(center[1], y_limit_min, y_limit_max)]
        while not GFileGenerator._validate_sampled_loc(sampled_loc, loc_list, min_dist_between_blocks, fin_config):
            sampled_loc = [sample_trunc_normal(center[0], x_limit_min, x_limit_max),
                           sample_trunc_normal(center[1], y_limit_min, y_limit_max)]
            limit += 0.001
            center, x_limit_min, x_limit_max, y_limit_min, y_limit_max = GFileGenerator._get_limits(limit, table_loc)

        sampled_loc += [0.022]
        return tuple(sampled_loc)

    @staticmethod
    def generate_trial_g_file(out_init_file_path, out_fin_file_path, out_exp_fin_file_path,
                              init_image_file_path, final_image_file_path, trial_name,
                              div_block_num: list, div_color: list, div_table_loc: list, fin_config, fin_noise):
        """
        creates a .g file with a randomly assigned initial configuration of the world
        NOTE: total number of block types must not exceed 2
        :param out_init_file_path       : file path of the init output file
        :param out_fin_file_path        : file path of the final output file
        :param out_exp_fin_file_path    : file path of the temporary location for final output file
        :param init_image_file_path     : file path of the init output image file
        :param final_image_file_path    : file path of the final output image file
        :param trial_name               : set when there is some special initial configuration to load
        :param div_block_num            : a list representing the number of blocks in each category
        :param div_color                : a list representing the colors used for each category of blocks
        :param div_table_loc            : a list that orients each of the 'bins' to a particular side of the table
        :param fin_config               : the configuration information of the final structure of blocks
        :param fin_noise                : the level of noise in the final structure
        """
        Msg.print_success_thin("Running trial: " + str(trial_name))

        if not len(div_block_num) == len(div_color):
            Msg.print_error("ERROR [init_g_random]: bins and colors are not correctly defined")
            assert False

        init_g_contents = []
        fin_g_contents = []

        special_fin_before_init = ["1_1", "2_1", "4_1", "6_1", "7_1", "8_1"]  # trials whose fin should before init
        special_split_trial_names = ["3_1", "5_1"]  # trials whose fin should be split

        generic_file_path = os.path.join(CoreUtils.get_exp_data_dir_path(), "generic.txt")

        with open(generic_file_path, "r") as header_file:
            for line in header_file:
                init_g_contents.append(line)
                fin_g_contents.append(line)

        loc_avoid_list = [GFileGenerator.PLACE_LEFT, GFileGenerator.PLACE_RIGHT, GFileGenerator.CENTER_LOC]
        if fin_config == GFileGenerator.FIN_DOUBLE:
            loc_avoid_list.extend([GFileGenerator.TOWER_DOUBLE_LEFT, GFileGenerator.TOWER_DOUBLE_RIGHT])
        elif fin_config == GFileGenerator.FIN_TRIPLE:
            loc_avoid_list.extend([GFileGenerator.TOWER_TRIPLE_LEFT, GFileGenerator.TOWER_TRIPLE_RIGHT])

        block_name_character = 'b'
        b_counter = 0
        c_counter = 0
        counter = 0

        block_names_list = []
        block_colors_list = []
        for div_index, num_of_blocks in enumerate(div_block_num):
            for block_counter in range(counter, num_of_blocks + counter):
                block_names_list.append(block_name_character + str(block_counter))
                block_colors_list.append(div_color[div_index])
            block_name_character = chr(ord(block_name_character) + 1)
            if block_name_character == "d":
                block_name_character = 'b'

            if block_name_character == "b":
                b_counter += num_of_blocks
                counter = c_counter
            else:
                c_counter += num_of_blocks
                counter = b_counter

        if trial_name.startswith("3") or trial_name.startswith("5") and not trial_name.endswith("1"):
            num = min(div_block_num[0], div_block_num[1])
            result = [None] * (num * 2)
            result[::2] = block_colors_list[:div_block_num[0]][:num]
            result[1::2] = block_colors_list[div_block_num[0]:][:num]
            result.extend(block_colors_list[:div_block_num[0]][num:])
            result.extend(block_colors_list[div_block_num[0]:][num:])
            block_colors_list = result

        for block_name, block_color in zip(block_names_list, block_colors_list):
            init_g_contents.append(GFileGenerator._get_generic_body_line(block_name, block_color))
            fin_g_contents.append(GFileGenerator._get_generic_body_line(block_name, block_color))

        # ---------- GET FINAL FOR SPECIAL AND TOWER-LIKE TRIALS
        block_loc_list = []
        if trial_name in special_fin_before_init:  # special trials
            if trial_name == "1_1":
                block_loc_list += GFileGenerator._get_block_hor_vert_rows(0, 5, 0.1)
                block_loc_list += GFileGenerator._get_block_hor_vert_rows(0, 5, 0.0)
            elif trial_name == "2_1":
                block_loc_list = GFileGenerator._get_block_hor_vert_rows(4, 4)
            elif trial_name == "4_1":
                block_loc_list = GFileGenerator._get_block_hor_vert_rows(4, 5)
            elif trial_name == "6_1":
                block_loc_list = GFileGenerator._get_block_hor_vert_rows(len(block_names_list), 0)
            elif trial_name == "7_1":
                block_loc_list = GFileGenerator._get_block_hor_vert_rows(len(block_names_list), 0)
            elif trial_name == "8_1":
                block_loc_list += GFileGenerator._get_block_hor_vert_rows(6, 0)
                block_loc_list += GFileGenerator._get_block_hor_vert_rows(0, 4, 0.1875 +
                                                                          GFileGenerator.FIN_DIST_BETWEEN_BLOCKS)

            loc_avoid_list.extend(block_loc_list)

            for block_name, block_loc in zip(block_names_list, block_loc_list):
                fin_g_contents.append(GFileGenerator._get_generic_joint_line(block_name, str(block_loc)))

            FileUtils.write_content_to_file(out_fin_file_path, fin_g_contents)
            GFileGenerator.run_file_generator(out_fin_file_path, out_fin_file_path, final_image_file_path)
        elif trial_name in special_split_trial_names:
            pass
        else:   # tower-like trials
            fin_g_contents.extend(GFileGenerator.get_tower_contents(fin_config, block_names_list))
            GFileGenerator.generate_towers(fin_g_contents, fin_noise, block_names_list,
                                           out_fin_file_path, out_exp_fin_file_path, final_image_file_path)

        # ---------- GET INIT
        block_loc_list = [] + loc_avoid_list
        block_rot_list = []
        for div_index, num_of_blocks in enumerate(div_block_num):
            for block_counter in range(num_of_blocks):
                sampled_loc = GFileGenerator._sample_loc_on_table(block_loc_list, div_table_loc[div_index],
                                                                  len(block_names_list),
                                                                  GFileGenerator.MIN_DIST_BETWEEN_BLOCKS, fin_config)
                block_loc_list.append(sampled_loc)
                block_rot_list.append(np.random.uniform(0, 90, 1)[0])

        block_loc_list = block_loc_list[len(loc_avoid_list):]
        random.shuffle(block_loc_list)

        block_names_to_ignore_list = []
        if trial_name.startswith("6") or trial_name.startswith("7"):
            total_blocks_to_fix = 8
            if trial_name.startswith("7"):
                total_blocks_to_fix = 5

            if trial_name.endswith("1"):
                horizontal_block_loc_list = GFileGenerator._get_block_hor_vert_rows(len(block_names_list), 0)
                horizontal_block_loc_list = sorted(horizontal_block_loc_list, key=lambda x: x[1])
                block_loc_list = horizontal_block_loc_list[:total_blocks_to_fix] + block_loc_list[total_blocks_to_fix:]
                block_rot_list = [0.0] * total_blocks_to_fix + block_rot_list[total_blocks_to_fix:]
            else:
                block_names_to_ignore_list = block_names_list[:total_blocks_to_fix]
                init_g_contents.extend(GFileGenerator.get_tower_contents(GFileGenerator.FIN_SINGLE,
                                                                         block_names_to_ignore_list))

        for block_name, block_loc, theta in zip(block_names_list, block_loc_list, block_rot_list):
            if block_name in block_names_to_ignore_list:
                continue
            init_g_contents.append(GFileGenerator._get_generic_joint_line(block_name, str(block_loc), str(theta)))

        if trial_name in special_split_trial_names:
            final_block_loc_list, final_block_rot_list = GFileGenerator._get_block_loc_35(block_names_list,
                                                                                          loc_avoid_list,
                                                                                          block_loc_list,
                                                                                          block_rot_list,
                                                                                          fin_config)
            for block_name, block_loc, theta in zip(block_names_list, final_block_loc_list, final_block_rot_list):
                fin_g_contents.append(GFileGenerator._get_generic_joint_line(block_name, str(block_loc), str(theta)))

            FileUtils.write_content_to_file(out_fin_file_path, fin_g_contents)
            GFileGenerator.run_file_generator(out_fin_file_path, out_fin_file_path, final_image_file_path)

        FileUtils.write_content_to_file(out_init_file_path, init_g_contents)
        GFileGenerator.run_file_generator(out_init_file_path, out_init_file_path, init_image_file_path)

    @staticmethod
    def generate_noisy_towers(input_file_path, out_file_path, image_file_path,
                              block_names_list, mean, bound_min, bound_max, sigma,
                              block_names_to_move=None, fixed_noise_dict=None, multiplier=1):
        def _get_random_truncated_normal():
            random_value = sample_trunc_normal(mean, bound_min, bound_max, sigma)
            return round(random_value, 5)

        def _get_random_sign():
            return np.random.randint(0, 2) * 2 - 1

        def _add_noise_to_joint_data(joint_line, fixed_noise=None):
            split1 = 'type=JT_rigid from=<T '
            split2 = ' > to'
            part1 = joint_line.split(split1)[0]
            part2 = joint_line.split(split1)[1].split(split2)[0].split(" ")
            part3 = joint_line.split(split1)[1].split(split2)[1]

            if fixed_noise is not None:
                part2[0] = str(float(part2[0]) + fixed_noise[0])
                part2[1] = str(float(part2[1]) + fixed_noise[1])
            else:
                part2[0] = str(float(part2[0]) + _get_random_sign() * _get_random_truncated_normal())
                part2[1] = str(float(part2[1]) + _get_random_sign() * _get_random_truncated_normal())

            return "".join([part1, split1, " ".join(part2), split2, part3])

        def _get_block_names_from_joint(joint_line):
            split1 = 'joint ('
            split2 = '){'
            return joint_line.split(split1)[1].split(split2)[0].split(" ")

        g_file_contents = []

        file_data = open(input_file_path, "r").read()
        file_data = file_data.split("\n")

        for line in file_data:
            if "joint" in line and "table" not in line:
                block_name_below, block_name = _get_block_names_from_joint(line)

                noise = None
                if block_names_to_move is not None:
                    if block_name in block_names_to_move:
                        noise = [b * multiplier for b in fixed_noise_dict[block_name]]
                    else:
                        noise = 0.0, 0.0

                if block_name_below in block_names_list and block_name in block_names_list:
                    g_file_contents.append(_add_noise_to_joint_data(line, noise) + "\n")
            else:
                g_file_contents.append(line + "\n")

        FileUtils.write_content_to_file(out_file_path, g_file_contents)
        GFileGenerator.run_file_generator(out_file_path, out_file_path, image_file_path)

    @staticmethod
    def _get_block_hor_vert_rows(total_horizontal=2, total_vertical=2, max_block_y=None):
        block_loc = []

        block_size = 0.05

        total_vertical = int(np.ceil(total_vertical / 2))
        total_horizontal = int(np.ceil(total_horizontal / 2))

        start_block_y = 0.0
        if total_horizontal > 0 and (total_horizontal * 2) % 2 == 0:
            start_block_y = block_size / 2 + GFileGenerator.FIN_DIST_BETWEEN_BLOCKS / 2

        if max_block_y is None:
            max_block_y = start_block_y + (block_size + GFileGenerator.FIN_DIST_BETWEEN_BLOCKS) * (total_horizontal - 1)

        start_block_x = 0.0
        if total_vertical > 0 and (total_vertical * 2) % 2 == 0:
            start_block_x = block_size / 2 + GFileGenerator.FIN_DIST_BETWEEN_BLOCKS / 2

        for _ in range(total_vertical):  # vertical
            if max_block_y == 0.0:
                block_loc.append(tuple([start_block_x, max_block_y, 0.022]))
                if not start_block_x == 0.0:
                    block_loc.append(tuple([-start_block_x, max_block_y, 0.022]))
                start_block_x += block_size + GFileGenerator.FIN_DIST_BETWEEN_BLOCKS
            else:
                block_loc.append(tuple([start_block_x, max_block_y, 0.022]))
                block_loc.append(tuple([start_block_x, -max_block_y, 0.022]))
                if not start_block_x == 0.0:
                    block_loc.append(tuple([-start_block_x, max_block_y, 0.022]))
                    block_loc.append(tuple([-start_block_x, -max_block_y, 0.022]))
                start_block_x += block_size + GFileGenerator.FIN_DIST_BETWEEN_BLOCKS

        for _ in range(total_horizontal):  # horizontal
            if start_block_x == 0.0:
                block_loc.append(tuple([start_block_x, start_block_y, 0.022]))
                if not start_block_y == 0.0:
                    block_loc.append(tuple([start_block_x, -start_block_y, 0.022]))
                start_block_y += block_size + GFileGenerator.FIN_DIST_BETWEEN_BLOCKS
            else:
                block_loc.append(tuple([start_block_x, start_block_y, 0.022]))
                block_loc.append(tuple([-start_block_x, start_block_y, 0.022]))
                if not start_block_y == 0.0:
                    block_loc.append(tuple([start_block_x, -start_block_y, 0.022]))
                    block_loc.append(tuple([-start_block_x, -start_block_y, 0.022]))
                start_block_y += block_size + GFileGenerator.FIN_DIST_BETWEEN_BLOCKS

        return block_loc

    @staticmethod
    def _get_block_loc_35(block_names_list, loc_avoid_list, block_loc_list, block_rot_list, fin_config):
        total_blocks = len(block_names_list)

        final_b_block_loc_list = [] + loc_avoid_list + block_loc_list
        final_c_block_loc_list = [] + loc_avoid_list + block_loc_list
        b_blocks_to_move = []
        c_blocks_to_move = []

        final_block_loc_list = []
        for block_name, block_loc in zip(block_names_list, block_loc_list):
            if block_name.startswith("b") and block_loc[1] > -GFileGenerator.SPLIT_BOUNDARY_VALUE:
                b_blocks_to_move.append(block_name)
            if block_name.startswith("b") and block_loc[1] <= -GFileGenerator.SPLIT_BOUNDARY_VALUE:
                final_b_block_loc_list.append(block_loc)

            if block_name.startswith("c") and block_loc[1] < GFileGenerator.SPLIT_BOUNDARY_VALUE:
                c_blocks_to_move.append(block_name)
            if block_name.startswith("c") and block_loc[1] >= GFileGenerator.SPLIT_BOUNDARY_VALUE:
                final_c_block_loc_list.append(block_loc)

        for block_name, block_loc in zip(block_names_list, block_loc_list):
            if block_name in b_blocks_to_move:
                sampled_loc = GFileGenerator._sample_loc_on_table(final_b_block_loc_list,
                                                                  GFileGenerator.TABLE_LEFT_CENTER,
                                                                  total_blocks,
                                                                  GFileGenerator.MIN_DIST_BETWEEN_BLOCKS, fin_config)
                final_b_block_loc_list.append(sampled_loc)
                final_block_loc_list.append(sampled_loc)
            elif block_name in c_blocks_to_move:
                sampled_loc = GFileGenerator._sample_loc_on_table(final_c_block_loc_list,
                                                                  GFileGenerator.TABLE_RIGHT_CENTER,
                                                                  total_blocks,
                                                                  GFileGenerator.MIN_DIST_BETWEEN_BLOCKS, fin_config)
                final_c_block_loc_list.append(sampled_loc)
                final_block_loc_list.append(sampled_loc)
            else:
                final_block_loc_list.append(block_loc)

        final_block_rot_list = []
        for block_name, block_rot in zip(block_names_list, block_rot_list):
            if block_name in b_blocks_to_move or block_name in c_blocks_to_move:
                final_block_rot_list.append(np.random.uniform(0, 90, 1)[0])
            else:
                final_block_rot_list.append(block_rot)

        return final_block_loc_list, final_block_rot_list

    @staticmethod
    def _generate_tower(block_names_list, start_loc):
        g_contents = []
        for counter, (block_name_below, block_name) in enumerate(zip(block_names_list, block_names_list[1:])):
            if counter == 0:
                g_contents.append(GFileGenerator._get_generic_joint_line(block_name_below, body_location=start_loc))

            g_contents.append(GFileGenerator._get_generic_joint_line(block_name, block_below=block_name_below))
        return g_contents

    @staticmethod
    def get_tower_contents(fin_config, block_names_list):
        g_contents = []

        generic_loc = str(GFileGenerator.TOWER_GENERIC_LOC)

        double_left = str(GFileGenerator.TOWER_DOUBLE_LEFT)
        double_right = str(GFileGenerator.TOWER_DOUBLE_RIGHT)

        triple_left = str(GFileGenerator.TOWER_TRIPLE_LEFT)
        triple_center = str(GFileGenerator.TOWER_TRIPLE_CENTER)
        triple_right = str(GFileGenerator.TOWER_TRIPLE_RIGHT)

        if fin_config == GFileGenerator.FIN_SINGLE:
            g_contents.extend(GFileGenerator._generate_tower(block_names_list, generic_loc))
        elif fin_config == GFileGenerator.FIN_DOUBLE:
            g_contents.extend(GFileGenerator._generate_tower(block_names_list[:len(block_names_list)//2], double_left))
            g_contents.extend(GFileGenerator._generate_tower(block_names_list[len(block_names_list)//2:], double_right))
        elif fin_config == GFileGenerator.FIN_TRIPLE:
            split1 = block_names_list[:len(block_names_list)//3]
            if len(block_names_list) % 3 == 0:
                split2 = block_names_list[len(block_names_list)//3:2 * len(block_names_list)//3]
                split3 = block_names_list[2 * len(block_names_list) // 3:]
            else:
                split2 = block_names_list[len(block_names_list)//3:2 * len(block_names_list)//3 + 1]
                split3 = block_names_list[2 * len(block_names_list) // 3 + 1:]
            g_contents.extend(GFileGenerator._generate_tower(split1, triple_left))
            g_contents.extend(GFileGenerator._generate_tower(split2, triple_center))
            g_contents.extend(GFileGenerator._generate_tower(split3, triple_right))

        return g_contents

    @staticmethod
    def generate_towers(fin_file_contents, fin_noise, block_names_list,
                        out_file_path, out_temp_file_path, image_file_path):
        block_names_to_move = random.sample([b for b in block_names_list if '0' not in b], random.randint(1, 4))
        fixed_noise_dict = {}
        for block_name in block_names_to_move:
            noise_tuple = [(random.randint(0, 1) * 2 - 1) * GFileGenerator.FIXED_NOISE_INCREMENT_VAL,
                           (random.randint(0, 1) * 2 - 1) * GFileGenerator.FIXED_NOISE_INCREMENT_VAL]
            fixed_noise_dict[block_name] = noise_tuple

        is_stable = False
        while not is_stable:
            FileUtils.write_content_to_file(out_file_path, fin_file_contents)
            GFileGenerator.run_file_generator(out_file_path, out_file_path)

            if fin_noise == GFileGenerator.FIN_IS_SMALL_NOISE:
                GFileGenerator.generate_noisy_towers(out_file_path, out_file_path, image_file_path, block_names_list,
                                                     GFileGenerator.SMALL_NOISE_MEAN,
                                                     -GFileGenerator.SMALL_NOISE_VAR,
                                                     GFileGenerator.SMALL_NOISE_VAR,
                                                     GFileGenerator.SMALL_NOISE_VAR)
                is_stable = GFileGenerator.is_block_config_stable(out_file_path, block_names_list)
            elif fin_noise == GFileGenerator.FIN_IS_LARGE_NOISE:
                GFileGenerator.generate_noisy_towers(out_file_path, out_file_path, image_file_path, block_names_list,
                                                     GFileGenerator.LARGE_NOISE_MEAN,
                                                     -GFileGenerator.LARGE_NOISE_VAR,
                                                     GFileGenerator.LARGE_NOISE_VAR,
                                                     GFileGenerator.LARGE_NOISE_VAR)

                is_stable = GFileGenerator.is_block_config_stable(out_file_path, block_names_list)
            else:
                GFileGenerator.run_file_generator(out_file_path, out_file_path, image_file_path)
                is_stable = True

        if fin_noise == GFileGenerator.FIN_IS_LARGE_NOISE:
            counter = 0
            while is_stable:
                counter += 1
                GFileGenerator.generate_noisy_towers(out_file_path, out_temp_file_path, "N", block_names_list,
                                                     GFileGenerator.LARGE_NOISE_MEAN,
                                                     -GFileGenerator.LARGE_NOISE_VAR,
                                                     GFileGenerator.LARGE_NOISE_VAR,
                                                     GFileGenerator.LARGE_NOISE_VAR,
                                                     block_names_to_move, fixed_noise_dict, counter)
                is_stable = GFileGenerator.is_block_config_stable(out_temp_file_path, block_names_list)

                if counter == 6:
                    break

            counter -= 1
            GFileGenerator.generate_noisy_towers(out_file_path, out_file_path, image_file_path, block_names_list,
                                                 GFileGenerator.LARGE_NOISE_MEAN,
                                                 -GFileGenerator.LARGE_NOISE_VAR,
                                                 GFileGenerator.LARGE_NOISE_VAR,
                                                 GFileGenerator.LARGE_NOISE_VAR,
                                                 block_names_to_move, fixed_noise_dict, counter)
            is_stable = GFileGenerator.is_block_config_stable(out_file_path, block_names_list)
            if not is_stable:
                Msg.print_error("ERROR [create_noisy_towers]: generated tower is still unstable")
                assert False

    @staticmethod
    def is_block_config_stable(file_path, block_names_list):
        arguments_list = [file_path, "#".join(block_names_list)]
        arguments_string = " ".join(arguments_list)
        return "FALLEN" not in str(subprocess.check_output("./run_risk_test.exe " + arguments_string, shell=True))

    @staticmethod
    def run_file_generator(input_file_path, out_file_path, image_file_path="N"):
        arguments_list = [input_file_path, out_file_path, image_file_path, "0"]
        if len(image_file_path) > 1:
            arguments_list = [input_file_path, out_file_path, image_file_path, "1"]
        arguments_string = " ".join(arguments_list)
        return subprocess.check_output("./file_generator.exe " + arguments_string, shell=True)


if __name__ == '__main__':
    t_blocks = 15
    test_init_file_path = os.path.join(CoreUtils.get_test_files_data_dir_path(), "test_init.g")
