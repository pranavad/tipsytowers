import csv
import os

from mlr.share.projects.integrated_model.utils.block_utils import BlockParams, BlockPosition, BlockQuaternion, \
    BlockShape
from mlr.share.projects.integrated_model.utils.compute_utils import is_close
from mlr.share.projects.integrated_model.utils.constants import CoreUtils, NameUtils
from mlr.share.projects.integrated_model.utils.msg_utils import Msg


class GFileUtils:

    @staticmethod
    def parse_g_file_path(g_file_path, block_names_list):
        file_data = open(g_file_path, "r").read()
        file_data = file_data.split("\n")

        body_position_data = {}
        body_rotation_data = {}

        joint_data = {}

        shape_data = {}

        def extract_joint_data(joint_line, block_name):
            return joint_line.split('joint (')[1].split(' ' + block_name)[0].lower()

        def extract_body_data(body_line):
            return body_line.split("<T ")[1].split(" >")[0].split(" ")

        def extract_shape_data(shape_line):
            return shape_line.split("type=ST_ssBox size=[")[1].split("]  color=")[0].split(" ")

        for line in file_data:
            for block_name_key in block_names_list:
                if "body " + block_name_key + " " in line:
                    body_data = extract_body_data(line)
                    body_position_data[block_name_key] = tuple(float(x) for x in body_data[:3])
                    body_rotation_data[block_name_key] = tuple(float(x) for x in body_data[3:])
                    continue

                if "joint" in line and block_name_key + ")" in line:
                    joint_data[block_name_key] = extract_joint_data(line, block_name_key)
                    continue

                if "shape" in line and block_name_key + " " in line:
                    shape_data[block_name_key] = extract_shape_data(line)
                    continue

        return body_position_data, body_rotation_data, joint_data, shape_data

    @staticmethod
    def get_block_params_from_parse_g_file_path(file_path, block_names_list):
        pos_dict, rot_dict, resting_on_dict, shape_dict = GFileUtils.parse_g_file_path(file_path, block_names_list)

        for block_name in block_names_list:
            if block_name not in pos_dict.keys():
                Msg.print_error("ERROR [get_block_params_from_parse_g_file_path]: " + block_name + " data not found!")
                assert False

        is_supporting_dict = {}
        total_blocks_supported = {}
        for key in block_names_list:
            is_supporting_dict[key] = NameUtils.NO_BLOCK_ON_TOP
            total_blocks_supported[key] = 0

        for key in block_names_list:
            resting_on_name = resting_on_dict[key]

            if resting_on_name not in is_supporting_dict:
                continue

            if not resting_on_name == NameUtils.TABLE:
                is_supporting_dict[resting_on_name.lower()] = key
                total_blocks_supported[resting_on_name.lower()] += 1

        blocks_params_dict = {}
        for block_name in block_names_list:
            pos = pos_dict[block_name]
            rot = rot_dict[block_name]
            shape = shape_dict[block_name]
            resting_on_name = resting_on_dict[block_name]
            is_supporting = is_supporting_dict[block_name]
            two_blocks_supported = total_blocks_supported[block_name] == 2
            block_params = BlockParams(BlockPosition(pos[0], pos[1], pos[2]),
                                       BlockQuaternion(rot[0], rot[1], rot[2], rot[3]),
                                       BlockShape(shape[0], shape[1], shape[2], shape[3]),
                                       resting_on_name,
                                       is_supporting,
                                       two_blocks_supported)
            blocks_params_dict[block_name] = block_params

        return blocks_params_dict

    @staticmethod
    def clean_g_file(g_file_path):
        new_contents = []
        with open(g_file_path, "r") as g_file:
            for line in g_file:
                new_joint_line = line

                if line.startswith("joint"):
                    if "Q=<" in line:
                        new_joint_line = line.split("Q=<")[0] + " }\n"
                    if "to=<T" in line:
                        to_key = " to=<T "
                        quaternion = new_joint_line.split(to_key)[1].split(" ")[3:7]
                        condition = all([is_close(float(value), 0.0, 2) for value in quaternion[1:]])

                        if condition:  # whether all are 0 (invalid quaternion) or last 3 are zeros (floating point)
                            l_start = new_joint_line.split(to_key)[0]
                            l_pos_list = " ".join(new_joint_line.split("to=<T")[1].split(" ")[0:4])
                            l_quaternion_list = "-0.99999 0.00001 0.00001 0.00001"
                            l_end = ">  }\n"
                            new_joint_line = " ".join([l_start, to_key, l_pos_list, l_quaternion_list, l_end])

                new_contents.append(new_joint_line)

        with open(g_file_path, "w") as g_file:
            for line in new_contents:
                g_file.write(line)

    @staticmethod
    def redefine_init_g_file(init_file_path, fin_file_path, block_names_list):
        with open(init_file_path) as init_file:
            init_file_data = init_file.read()

        init_block_params_dict = GFileUtils.get_block_params_from_parse_g_file_path(init_file_path, block_names_list)
        final_block_params_dict = GFileUtils.get_block_params_from_parse_g_file_path(fin_file_path, block_names_list)

        assignments_b, assignments_c = BlockParams.get_hungarian_algorithm_fits(init_block_params_dict,
                                                                                final_block_params_dict)

        # Rename blocks in init world file string corresponding to assignments found
        init_file_data_copy = init_file_data[:]

        # Rename for b blocks
        for i in reversed(range(len(assignments_b[0]))):
            init_file_data_copy = init_file_data_copy.replace('b' + str(assignments_b[0][i]),
                                                              'TEMP' + str(assignments_b[1][i]))

        init_file_data_copy = init_file_data_copy.replace('TEMP', 'b')

        # # Rename for c blocks
        for i in reversed(range(len(assignments_c[0]))):
            init_file_data_copy = init_file_data_copy.replace('c' + str(assignments_c[0][i]),
                                                              'TEMP' + str(assignments_c[1][i]))

        init_file_data_copy = init_file_data_copy.replace('TEMP', 'c')

        with open(init_file_path, 'w') as init_file:
            init_file.write(init_file_data_copy)


