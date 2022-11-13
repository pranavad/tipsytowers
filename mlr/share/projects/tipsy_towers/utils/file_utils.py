import csv
import os

from mlr.share.projects.tipsy_towers.utils.block_utils import BlockParams, BlockPosition, BlockQuaternion, \
    BlockShape
from mlr.share.projects.tipsy_towers.utils.compute_utils import is_close
from mlr.share.projects.tipsy_towers.utils.constants import NameUtils
from mlr.share.projects.tipsy_towers.utils.msg_utils import Msg


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


class FileUtils:
    @staticmethod
    def create_dir(dir_path):
        if os.path.exists(dir_path):
            return
        if os.path.isdir(dir_path):
            return
        os.mkdir(dir_path)

    @staticmethod
    def create_out_file(out_file_path):
        FileUtils.create_dir(os.path.dirname(out_file_path))
        file = open(out_file_path, 'wb')
        file.close()

    @staticmethod
    def write_content_to_file(out_file_path, file_contents_as_list):
        with open(out_file_path, "w") as file:
            for line in file_contents_as_list:
                file.write(line)

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

    @staticmethod
    def get_dir_list_in_directory(dir_path):
        try:
            return list(filter(os.path.isdir, [os.path.join(dir_path, i) for i in os.listdir(dir_path)]))
        except Exception as e:
            Msg.print_error("ERROR: issue in retrieving directory list within given directory " + dir_path)
            Msg.print_error(str(e))

    @staticmethod
    def get_files_in_directory(dir_path):
        try:
            return list(filter(os.path.isfile, [os.path.join(dir_path, i) for i in os.listdir(dir_path)]))
        except Exception as e:
            Msg.print_error("ERROR: issue in retrieving files within directory " + dir_path)
            Msg.print_error(str(e))


if __name__ == '__main__':
    pass