class FileUtils:
    @staticmethod
    def create_out_file(out_file_path):
        file = open(out_file_path, 'wb')
        file.close()

    @staticmethod
    def write_row_to_file(out_file_path, row_data):
        """
        Writes a new row into the given file
        :param out_file_path: name of the file that is to be written in
        :param row_data: the entire row entry that is to be written into the file
        :return: None
        """
        if not os.path.isfile(out_file_path):
            FileUtils.create_out_file(out_file_path)

        try:
            file = open(out_file_path, 'a')
            writer = csv.writer(file)
            writer.writerow(row_data)
            file.close()

        except Exception as e:
            Msg.print_error("ERROR [write_row_to_file]: could not write into " + out_file_path)
            Msg.print_error(str(e))
            assert False


# class GFileGenerator:
#     DEFAULT_COLOR = "[.92 .75 .52]"
#     RED_COLOR = "[1 0 0]"
#     BLUE_COLOR = "[0 0 1]"
#     GREEN_COLOR = "[0 1 0]"
#     YELLOW_COLOR = "[1 1 0]"
#     PURPLE_COLOR = "[0.5 0 0.5]"
#
#     TABLE_X_LIM = 0.3
#     TABLE_Y_LIM = 0.6
#
#     TABLE_CENTER = (0, 0, 0.022)
#     TABLE_LEFT = (0, -.25, 0.022)
#     TABLE_RIGHT = (0, .25, 0.022)
#
#     @staticmethod
#     def init_g_random(out_filename, bins: list, colors: list, partition: list):
#         """
#         creates a .g file with a randomly assigned initial configuration of the world
#
#         NOTE: total number of block types must not exceed 2
#
#         :param out_filename : name of the output file
#         :param bins         : a list representing the number of blocks in each category
#         :param colors       : a list representing the colors used for each category of blocks
#         :param partition    : a list that orients each of the 'bins' to a particular side of the table
#         """
#
#         def _get_generic_body_line(body_name, body_color):
#             return "body " + body_name + " { type=9 size=[.05 .05 .05 .005] color=" + body_color + " contact }\n"
#
#         def _get_generic_joint_line(body_name, body_location, body_angle, body_color):
#             body_location_as_string = body_location.replace(",", " ")
#             return "joint (table " + body_name + ")    { from=<T t" + \
#                    body_location_as_string + " d(" + \
#                    body_angle + " 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }\n"
#
#         def _sample_point_on_table(x_limit, y_limit, table_location):
#             point = np.random.uniform(-x_limit, x_limit, 1)[0], np.random.uniform(-y_limit, y_limit, 1)[0]
#             return tuple([sum(value) for value in zip(table_location, point)])
#
#         def _is_sampled_point_good(sampled_point, all_points_list):
#             def _get_l2_distance(point1, point2):
#                 return np.sqrt((point1[0] - point2[0]) ** 2 +
#                                (point1[1] - point2[1]) ** 2 +
#                                (point1[2] - point2[2]) ** 2)
#
#             radius_collision_circle = 0.5 * np.sqrt(0.05 * 0.05 + 0.05 * 0.05)
#             eps_collision_circle = 0.005
#
#             for existing_point in all_points_list:
#                 l2_dist = _get_l2_distance(existing_point, sampled_point)
#                 if l2_dist < 2 * radius_collision_circle * eps_collision_circle:
#                     return False
#             return True
#
#         if not len(bins) == len(colors):
#             Msg.print_error("ERROR [init_g_random]: bins and colors are not correctly defined")
#             assert False
#
#         num_blocks_1 = bins[0]
#
#         num_blocks_2 = 0
#         table_location_partition1 = GFileGenerator.TABLE_CENTER
#         table_location_partition2 = GFileGenerator.TABLE_CENTER
#         if len(bins) == 2:
#             num_blocks_2 = bins[1]
#             table_location_partition1 = GFileGenerator.TABLE_LEFT
#             table_location_partition2 = GFileGenerator.TABLE_RIGHT
#
#         elif len(bins) > 2:
#             Msg.print_error("ERROR [init_g_random]: bins entered are not supported, please use length of 1 OR 2")
#             assert False
#
#         new_contents = []
#
#         generic_file_path = os.path.join(CoreUtils.get_intermediate_files_dir_path(), "generic.txt")
#         with open(generic_file_path, "r") as header_file:
#             for line in header_file:
#                 new_contents.append(line)
#
#         all_blocks_list = set()
#
#         with open(out_filename, "w") as g_file:
#             for line in new_contents:
#                 g_file.write(line)
#
#         block_names_list = []
#         block_colors_list = []
#
#         total_blocks_counter = 0
#         b_counter = 0  # primary colored blocks
#         c_counter = 0  # secondary colored blocks
#         for i in range(num_blocks_1):
#             block_string = "b"
#             block_names_list.append(block_string + " " + str(b_counter))
#             block_colors_list.append(colors[0])
#             b_counter += 1
#
#         total_blocks_counter += b_counter
#
#         for i in range(num_blocks_2):
#             block_string = "c"
#             block_names_list.append(block_string + " " + str(c_counter))
#             block_colors_list.append(colors[1])
#             c_counter += 1
#
#         total_blocks_counter += c_counter
#
#         for block_details in zip(block_names_list, block_colors_list):
#             new_contents.append(_get_generic_body_line(block_details[0], block_details[1]))
#
#         while not is_good_sample:
#             is_good_sample = True
#
#             x_lim = min(0.3 * total_blocks / 16, GFileGenerator.TABLE_X_LIM)
#             y_lim = min(0.3 * total_blocks / 16, GFileGenerator.TABLE_Y_LIM)
#             rand_loc = add_coords(table_location_partition1, (x, y, 0))
#
#             for loc in all_blocks_loc:
#                 if distance(loc, rand_loc) < (2 * radius_collision_circle + eps_collision_circle):
#                     is_good_sample = False
#                     x_lim = x_lim - 0.0001
#                     y_lim = y_lim + 0.0001
#         all_blocks_loc.add(rand_loc)
#
#         theta = np.random.uniform(0, 90, 1)[0]
#
#             mg.write("joint (table " + block_string + str(block_counter) + ")    { from=<T t"
#                      + str(rand_loc).replace(',', ' ') + " d("
#                      + str(theta) + " 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }\n")
#
#         if len(bins) > 1 and not partition[0] == 'CENTRE':
#             # Sample second bin
#             if fixed_config[1] == 'NONE':
#                 for i in range(num_blocks_2):
#                     color = colors[1]
#                     block_string = "c"
#                     block_counter = c_counter
#                     c_counter += 1
#
#                     mg.write("body " + block_string + str(
#                         block_counter) + " { type=9 size=[.05 .05 .05 .005] color=" + color + "  contact }\n")
#
#                     x_lim = min(0.3 * num_blocks_2 / 16, 0.4)
#                     y_lim = min(0.3 * num_blocks_2 / 16, 0.7)
#                     # Radius of circle that a cube occupies
#                     radius_collision_circle = 0.5 * np.sqrt(0.05 * 0.05 + 0.05 * 0.05)
#                     # margin for collisions
#                     eps_collision_circle = 0.005
#                     # Generate random location
#                     is_good_sample = False
#                     while not is_good_sample:
#                         is_good_sample = True
#                         x = np.random.uniform(-x_lim, x_lim, 1)[0]
#                         y = np.random.uniform(-y_lim, y_lim, 1)[0]
#                         rand_loc = add_coords(table_location_partition2, (x, y, 0))
#                         for loc in all_blocks_loc:
#                             if distance(loc, rand_loc) < (2 * radius_collision_circle + eps_collision_circle):
#                                 is_good_sample = False
#                                 x_lim = x_lim - 0.0001
#                                 y_lim = y_lim + 0.0001
#                     all_blocks_loc.add(rand_loc)
#
#                     # Generate random orientation angle
#                     theta = np.random.uniform(0, 90, 1)[0]
#
#                     # Place body joint
#                     mg.write(
#                         "joint (table " + block_string + str(block_counter) + ")    { from=<T t"
#                         + str(rand_loc).replace(',', ' ') + " d("
#                         + str(theta) + " 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }\n")


if __name__ == '__main__':
    test_filename = "state_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1.g"
    test_file_path = os.path.join(CoreUtils.get_intermediate_files_dir_path(), "diff_22_2_1", test_filename)
    total_b_blocks = 21
    total_c_blocks = 0
    block_names = []
    if total_b_blocks > 0:
        block_names.extend([NameUtils.NAME_DIFF_B + str(num) for num in range(total_b_blocks)])
    if total_c_blocks > 0:
        block_names.extend([NameUtils.NAME_DIFF_C + str(num) for num in range(total_c_blocks)])

    GFileUtils.clean_g_file(test_file_path)
